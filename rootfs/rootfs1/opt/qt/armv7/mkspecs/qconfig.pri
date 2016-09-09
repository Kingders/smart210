#configuration
CONFIG +=  cross_compile shared qpa no_mocdepend release qt_no_framework
host_build {
    QT_ARCH = x86_64
    QT_TARGET_ARCH = arm
} else {
    QT_ARCH = arm
    QMAKE_DEFAULT_LIBDIRS = /opt/FriendlyARM/toolschain/4.5.1/lib/gcc/arm-none-linux-gnueabi/4.5.1 /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/lib /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/sys-root/lib /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/sys-root/usr/lib
    QMAKE_DEFAULT_INCDIRS = /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/include/c++/4.5.1 /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/include/c++/4.5.1/arm-none-linux-gnueabi /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/include/c++/4.5.1/backward /opt/FriendlyARM/toolschain/4.5.1/lib/gcc/arm-none-linux-gnueabi/4.5.1/include /opt/FriendlyARM/toolschain/4.5.1/lib/gcc/arm-none-linux-gnueabi/4.5.1/include-fixed /opt/FriendlyARM/toolschain/4.5.1/arm-none-linux-gnueabi/include
}
QT_CONFIG +=  minimal-config small-config medium-config large-config full-config no-pkg-config evdev tslib linuxfb accessibility shared qpa reduce_exports clock-gettime clock-monotonic posix_fallocate mremap getaddrinfo ipv6ifname getifaddrs inotify eventfd threadsafe-cloexec system-jpeg system-png png freetype harfbuzz system-zlib nis iconv dbus openssl rpath alsa concurrent audio-backend release

#versioning
QT_VERSION = 5.6.0
QT_MAJOR_VERSION = 5
QT_MINOR_VERSION = 6
QT_PATCH_VERSION = 0

#namespaces
QT_LIBINFIX = 
QT_NAMESPACE = 

QT_EDITION = OpenSource

QT_COMPILER_STDCXX = 
QT_GCC_MAJOR_VERSION = 4
QT_GCC_MINOR_VERSION = 5
QT_GCC_PATCH_VERSION = 1
