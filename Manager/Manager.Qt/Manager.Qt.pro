QT += core gui widgets sql

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = app
TARGET = ManagerQt

DESTDIR = $$PWD/bin

CONFIG(debug, debug|release) {
    OBJECTS_DIR = $$PWD/build/debug/obj
    MOC_DIR     = $$PWD/build/debug/moc
    RCC_DIR     = $$PWD/build/debug/rcc
    UI_DIR      = $$PWD/build/debug/ui
} else {
    OBJECTS_DIR = $$PWD/build/release/obj
    MOC_DIR     = $$PWD/build/release/moc
    RCC_DIR     = $$PWD/build/release/rcc
    UI_DIR      = $$PWD/build/release/ui
}

# Used to locate the repository root (and therefore LabManager.db) at runtime.
DEFINES += LAB_SOURCE_DIR=\\\"$$PWD\\\"

INCLUDEPATH += $$PWD/src

SOURCES += \
    src/main.cpp \
    src/core/DatabaseConfig.cpp \
    src/core/ExperimentService.cpp \
    src/core/Theme.cpp \
    src/models/ExperimentFilterProxy.cpp \
    src/models/ExperimentTableModel.cpp \
    src/ui/Blueprint.cpp \
    src/ui/Chips.cpp \
    src/ui/Delegates.cpp \
    src/ui/DetailsPanel.cpp \
    src/ui/Dialogs.cpp \
    src/ui/ExperimentDialog.cpp \
    src/ui/MainWindow.cpp

HEADERS += \
    src/core/DatabaseConfig.h \
    src/core/Experiment.h \
    src/core/ExperimentService.h \
    src/core/Theme.h \
    src/models/ExperimentFilterProxy.h \
    src/models/ExperimentTableModel.h \
    src/ui/Blueprint.h \
    src/ui/Chips.h \
    src/ui/Delegates.h \
    src/ui/DetailsPanel.h \
    src/ui/Dialogs.h \
    src/ui/ExperimentDialog.h \
    src/ui/MainWindow.h \
    src/ui/NavItem.h
