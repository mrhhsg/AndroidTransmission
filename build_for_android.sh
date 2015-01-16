#!/bin/sh

# directories
SOURCE="transmission-2.84"
CURL_SOURCE="curl-7.40.0"
LIBEVENT_SOURCE="libevent-2.0.21-stable"

FAT="TS-Android"

SCRATCH="scratch"
# must be an absolute path
THIN=`pwd`/"thin"

NDK="/opt/google/android-ndk-r9"

SYSROOT_PREFIX="$NDK/platforms/android-14/arch-"


CONFIGURE_FLAGS="--enable-largefile --enable-utp --enable-lightweight --build=\"x86_64-unknown-linux-gnu\""


ARCHS="arm"

COMPILE="y"
#LIPO="y"

DEPLOYMENT_TARGET="6.0"

build_libevent() {
    ARCH="$1"
    PLATFROM="$2"

    if [ -z $PLATFROM ]
    then
        PLATFROM="linux-androideabi"
    fi

    if [ $ARCH = "arm64" ]
    then
        PLATFROM="linux-android"
        ACT_ARCH="aarch64"
    else
        ACT_ARCH=$ARCH
    fi

    SYSROOT=${SYSROOT_PREFIX}${ARCH}
    CC="$NDK/toolchains/${ACT_ARCH}-${PLATFROM}-4.6/prebuilt/linux-x86_64/bin/${ACT_ARCH}-${PLATFROM}-gcc --sysroot=${SYSROOT}"

    export CC="$CC"

    CWD=`pwd`

    echo "building curl for $ARCH..."
    mkdir -p "$SCRATCH/$ARCH"
    cd "$SCRATCH/$ARCH"
    CFLAGS="-I${SYSROOT_PREFIX}${ARCH}/usr/include"
    export CFLAGS="$CFLAGS"
    $CWD/$LIBEVENT_SOURCE/configure --host=$ACT_ARCH-$PLATFROM \
        --prefix=$THIN/$ARCH
    make -j4 install
}

#build_libevent "arm"

build_Curl() {
    ARCH="$1"
    PLATFROM="$2"

    if [ -z $PLATFROM ]
    then
        PLATFROM="linux-androideabi"
    fi

    if [ $ARCH = "arm64" ]
    then
        PLATFROM="linux-android"
        ACT_ARCH="aarch64"
    else
        ACT_ARCH=$ARCH
    fi

    SYSROOT=${SYSROOT_PREFIX}${ARCH}
    CC="$NDK/toolchains/${ACT_ARCH}-${PLATFROM}-4.6/prebuilt/linux-x86_64/bin/${ACT_ARCH}-${PLATFROM}-gcc --sysroot=${SYSROOT}"

    export CC="$CC"

    CWD=`pwd`

    echo "building curl for $ARCH..."
    mkdir -p "$SCRATCH/$ARCH"
    cd "$SCRATCH/$ARCH"
    CFLAGS="-I${SYSROOT_PREFIX}${ARCH}/usr/include"
    export CFLAGS="$CFLAGS"
    $CWD/$CURL_SOURCE/configure --host=$ACT_ARCH-$PLATFROM \
        --with-ssl \
        --prefix=$THIN/$ARCH
    make -j4 install
}

#build_Curl "arm"

if [ "$COMPILE" ]
then
	CWD=`pwd`
	for ARCH in $ARCHS
	do
		ACT_ARCH=$ARCH
		PLATFROM="linux-androideabi"
		if [ $ARCH = "arm64" ]
		then
			ACT_ARCH="aarch64"
			PLATFROM="linux-android"
		fi

		echo "building $ARCH..."
		mkdir -p "$SCRATCH/$ARCH"
		cd "$SCRATCH/$ARCH"

        CFLAGS="-I${SYSROOT_PREFIX}${ARCH}/usr/include -I${THIN}/$ARCH/include"

        export LIBCURL_CFLAGS="-I${THIN}/$ARCH/include"
        export LIBCURL_LIBS="-L${THIN}/$ARCH/lib"
        export LIBEVENT_CFLAGS="-I${THIN}/$ARCH/include"
        export LIBEVENT_LIBS="-L${THIN}/$ARCH/lib"

        SYSROOT=${SYSROOT_PREFIX}${ARCH}

		CC="$NDK/toolchains/${ACT_ARCH}-${PLATFROM}-4.6/prebuilt/linux-x86_64/bin/${ACT_ARCH}-${PLATFROM}-gcc --sysroot=${SYSROOT}"
        export CC="${CC}"
		CXXFLAGS="$CFLAGS"
        LDFLAGS="-L${SYSROOT_PREFIX}${ARCH}/usr/lib -L${THIN}/$ARCH/lib"
        export CFLAGS="$CFLAGS"
        export LDFLAGS="$LDFLAGS"
		$CWD/$SOURCE/configure \
            $CONFIGURE_FLAGS \
            --host=arm-linux-androideabi \
            --prefix=$THIN/$ARCH || exit 1;
        make clean;
		make -j4 install || exit 1
		cd $CWD
	done
fi

if [ "$LIPO" ]
then
	echo "building fat binaries..."
	mkdir -p $FAT/lib
	set - $ARCHS
	CWD=`pwd`
	cd $THIN/$1/lib
	for LIB in *.a
	do
		cd $CWD
		echo lipo -create `find $THIN -name $LIB` -output $FAT/lib/$LIB 1>&2
		lipo -create `find $THIN -name $LIB` -output $FAT/lib/$LIB || exit 1
	done

	cd $CWD
	cp -rf $THIN/$1/include $FAT
fi

echo Done
