QT += xml sql
TARGET = project2
TEMPLATE = app

CONFIG += qt warn_on release

win32 {
    LIBS +=  -lshell32
    RC_FILE =  project.rc
}

PROJECTNAME = project2
DESTDIR     = build/bin
OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
RCC_DIR     = build


INCLUDEPATH += src/h/ \
    src/project/ \
    src/helpdialog \
    src/tab


FORMS += src/ui/autoitems.ui \
     src/ui/docproperty.ui \
     src/helpdialog/helpdialog.ui \
     src/ui/itemproperty.ui \
     src/ui/linkproperty.ui \
     src/ui/mainwindow.ui \
     src/project/projectproperty.ui \
     src/project/projectsources.ui \
     src/tab/tabbedbrowser.ui \
     src/tab/tableproperty.ui \
     src/ui/topicchooser.ui \
     src/ui/cellsplit.ui \
     src/ui/imageproperty.ui \
     src/ui/bookadddialog.ui \
     src/ui/guiabout.ui \
     src/ui/settings.ui

SOURCES +=  src/project/projectproperty.cpp \
     src/project/projectsources.cpp \
     src/cpp/autoitems.cpp \
     src/cpp/config.cpp \
     src/cpp/docproperty.cpp \
     src/cpp/docuparser.cpp \
     src/cpp/fontsettingsdialog.cpp \
     src/helpdialog/helpdialog.cpp \
     src/helpdialog/helpwindow.cpp \
     src/cpp/index.cpp \
     src/cpp/itemproperty.cpp \
     src/cpp/linkproperty.cpp \
     src/cpp/main.cpp \
     src/cpp/mainwindow.cpp \
     src/cpp/pcommon.cpp \
     src/cpp/profile.cpp \
     src/cpp/raedit.cpp \
     src/cpp/settings.cpp \
     src/cpp/srclistparser.cpp \
     src/tab/tabbedbrowser.cpp \
     src/tab/tableproperty.cpp \
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
     src/helpdialog/helpdialog.h \
     src/helpdialog/helpwindow.h \
     src/h/index.h \
     src/h/itemproperty.h \
     src/h/linkproperty.h \
     src/h/mainwindow.h \
     src/h/pcommon.h \
     src/h/profile.h \
     src/project/projectproperty.h \
     src/project/projectsources.h \
     src/h/raedit.h \
     src/h/settings.h \
     src/h/srclistparser.h \
     src/tab/tabbedbrowser.h \
     src/tab/tableproperty.h \
     src/h/topicchooser.h \
     src/h/cellsplit.h \
     src/h/imageproperty.h\
     src/h/fontpanel.h \
     src/h/bookadddialog.h \
     src/h/guiabout.h

RESOURCES += resources/ra.qrc
TRANSLATIONS += lang/ra_rus.ts
