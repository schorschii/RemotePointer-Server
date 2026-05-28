#!/bin/bash
set -e

# build .deb package
INSTALLDIR=/usr/share/remotepointer
BUILDDIR=remotepointer

# check root permissions
if [ "$EUID" -ne 0 ] && ! groups | grep -q sudo ; then
	echo "Please run this script as root!"
	#exit 1 # disabled for github workflow. don't know why this check fails here but sudo works.
fi

# cd to working dir
cd "$(dirname "$0")"

# empty / create necessary directories
if [ -d "$BUILDDIR/$INSTALLDIR" ]; then
    sudo rm -r $BUILDDIR/$INSTALLDIR
fi

# copy files in place
sudo mkdir -p $BUILDDIR/$INSTALLDIR/
sudo cp      ../../RemotePointerServer.py   $BUILDDIR/$INSTALLDIR/
sudo cp -r   ../../res                      $BUILDDIR/$INSTALLDIR/res

# make binary available in PATH
sudo mkdir -p $BUILDDIR/usr/bin
sudo ln -sf   $INSTALLDIR/RemotePointerServer.py   $BUILDDIR/usr/bin/remotepointer

# set file permissions
sudo chown -R root:root $BUILDDIR

# build deb
sudo dpkg-deb -Zxz --build $BUILDDIR

echo "Build finished"
