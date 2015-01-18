/*
 * This file Copyright (C) 2008-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 * $Id: daemon.c 14270 2014-05-05 21:08:30Z jordan $
 */
#define LOG_TAG "TransmissionJNI"

#include <errno.h>
#include <stdio.h> /* printf */
#include <stdlib.h> /* exit, atoi */

#include <fcntl.h> /* open */
#include <signal.h>
#ifdef HAVE_SYSLOG
#include <syslog.h>
#endif
#include <unistd.h> /* daemon */

#include <event2/buffer.h>
#include <event2/event.h>

#include <native_log.h>

#include <libtransmission/transmission.h>
#include <libtransmission/tr-getopt.h>
#include <libtransmission/log.h>
#include <libtransmission/utils.h>
#include <libtransmission/variant.h>
#include <libtransmission/version.h>

#ifdef USE_SYSTEMD_DAEMON
 #include <systemd/sd-daemon.h>
#else
 static void sd_notify (int status UNUSED, const char * str UNUSED) { }
 static void sd_notifyf (int status UNUSED, const char * fmt UNUSED, ...) { }
#endif

#include "watch.h"

#define MY_NAME "transmission-daemon"

#define MEM_K 1024
#define MEM_K_STR "KiB"
#define MEM_M_STR "MiB"
#define MEM_G_STR "GiB"
#define MEM_T_STR "TiB"

#define DISK_K 1000
#define DISK_B_STR  "B"
#define DISK_K_STR "kB"
#define DISK_M_STR "MB"
#define DISK_G_STR "GB"
#define DISK_T_STR "TB"

#define SPEED_K 1000
#define SPEED_B_STR  "B/s"
#define SPEED_K_STR "kB/s"
#define SPEED_M_STR "MB/s"
#define SPEED_G_STR "GB/s"
#define SPEED_T_STR "TB/s"

#define LOGFILE_MODE_STR "a+"

static bool paused = false;
static bool seenHUP = false;
static const char *logfileName = NULL;
static FILE *logfile = NULL;
static tr_session * mySession = NULL;
static tr_quark key_pidfile = 0;
static struct event_base *ev_base = NULL;


static void
gotsig (int sig)
{
    switch (sig)
    {
        case SIGHUP:
        {
            if (!mySession)
            {
                tr_logAddInfo ("Deferring reload until session is fully started.");
                seenHUP = true;
            }
            else
            {
                tr_variant settings;
                const char * configDir;

                /* reopen the logfile to allow for log rotation */
                if (logfileName) {
                    logfile = freopen (logfileName, LOGFILE_MODE_STR, logfile);
                    if (!logfile)
                        fprintf (stderr, "Couldn't reopen \"%s\": %s\n", logfileName, tr_strerror (errno));
                }

                configDir = tr_sessionGetConfigDir (mySession);
                tr_logAddInfo ("Reloading settings from \"%s\"", configDir);
                tr_variantInitDict (&settings, 0);
                tr_variantDictAddBool (&settings, TR_KEY_rpc_enabled, true);
                tr_sessionLoadSettings (&settings, configDir, MY_NAME);
                tr_sessionSet (mySession, &settings);
                tr_variantFree (&settings);
                tr_sessionReloadBlocklists (mySession);
            }
            break;
        }

        default:
            tr_logAddError ("Unexpected signal (%d) in daemon, closing.", sig);
            /* no break */

        case SIGINT:
        case SIGTERM:
            event_base_loopexit(ev_base, NULL);
            break;
    }
}

#if defined (WIN32)
 #define USE_NO_DAEMON
#elif !defined (HAVE_DAEMON) || defined (__UCLIBC__)
 #define USE_TR_DAEMON
#else
 #define USE_OS_DAEMON
#endif

static int
tr_daemon (int nochdir, int noclose)
{
#if defined (USE_OS_DAEMON)

    return daemon (nochdir, noclose);

#elif defined (USE_TR_DAEMON)

    /* this is loosely based off of glibc's daemon () implementation
     * http://sourceware.org/git/?p=glibc.git;a=blob_plain;f=misc/daemon.c */

    switch (fork ()) {
        case -1: return -1;
        case 0: break;
        default: _exit (0);
    }

    if (setsid () == -1)
        return -1;

    if (!nochdir)
        chdir ("/");

    if (!noclose) {
        int fd = open ("/dev/null", O_RDWR, 0);
        dup2 (fd, STDIN_FILENO);
        dup2 (fd, STDOUT_FILENO);
        dup2 (fd, STDERR_FILENO);
        close (fd);
    }

    return 0;

#else /* USE_NO_DAEMON */
    return 0;
#endif
}

static char *userConfigDir = NULL;

extern void setUserConfigDir(const char *dir) {
    if (userConfigDir != NULL) {
        free (userConfigDir);
    }

    userConfigDir = strdup(dir);
}

static const char*
getConfigDir (int argc, const char ** argv)
{
    (void)argc;
    (void)argv;

    const char * configDir = NULL;
    if (userConfigDir == NULL) {
        configDir = tr_getDefaultConfigDir (MY_NAME);
    } else {
        configDir = userConfigDir;
    }
    return configDir;
}

static void
onFileAdded (tr_session * session, const char * dir, const char * file)
{
    char * filename = tr_buildPath (dir, file, NULL);
    tr_ctor * ctor = tr_ctorNew (session);
    int err = tr_ctorSetMetainfoFromFile (ctor, filename);

    if (!err)
    {
        tr_torrentNew (ctor, &err, NULL);

        if (err == TR_PARSE_ERR)
            tr_logAddError ("Error parsing .torrent file \"%s\"", file);
        else
        {
            bool trash = false;
            int test = tr_ctorGetDeleteSource (ctor, &trash);

            tr_logAddInfo ("Parsing .torrent file successful \"%s\"", file);

            if (!test && trash)
            {
                tr_logAddInfo ("Deleting input .torrent file \"%s\"", file);
                if (tr_remove (filename))
                    tr_logAddError ("Error deleting .torrent file: %s", tr_strerror (errno));
            }
            else
            {
                char * new_filename = tr_strdup_printf ("%s.added", filename);
                tr_rename (filename, new_filename);
                tr_free (new_filename);
            }
        }
    }

    tr_ctorFree (ctor);
    tr_free (filename);
}

static void
printMessage (FILE * logfile, int level, const char * name, const char * message, const char * file, int line)
{
    if (logfile != NULL)
    {
        char timestr[64];
        tr_logGetTimeStr (timestr, sizeof (timestr));
        if (name)
            fprintf (logfile, "[%s] %s %s (%s:%d)\n", timestr, name, message, file, line);
        else
            fprintf (logfile, "[%s] %s (%s:%d)\n", timestr, message, file, line);
    }
#ifdef HAVE_SYSLOG
    else /* daemon... write to syslog */
    {
        int priority;

        /* figure out the syslog priority */
        switch (level) {
            case TR_LOG_ERROR: priority = LOG_ERR; break;
            case TR_LOG_DEBUG: priority = LOG_DEBUG; break;
            default:           priority = LOG_INFO; break;
        }

        if (name)
            syslog (priority, "%s %s (%s:%d)", name, message, file, line);
        else
            syslog (priority, "%s (%s:%d)", message, file, line);
    }
#endif
}

static void
pumpLogMessages (FILE * logfile)
{
    const tr_log_message * l;
    tr_log_message * list = tr_logGetQueue ();

    for (l=list; l!=NULL; l=l->next)
        printMessage (logfile, l->level, l->name, l->message, l->file, l->line);

    if (logfile != NULL)
        fflush (logfile);

    tr_logFreeQueue (list);
}

static void
reportStatus (void)
{
    const double up = tr_sessionGetRawSpeed_KBps (mySession, TR_UP);
    const double dn = tr_sessionGetRawSpeed_KBps (mySession, TR_DOWN);

    if (up>0 || dn>0)
	sd_notifyf (0, "STATUS=Uploading %.2f KBps, Downloading %.2f KBps.\n", up, dn);
    else
	sd_notify (0, "STATUS=Idle.\n");
}

static void
periodicUpdate (evutil_socket_t fd UNUSED, short what UNUSED, void *watchdir)
{
    dtr_watchdir_update (watchdir);

    pumpLogMessages (logfile);

    reportStatus ();
}

static tr_rpc_callback_status
on_rpc_callback (tr_session            * session UNUSED,
                 tr_rpc_callback_type    type,
                 struct tr_torrent     * tor UNUSED,
                 void                  * user_data UNUSED)
{
    if (type == TR_RPC_SESSION_CLOSE)
        event_base_loopexit(ev_base, NULL);
    return TR_RPC_OK;
}

int tr_checkConfig() {
    tr_variant settings;
    tr_variantInitDict(&settings, 0);

    return tr_sessionLoadSettings(&settings, getConfigDir(0, NULL), MY_NAME);
}

int
tr_main (int argc, char ** argv)
{
    int c;
    const char * optarg;
    tr_variant settings;
    bool boolVal;
    bool loaded;
    bool foreground = false;
    bool dumpSettings = false;
    const char * configDir = NULL;
    const char * pid_filename;
    dtr_watchdir * watchdir = NULL;
    bool pidfile_created = false;
    tr_session * session = NULL;
    struct event *status_ev;
    LOGD("here tr_main() started\n");
    key_pidfile = tr_quark_new ("pidfile",  7);

    signal (SIGINT, gotsig);
    signal (SIGTERM, gotsig);
#ifndef WIN32
    signal (SIGHUP, gotsig);
#endif

    /* load settings from defaults + config file */
    tr_variantInitDict (&settings, 0);
    tr_variantDictAddBool (&settings, TR_KEY_rpc_enabled, true);
    tr_variantDictAddBool (&settings, TR_KEY_blocklist_enabled, false);
    configDir = getConfigDir (argc, (const char**)argv);
    loaded = tr_sessionLoadSettings (&settings, configDir, MY_NAME);
    LOGD("settings loaded finished\n");
    /* overwrite settings from the comamndline */
    tr_optind = 1;
    if (foreground && !logfile)
        logfile = stderr;

    if (!loaded)
    {
        printMessage (logfile, TR_LOG_ERROR, MY_NAME, "Error loading config file -- exiting.", __FILE__, __LINE__);
        LOGD("Error loading config file -- exiting\n");
        return -1;
    }

    if (dumpSettings)
    {
        char * str = tr_variantToStr (&settings, TR_VARIANT_FMT_JSON, NULL);
        fprintf (stderr, "%s", str);
        tr_free (str);
        return 0;
    }
#if 0
    if (!foreground && tr_daemon (true, false) < 0)
    {
        char buf[256];
        tr_snprintf (buf, sizeof (buf), "Failed to daemonize: %s", tr_strerror (errno));
        printMessage (logfile, TR_LOG_ERROR, MY_NAME, buf, __FILE__, __LINE__);
        exit (1);
    }
#endif
    sd_notifyf (0, "MAINPID=%d\n", (int)getpid()); 

    LOGD("setup event state");
    /* setup event state */
    ev_base = event_base_new();
    if (ev_base == NULL)
    {
        char buf[256];
        tr_snprintf(buf, sizeof(buf), "Failed to init daemon event state: %s", tr_strerror(errno));
        printMessage (logfile, TR_LOG_ERROR, MY_NAME, buf, __FILE__, __LINE__);
        exit (1);
    }

    LOGD("start the session\n");
    /* start the session */
    tr_formatter_mem_init (MEM_K, MEM_K_STR, MEM_M_STR, MEM_G_STR, MEM_T_STR);
    tr_formatter_size_init (DISK_K, DISK_K_STR, DISK_M_STR, DISK_G_STR, DISK_T_STR);
    tr_formatter_speed_init (SPEED_K, SPEED_K_STR, SPEED_M_STR, SPEED_G_STR, SPEED_T_STR);
    LOGD("session init\n");
    session = tr_sessionInit ("daemon", configDir, true, &settings);
    LOGD("set RPC Callback\n");
    tr_sessionSetRPCCallback (session, on_rpc_callback, NULL);
    tr_logAddNamedInfo (NULL, "Using settings from \"%s\"", configDir);
    LOGD("save settings\n");
    tr_sessionSaveSettings (session, configDir, &settings);
    LOGD("save settings finished\n");

    pid_filename = NULL;
    tr_variantDictFindStr (&settings, key_pidfile, &pid_filename, NULL);
    LOGD("save pidfile\n");
    if (pid_filename && *pid_filename)
    {
        FILE * fp = fopen (pid_filename, "w+");
        if (fp != NULL)
        {
            fprintf (fp, "%d", (int)getpid ());
            fclose (fp);
            tr_logAddInfo ("Saved pidfile \"%s\"", pid_filename);
            pidfile_created = true;
        }
        else
            tr_logAddError ("Unable to save pidfile \"%s\": %s", pid_filename, tr_strerror (errno));
    }

    if (tr_variantDictFindBool (&settings, TR_KEY_rpc_authentication_required, &boolVal) && boolVal)
        tr_logAddNamedInfo (MY_NAME, "requiring authentication");

    mySession = session;

    /* If we got a SIGHUP during startup, process that now. */
    if (seenHUP)
        gotsig (SIGHUP);

    LOGD("add a watch dir");
    /* maybe add a watchdir */
    {
        const char * dir;

        if (tr_variantDictFindBool (&settings, TR_KEY_watch_dir_enabled, &boolVal)
            && boolVal
            && tr_variantDictFindStr (&settings, TR_KEY_watch_dir, &dir, NULL)
            && dir
            && *dir)
        {
            tr_logAddInfo ("Watching \"%s\" for new .torrent files", dir);
            watchdir = dtr_watchdir_new (mySession, dir, onFileAdded);
        }
    }

    LOGD("load the torrents\n");
    /* load the torrents */
    {
        tr_torrent ** torrents;
        tr_ctor * ctor = tr_ctorNew (mySession);
        if (paused)
            tr_ctorSetPaused (ctor, TR_FORCE, true);
        torrents = tr_sessionLoadTorrents (mySession, ctor, NULL);
        tr_free (torrents);
        tr_ctorFree (ctor);
    }

#ifdef HAVE_SYSLOG
    if (!foreground)
        openlog (MY_NAME, LOG_CONS|LOG_PID, LOG_DAEMON);
#endif

    /* Create new timer event to report daemon status */
    {
        struct timeval one_sec = { 1, 0 };
        status_ev = event_new(ev_base, -1, EV_PERSIST, &periodicUpdate, watchdir);
        if (status_ev == NULL)
        {
            tr_logAddError("Failed to create status event %s", tr_strerror(errno));
            goto cleanup;
        }
        if (event_add(status_ev, &one_sec) == -1)
        {
            tr_logAddError("Failed to add status event %s", tr_strerror(errno));
            goto cleanup;
        }
    }

    sd_notify( 0, "READY=1\n" );

    LOGD("start event loop here\n");
    /* Run daemon event loop */
    if (event_base_dispatch(ev_base) == -1)
    {
        tr_logAddError("Failed to launch daemon event loop: %s", tr_strerror(errno));
	goto cleanup;
    }

cleanup:
    sd_notify( 0, "STATUS=Closing transmission session...\n" );
    printf ("Closing transmission session...");

    if (status_ev)
    {
        event_del(status_ev);
        event_free(status_ev);
    }
    event_base_free(ev_base);

    tr_sessionSaveSettings (mySession, configDir, &settings);
    dtr_watchdir_free (watchdir);
    tr_sessionClose (mySession);
    pumpLogMessages (logfile);
    printf (" done.\n");

    /* shutdown */
#if HAVE_SYSLOG
    if (!foreground)
    {
        syslog (LOG_INFO, "%s", "Closing session");
        closelog ();
    }
#endif

    /* cleanup */
    if (pidfile_created)
        tr_remove (pid_filename);
    tr_variantFree (&settings);
    sd_notify (0, "STATUS=\n");
    return 0;
}
