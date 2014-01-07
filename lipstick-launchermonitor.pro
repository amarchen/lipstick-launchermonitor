TEMPLATE = lib

# The name of your app
TARGET = lipstick-launchermonitor

# It is supposed to be deployed by your main app next to your main project.
# For now main project dir is hardcoded here.. Not sure how solve it elegantly.
# Maybe skip this setting and require it to be set by the higher level .pro or .yaml/spec?
TARGETPATH = /usr/share/harbour-launcher/lib
target.path = $$TARGETPATH

CONFIG += link_pkgconfig
PKGCONFIG += mlite5

# C++ sources
SOURCES += \
    src/lipstick-components/launcheritem.cpp \
    src/lipstick-components/launchermodel.cpp \
    src/lipstick-components/launchermonitor.cpp \
    src/lipstick-components/qobjectlistmodel.cpp

# C++ headers
HEADERS += \
    src/lipstick-components/launcheritem.h \
    src/lipstick-components/launchermodel.h \
    src/lipstick-components/launchermonitor.h \
    src/lipstick-components/qobjectlistmodel.h \
    src/lipstick-components/lipstickglobal.h

INSTALLS += target


