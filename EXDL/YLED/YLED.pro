QT += core gui widgets network sql

TARGET = YLED
TEMPLATE = app
CONFIG += c++17
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/ExperimentService.cpp \
    src/Manifest.cpp \
    src/ExperimentTile.cpp \
    src/NavBar.cpp \
    src/PageExperiments.cpp \
    src/PageExperimentDetail.cpp \
    src/PageSettings.cpp \
    src/MainWindow.cpp

HEADERS += \
    src/Experiment.h \
    src/ExperimentService.h \
    src/Manifest.h \
    src/ExperimentTile.h \
    src/NavBar.h \
    src/PageExperiments.h \
    src/PageExperimentDetail.h \
    src/PageSettings.h \
    src/MainWindow.h \
    src/Theme.h

RESOURCES += resources/app.qrc
