TEMPLATE = app

QT += qml quick widgets

SOURCES += Main.cxx \
           AndroidQt.cxx \
           AndroidQt_Window.cxx \
           AndroidQt_TouchParameters.cxx

RESOURCES += AndroidQt.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# OCCT
include(OCCT.pri)

# Default rules for deployment.
include(Deployment.pri)

HEADERS += \
           AndroidQt.h \
           AndroidQt_Window.h \
           AndroidQt_TouchParameters.h \
           AndroidQt_UserInteractionParameters.h

OTHER_FILES += \
    android/src/org/qtproject/example/AndroidQt/AndroidQt.java \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
