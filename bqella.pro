QT += xml sql
TARGET = bQella
TEMPLATE = app
LANGUAGE = C++

CONFIG += qt \
    warn_on

win32 {
    LIBS +=  -lshell32
    RC_FILE =  resources/resources-win.rc
}


PROJECTNAME = bQella
DESTDIR     = build/bin
OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
RCC_DIR     = build/rc
UI_DIR      = build/ui

DEPENDPATH += \
    src/cpp \
    src/h \
    src/ui \
    src/helpdialog \
    src/about \
    src/fontdialog \
    src/bookdialog \
    src/finds/findandreplacedialog \
    src/propertys/projectproperty \
    src/propertys/itemproperty \
    src/propertys/linkproperty \
    src/import/importdialog \
    src/import/importbookdialog \
    src/tagdialog \
    src/propertys/tableproperty \
    src/cellsplit \
    src/help \
    src/contents/book \
    src/contents/module \
    src/tab \
    src/settings \
    src/main \
    src/doc \
    src/export


INCLUDEPATH += \
    src/h/ \
    src/helpdialog \
    src/about \
    src/fontdialog \
    src/bookdialog \
    src/finds/findandreplacedialog \
    src/propertys/projectproperty \
    src/propertys/itemproperty \
    src/propertys/linkproperty \
    src/import/importdialog \
    src/import/importbookdialog \
    src/tagdialog \
    src/propertys/tableproperty\
    src/cellsplit \
    src/help \
    src/contents/book \
    src/contents/module \
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
     about.ui \
     projectproperty.ui \
     mainwindow.ui \
     tabbedbrowser.ui \
     tableproperty.ui \
     cellsplit.ui \
     settings.ui \
     frdialog.ui \
     importdialog.ui \
     tagdialog.ui \
     importbookdialog.ui \
     contentsbook.ui

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
     about.cpp \
     itemproperty.cpp \
     linkproperty.cpp \
     index.cpp \
     pcommon.cpp \
     raedit.cpp \
     srclistparser.cpp \
     cellsplit.cpp \
     export.cpp \
     tagdialog.cpp \
     assistant.cpp \
     importbookdialog.cpp \
     contentsbook.cpp

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
     about.h \
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
     tagdialog.h \
     assistant.h \
     importbookdialog.h \
     contentsbook.h

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/lang/bqella_ru.ts \
    resources/lang/bqella_de.ts \
    resources/lang/bqella_fr.ts
