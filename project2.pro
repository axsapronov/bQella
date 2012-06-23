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
    src/dialogs/helpdialog \
    src/dialogs/about \
    src/dialogs/fontdialog \
    src/dialogs/bookdialog \
    src/tab \
    src/settings \
    src/main \
    src/doc \
    src/export


FORMS += src/ui/autoitems.ui \
     src/doc/docproperty.ui \
     src/dialogs/helpdialog/helpdialog.ui \
     src/ui/itemproperty.ui \
     src/ui/linkproperty.ui \
     src/main/mainwindow.ui \
     src/project/projectproperty.ui \
     src/tab/tabbedbrowser.ui \
     src/tab/tableproperty.ui \
     src/ui/topicchooser.ui \
     src/ui/cellsplit.ui \
     src/ui/imageproperty.ui \
     src/dialogs/bookdialog/bookadddialog.ui \
     src/dialogs/about/guiabout.ui \
     src/settings/settings.ui

SOURCES +=  src/project/projectproperty.cpp \
     src/cpp/autoitems.cpp \
     src/settings/config.cpp \
     src/doc/docproperty.cpp \
     src/doc/docuparser.cpp \
     src/dialogs/fontdialog/fontsettingsdialog.cpp \
     src/dialogs/helpdialog/helpdialog.cpp \
     src/dialogs/helpdialog/helpwindow.cpp \
     src/cpp/index.cpp \
     src/cpp/itemproperty.cpp \
     src/cpp/linkproperty.cpp \
     src/main/main.cpp \
     src/main/mainwindow.cpp \
     src/cpp/pcommon.cpp \
     src/settings/profile.cpp \
     src/cpp/raedit.cpp \
     src/settings/settings.cpp \
     src/cpp/srclistparser.cpp \
     src/tab/tabbedbrowser.cpp \
     src/tab/tableproperty.cpp \
     src/cpp/topicchooser.cpp \
     src/cpp/cellsplit.cpp \
     src/cpp/imageproperty.cpp\
     src/dialogs/fontdialog/fontpanel.cpp \
     src/dialogs/bookdialog/bookadddialog.cpp \
     src/dialogs/about/guiabout.cpp

HEADERS += src/h/autoitems.h \
     src/settings/config.h \
     src/doc/docproperty.h \
     src/doc/docuparser.h \
     src/dialogs/fontdialog/fontsettingsdialog.h \
     src/dialogs/helpdialog/helpdialog.h \
     src/dialogs/helpdialog/helpwindow.h \
     src/main/mainwindow.h \
     src/project/projectproperty.h \
     src/settings/settings.h \
     src/tab/tabbedbrowser.h \
     src/tab/tableproperty.h \
     src/dialogs/fontdialog/fontpanel.h \
     src/dialogs/bookdialog/bookadddialog.h \
     src/dialogs/about/guiabout.h \
     src/h/topicchooser.h \
     src/h/cellsplit.h \
     src/h/imageproperty.h \
     src/h/pcommon.h \
     src/settings/profile.h \
     src/h/index.h \
     src/h/itemproperty.h \
     src/h/linkproperty.h \
     src/h/raedit.h \
     src/h/srclistparser.h


TRANSLATIONS += resources/lang/rus.ts

RESOURCES += \
    resources/recources.qrc
