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
    src/dialogs/helpdialog \
    src/dialogs/about \
    src/dialogs/fontdialog \
    src/dialogs/bookdialog \
    src/dialogs/finds/findandreplacedialog \
    src/properties/projectproperty \
    src/properties/docproperty \
    src/properties/itemproperty \
    src/properties/linkproperty \
    src/import/importdialog \
    src/import/importbookdialog \
    src/properties/tagdialog \
    src/table/tableproperty \
    src/properties/strongproperty \
    src/table/cellsplit \
    src/dialogs/assistant \
    src/raedit \
    src/dialogs/contents/book \
    src/dialogs/contents/module \
    src/tab \
    src/settings \
    src/main \
    src/doc \
    src/export \
    src/topicchooser \
    src/advance/splitFile \
    src/advance/preview \
    src/cpp/common \
    src/h/common \
    src/panel


INCLUDEPATH += \
    src/cpp \
    src/h \
    src/ui \
    src/dialogs/helpdialog \
    src/dialogs/about \
    src/dialogs/fontdialog \
    src/dialogs/bookdialog \
    src/dialogs/finds/findandreplacedialog \
    src/properties/projectproperty \
    src/properties/itemproperty \
    src/properties/linkproperty \
    src/properties/docproperty \
    src/import/importdialog \
    src/import/importbookdialog \
    src/properties/tagdialog \
    src/table/tableproperty \
    src/properties/strongproperty \
    src/table/cellsplit \
    src/dialogs/assistant \
    src/raedit \
    src/dialogs/contents/book \
    src/dialogs/contents/module \
    src/tab \
    src/settings \
    src/main \
    src/doc \
    src/export \
    src/topicchooser \
    src/advance/splitFile \
    src/advance/preview \
    src/h/common \
    src/panel


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
     contentsbook.ui \
     topicchooser.ui \
     strongproperty.ui \
     splitfile.ui \
    src/advance/preview/previewbook.ui \
    src/advance/preview/previewmodule.ui \
    src/panel/rightpanel.ui

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
     strongcommon.cpp \
     filecommon.cpp \
     raedit.cpp \
     srclistparser.cpp \
     cellsplit.cpp \
     export.cpp \
     tagdialog.cpp \
     assistant.cpp \
     importbookdialog.cpp \
     contentsbook.cpp \
     topicchooser.cpp \
     strongproperty.cpp \
     splitfile.cpp \
    src/advance/preview/previewbook.cpp \
    src/advance/preview/previewmodule.cpp \
    src/panel/rightpanel.cpp

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
     filecommon.h \
     strongcommon.h \
     index.h \
     raedit.h \
     srclistparser.h \
     export.h \
     tagdialog.h \
     assistant.h \
     importbookdialog.h \
     contentsbook.h \
     topicchooser.h \
     strongproperty.h \
     splitfile.h \
    src/advance/preview/previewbook.h \
    src/advance/preview/previewmodule.h \
    src/panel/rightpanel.h

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/lang/bqella_ru.ts \
    resources/lang/bqella_de.ts \
    resources/lang/bqella_fr.ts
