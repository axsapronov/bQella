
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
    src/gui/dialogs/helpdialog \
    src/gui/dialogs/about \
    src/gui/dialogs/fontdialog \
    src/gui/dialogs/bookdialog \
    src/gui/dialogs/finds/findandreplacedialog \
    src/gui/dialogs/contents/book \
    src/gui/dialogs/contents/module \
    src/gui/dialogs/assistant \
    src/gui/properties/projectproperty \
    src/gui/properties/docproperty \
    src/gui/properties/itemproperty \
    src/gui/properties/linkproperty \
    src/gui/properties/linkmoduleproperty \
    src/gui/properties/imageproperty \
    src/gui/properties/strongproperty \
    src/gui/properties/tagdialog\
    src/gui/table/cellsplit \
    src/gui/panel \
    src/gui/import/importdialog \
    src/gui/import/importbookdialog \
    src/gui/table/tableproperty \
    src/gui/advance/splitFile \
    src/gui/advance/preview \
    src/gui/tab \
    src/texteditorbqella \
    src/gui/topicchooser \
    src/settings \
    src/main \
    src/doc \
    src/export \
    src/hunspell \
    src/common \
    src/other

INCLUDEPATH += \
    src/cpp \
    src/h \
    src/ui \
    src/gui/dialogs/helpdialog \
    src/gui/dialogs/about \
    src/gui/dialogs/fontdialog \
    src/gui/dialogs/bookdialog \
    src/gui/dialogs/finds/findandreplacedialog \
    src/gui/dialogs/contents/book \
    src/gui/dialogs/contents/module \
    src/gui/dialogs/assistant \
    src/gui/properties/projectproperty \
    src/gui/properties/docproperty \
    src/gui/properties/itemproperty \
    src/gui/properties/linkproperty \
    src/gui/properties/linkmoduleproperty \
    src/gui/properties/imageproperty \
    src/gui/properties/strongproperty \
    src/gui/properties/tagdialog\
    src/gui/table/tableproperty \
    src/gui/advance/splitFile \
    src/gui/advance/preview \
    src/gui/table/cellsplit \
    src/gui/import/importdialog \
    src/gui/import/importbookdialog \
    src/gui/tab \
    src/gui/topicchooser \
    src/other \
    src/hunspell \
    src/other \
    src/texteditorbqella \
    src/settings \
    src/main \
    src/doc \
    src/export \
    src/common \
    src/panel


FORMS += \
     docproperty.ui \
     leftpanel.ui\
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
     settingaspell.ui \
     topicchooser.ui \
     strongproperty.ui \
     splitfile.ui \
     imageproperty.ui \
     previewbook.ui \
     previewmodule.ui \
     rightpanel.ui \
     statistics.ui \
     linkmoduleproperty.ui

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
     leftpanel.cpp \
     helpwindow.cpp \
     fontpanel.cpp \
     imageproperty.cpp \
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
     texteditorbqella.cpp \
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
     previewbook.cpp \
     settingaspell.cpp \
     previewmodule.cpp \
     rightpanel.cpp \
     statistics.cpp \
     src/hunspell/affentry.cxx \
     src/hunspell/affixmgr.cxx \
     src/hunspell/csutil.cxx \
     src/hunspell/dictmgr.cxx \
     src/hunspell/filemgr.cxx \
     src/hunspell/hashmgr.cxx \
     src/hunspell/hunspell.cxx \
     src/hunspell/hunzip.cxx \
     src/hunspell/phonet.cxx \
     src/hunspell/suggestmgr.cxx \
     src/hunspell/utf_info.cxx \
     src/hunspell/highlighter.cpp \
     linkmoduleproperty.cpp

HEADERS += \
     docproperty.h \
     docuparser.h \
     fontsettingsdialog.h \
     leftpanel.h \
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
     imageproperty.h \
     profile.h \
     settingaspell.h \
     config.h \
     tabbedbrowser.h \
     tableproperty.h \
     cellsplit.h \
     pcommon.h \
     filecommon.h \
     strongcommon.h \
     index.h \
     texteditorbqella.h \
     srclistparser.h \
     export.h \
     tagdialog.h \
     assistant.h \
     importbookdialog.h \
     contentsbook.h \
     topicchooser.h \
     strongproperty.h \
     splitfile.h \
     previewbook.h \
     previewmodule.h \
     rightpanel.h \
     statistics.h \
     src/hunspell/affentry.hxx \
     src/hunspell/affixmgr.hxx \
     src/hunspell/atypes.hxx \
     src/hunspell/baseaffix.hxx \
     src/hunspell/csutil.hxx \
     src/hunspell/dictmgr.hxx \
     src/hunspell/filemgr.hxx \
     src/hunspell/hashmgr.hxx \
     src/hunspell/htypes.hxx \
     src/hunspell/hunspell.h \
     src/hunspell/hunspell.hxx \
     src/hunspell/hunzip.hxx \
     src/hunspell/langnum.hxx \
     src/hunspell/phonet.hxx \
     src/hunspell/suggestmgr.hxx \
     src/hunspell/w_char.hxx \
     highlighter.h \
     linkmoduleproperty.h

RESOURCES += \
    resources/resources.qrc

TRANSLATIONS += \
    resources/lang/bqella_ru.ts \
    resources/lang/bqella_de.ts \
    resources/lang/bqella_fr.ts
