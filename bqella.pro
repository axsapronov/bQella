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

DEPENDPATH += \
    src/cpp \
    src/h \
    src/ui \
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


INCLUDEPATH += \
    src/h/ \
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
     docproperty.ui \
     helpdialog.ui \
     itemproperty.ui \
     linkproperty.ui \
     bookadddialog.ui \
     guiabout.ui \
     projectproperty.ui \
     mainwindow.ui \
     tabbedbrowser.ui \
     tableproperty.ui \
     cellsplit.ui \
     settings.ui \
     frdialog.ui \
     importdialog.ui \
     tagdialog.ui

SOURCES +=  \
     config.cpp \
     settings.cpp \
     profile.cpp \
     docproperty.cpp \
     docuparser.cpp \
     main.cpp \
     mainwindow.cpp \
     tabbedbrowser.cpp \
     tableproperty.cpp \
     projectproperty.cpp \
     fontsettingsdialog.cpp \
     helpdialog.cpp \
     helpwindow.cpp \
     fontpanel.cpp \
     importdialog.cpp \
     frdialog.cpp \
     bookadddialog.cpp \
     guiabout.cpp \
     itemproperty.cpp \
     linkproperty.cpp \
     index.cpp \
     pcommon.cpp \
     raedit.cpp \
     srclistparser.cpp \
     cellsplit.cpp \
     export.cpp \
     tagdialog.cpp


HEADERS += \     
     docproperty.h \
     docuparser.h \
     fontsettingsdialog.h \
     helpdialog.h \
     helpwindow.h \
     fontpanel.h \
     importdialog.h \
     frdialog.h \
     bookadddialog.h \
     itemproperty.h \
     linkproperty.h \
     guiabout.h \
     projectproperty.h \
     mainwindow.h \
     settings.h \
     profile.h \
     config.h \
     tabbedbrowser.h \
     tableproperty.h \
     cellsplit.h \
     pcommon.h \
     index.h \
     raedit.h \
     srclistparser.h \
     export.h \
     tagdialog.h

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/lang/bqella_ru.ts \
    resources/lang/bqella_de.ts \
    resources/lang/bqella_fr.ts
