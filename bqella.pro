QT += xml sql
TARGET = bQella
TEMPLATE = app

CONFIG += qt warn_on release

win32 {
    LIBS +=  -lshell32
    RC_FILE =  resources/resources-win.rc
}

PROJECTNAME = bQella
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
    src/dialogs/findandreplacedialog \
    src/dialogs/projectproperty \
    src/dialogs/itemproperty \
    src/dialogs/linkproperty \
    src/dialogs/importdialog \
    src/dialogs/tagdialog \
    src/tab \
    src/settings \
    src/main \
    src/doc \
    src/export


FORMS += \
     src/doc/docproperty.ui \
     src/dialogs/helpdialog/helpdialog.ui \
     src/dialogs/itemproperty/itemproperty.ui \
     src/dialogs/linkproperty/linkproperty.ui \
     src/dialogs/bookdialog/bookadddialog.ui \
     src/dialogs/about/guiabout.ui \
     src/dialogs/projectproperty/projectproperty.ui \
     src/main/mainwindow.ui \
     src/tab/tabbedbrowser.ui \
     src/tab/tableproperty.ui \
     src/ui/cellsplit.ui \
     src/settings/settings.ui \
     src/dialogs/findandreplacedialog/frdialog.ui \
     src/dialogs/importdialog/importdialog.ui \
    src/dialogs/tagdialog/tagdialog.ui

SOURCES +=  \
     src/settings/config.cpp \
     src/settings/settings.cpp \
     src/settings/profile.cpp \
     src/doc/docproperty.cpp \
     src/doc/docuparser.cpp \
     src/main/main.cpp \
     src/main/mainwindow.cpp \
     src/tab/tabbedbrowser.cpp \
     src/tab/tableproperty.cpp \
     src/dialogs/projectproperty/projectproperty.cpp \
     src/dialogs/fontdialog/fontsettingsdialog.cpp \
     src/dialogs/helpdialog/helpdialog.cpp \
     src/dialogs/helpdialog/helpwindow.cpp \
     src/dialogs/fontdialog/fontpanel.cpp \
     src/dialogs/importdialog/importdialog.cpp \
     src/dialogs/findandreplacedialog/frdialog.cpp \
     src/dialogs/bookdialog/bookadddialog.cpp \
     src/dialogs/about/guiabout.cpp \
     src/dialogs/itemproperty/itemproperty.cpp \
     src/dialogs/linkproperty/linkproperty.cpp \
     src/cpp/index.cpp \
     src/cpp/pcommon.cpp \
     src/cpp/raedit.cpp \
     src/cpp/srclistparser.cpp \
     src/cpp/cellsplit.cpp \
     src/export/export.cpp \
    src/dialogs/tagdialog/tagdialog.cpp


HEADERS += \     
     src/doc/docproperty.h \
     src/doc/docuparser.h \
     src/dialogs/fontdialog/fontsettingsdialog.h \
     src/dialogs/helpdialog/helpdialog.h \
     src/dialogs/helpdialog/helpwindow.h \
     src/dialogs/fontdialog/fontpanel.h \
     src/dialogs/importdialog/importdialog.h \
     src/dialogs/findandreplacedialog/frdialog.h \
     src/dialogs/bookdialog/bookadddialog.h \
     src/dialogs/itemproperty/itemproperty.h \
     src/dialogs/linkproperty/linkproperty.h \
     src/dialogs/about/guiabout.h \
     src/dialogs/projectproperty/projectproperty.h \
     src/main/mainwindow.h \
     src/settings/settings.h \
     src/settings/profile.h \
     src/settings/config.h \
     src/tab/tabbedbrowser.h \
     src/tab/tableproperty.h \
     src/h/cellsplit.h \
     src/h/pcommon.h \
     src/h/index.h \
     src/h/raedit.h \
     src/h/srclistparser.h \
     src/export/export.h \
    src/dialogs/tagdialog/tagdialog.h


RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    lang/rus.ts
