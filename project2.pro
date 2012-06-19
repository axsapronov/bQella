

QT += xml sql
TARGET = project2
TEMPLATE = app

CONFIG += qt warn_on release

win32 {
    LIBS +=  -lshell32
    RC_FILE =  ra.rc
}

PROJECTNAME = project2
DESTDIR     = build/bin
OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
RCC_DIR     = build

INCLUDEPATH = src/h/

FORMS += src/ui/autoitems.ui \
     src/ui/docproperty.ui \
     src/ui/helpdialog.ui \
     src/ui/itemproperty.ui \
     src/ui/linkproperty.ui \
     src/ui/mainwindow.ui \
     src/ui/projectproperty.ui \
     src/ui/projectsources.ui \
     src/ui/settings.ui \
     src/ui/tabbedbrowser.ui \
     src/ui/tableproperty.ui \
     src/ui/topicchooser.ui \
     src/ui/cellsplit.ui \
     src/ui/imageproperty.ui \
    src/ui/bookadddialog.ui \
    src/ui/guiabout.ui

SOURCES += src/cpp/autoitems.cpp \
     src/cpp/config.cpp \
     src/cpp/docproperty.cpp \
     src/cpp/docuparser.cpp \
     src/cpp/fontsettingsdialog.cpp \
     src/cpp/helpdialog.cpp \
     src/cpp/helpwindow.cpp \
     src/cpp/index.cpp \
     src/cpp/itemproperty.cpp \
     src/cpp/linkproperty.cpp \
     src/cpp/main.cpp \
     src/cpp/mainwindow.cpp \
     src/cpp/pcommon.cpp \
     src/cpp/profile.cpp \
     src/cpp/projectproperty.cpp \
     src/cpp/projectsources.cpp \
     src/cpp/raedit.cpp \
     src/cpp/settings.cpp \
     src/cpp/srclistparser.cpp \
     src/cpp/tabbedbrowser.cpp \
     src/cpp/tableproperty.cpp \
     src/cpp/topicchooser.cpp \
     src/cpp/cellsplit.cpp \
     src/cpp/imageproperty.cpp\
     src/cpp/fontpanel.cpp \
     src/cpp/bookadddialog.cpp \
    src/cpp/guiabout.cpp

HEADERS += src/h/autoitems.h \
     src/h/config.h \
     src/h/docproperty.h \
     src/h/docuparser.h \
     src/h/fontsettingsdialog.h \
     src/h/helpdialog.h \
     src/h/helpwindow.h \
     src/h/index.h \
     src/h/itemproperty.h \
     src/h/linkproperty.h \
     src/h/mainwindow.h \
     src/h/pcommon.h \
     src/h/profile.h \
     src/h/projectproperty.h \
     src/h/projectsources.h \
     src/h/raedit.h \
     src/h/settings.h \
     src/h/srclistparser.h \
     src/h/tabbedbrowser.h \
     src/h/tableproperty.h \
     src/h/topicchooser.h \
     src/h/cellsplit.h \
     src/h/imageproperty.h\
     src/h/fontpanel.h \
    src/h/bookadddialog.h \
    src/h/guiabout.h

RESOURCES += src/ra.qrc
TRANSLATIONS += lang/ra_rus.ts
