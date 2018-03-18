TEMPLATE = app

QT += qml quick widgets

SOURCES += src/Main.cxx \
           src/AndroidQt.cxx \
           src/AndroidQt_Window.cxx \
           src/AndroidQt_TouchParameters.cxx

RESOURCES += ./src/AndroidQt.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# OCCT
include(OCCT.pri)

# Default rules for deployment.
include(Deployment.pri)

HEADERS += \
           src/AndroidQt.h \
           src/AndroidQt_Window.h \
           src/AndroidQt_TouchParameters.h \
           src/AndroidQt_UserInteractionParameters.h

INCLUDEPATH += $$_PRO_FILE_PWD_/src
DEPENDPATH  += $$_PRO_FILE_PWD_/src

OTHER_FILES += \
    android/src/org/qtproject/example/AndroidQt/AndroidQt.java \
    android/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
