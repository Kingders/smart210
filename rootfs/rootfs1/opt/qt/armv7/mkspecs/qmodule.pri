CONFIG +=  cross_compile compile_examples qpa largefile precompile_header enable_new_dtags pcre
QT_BUILD_PARTS += libs examples
QT_NO_DEFINES =  CUPS EGL EGLFS EGL_X11 FONTCONFIG GLIB IMAGEFORMAT_JPEG LIBPROXY OPENGL OPENVG PULSEAUDIO STYLE_GTK XRENDER ZLIB
QT_QCONFIG_PATH = 
host_build {
    QT_CPU_FEATURES.x86_64 =  mmx sse sse2
} else {
    QT_CPU_FEATURES.arm = 
}
QT_COORD_TYPE = float
QT_LFLAGS_ODBC   = -lodbc
styles += mac fusion windows
DEFINES += QT_NO_MTDEV
DEFINES += QT_NO_LIBUDEV
DEFINES += QT_NO_LIBINPUT
QMAKE_X11_PREFIX = /usr
sql-drivers = 
sql-plugins =  sqlite
