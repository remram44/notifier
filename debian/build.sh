#!/bin/sh

if [ $(id -u) -ne  0 ] ; then
    exec sudo "$0"
fi

cd ..
hg pull http://chewie.rez-gif.supelec.fr/hg/notifier && hg up -C -r stable
sed -i 's/^#define PREFIX.*$/#define PREFIX "\/usr"/' *.h
mkdir -p debian/notifier/usr/bin
mkdir -p debian/notifier/usr/share/notifier

qmake && make && lrelease notifier.pro
cp notifier debian/notifier/usr/bin/
cp beep.wav icon.png icon2.png notifier_fr.qm debian/notifier/usr/share/notifier/

cd debian/notifier
rm -f DEBIAN/md5sums
for n in $(find usr -type f) ; do
    md5sum "$n" >> DEBIAN/md5sums
done

cd ..
sudo chown -R root:root notifier
dpkg-deb -b notifier notifier.deb
