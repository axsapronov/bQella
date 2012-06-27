/*!
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *    Author: Alexander Sapronov/WarmongeR
 *    E-Mail: warmonger72@gmail.com
 *    Years: 2011
 */
#include "mainwindow.h"
#include "tabbedbrowser.h"
#include "helpdialog.h"
#include "config.h"
#include "pcommon.h"
#include "settings.h"
#include "guiabout.h"
#include "export.h"
#include "import.h"


#include <QAbstractTextDocumentLayout>
#include <QDir>
#include <QDockWidget>
#include <QEventLoop>
#include <QFileDialog>
#include <QFontDatabase>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QShortcut>
#include <QStatusBar>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextObject>
#include <QTimer>
#include <QtDebug> //to use qWarning and qDebug messages
#include <QtEvents>

QList<MainWindow*> MainWindow::windows;

#if defined(Q_WS_WIN)
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

//-------------------------------------------------
MainWindow::MainWindow():
    m_gui_About(new GUIAbout(this))
{
    setUnifiedTitleAndToolBarOnMac(true);
    ui.setupUi(this);

#if defined(Q_WS_WIN)
    // Workaround for QMimeSourceFactory failing in QFileInfo::isReadable() for
    // certain user configs. See task: 34372
    qt_ntfs_permission_lookup = 0;
#endif
    setupCompleted = false;

    goActions = QList<QAction*>();
    //    goActionDocFiles = new QMap<QAction*,QString>;
    
    windows.append(this);
    tabs = new TabbedBrowser(this);
    connect(tabs, SIGNAL(tabCountChanged(int)), this, SLOT(updateTabActions(int)));
    setCentralWidget(tabs);

    Config *config = Config::configuration();

    updateProfileSettings();

    dw = new QDockWidget(this);
    dw -> setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dw -> setWindowTitle(tr("Project manager"));
    dw -> setObjectName(QLatin1String("sidebar"));
    helpDock = new HelpDialog(dw, this);


    exportm = new Export();

    importm = new Import();

//    importm -> test();
//    importm -> setTestValue("lalala");
//    importm -> test();


    dw -> setWidget(helpDock);
    addDockWidget(Qt::LeftDockWidgetArea, dw);
    
    prjprop = new ProjectProperties(this);    
    appsets = new AppSettings(this);
    menuSign = new QMenu(tr("Insert Sign"));
    //включить после отладки
    //    connect(menuSign, SIGNAL(triggered(QAction*)), this, SLOT(insertSignature(QAction*)));

    restoreGeometry(config -> windowGeometry());
    restoreState(config -> mainWindowState());
    if (config -> sideBarHidden())
        dw -> hide();

    tabs -> setup();
    QTimer::singleShot(0, this, SLOT(setup()));
    
    if (config -> Lang() == "Russian"){
    	setLangRu();
    }

    //выключаем ненужный функционал
    ui.actionInsertImage -> setVisible(!ui.actionInsertImage -> isVisible());
    ui.actionFilePrint -> setVisible(false);
    ui.actionPrint_Preview -> setVisible(false);
    ui.toolBarTabs -> setVisible(false);
    ui.actionEditFind -> setVisible(false);
    //    ui.menuNew_with_pattern -> setVisible(false);
    ui.actionProjectBackup -> setVisible(false);

}

//-------------------------------------------------
MainWindow::~MainWindow()
{
    windows.removeAll(this);
    //    delete goActionDocFiles;
}

//-------------------------------------------------
void MainWindow::setup()
{
    if(setupCompleted)
        return;

    qApp -> setOverrideCursor(QCursor(Qt::WaitCursor));
    statusBar() -> showMessage(tr("Initializing %1...").arg(GL_Prog_Name));
    helpDock -> initialize();

    // Menu Project
    connect(ui.actionProjectNew, SIGNAL(triggered()), this, SLOT(ProjectNew()));
    //    connect(ui.actionNewPrjDiary, SIGNAL(triggered()), this, SLOT(ProjectNewDiary()));
    //    connect(ui.actionNewPrjNotebook, SIGNAL(triggered()), this, SLOT(ProjectNewNotebook()));
    connect(ui.actionProjectOpen, SIGNAL(triggered()), this, SLOT(ProjectOpen()));
    connect(ui.actionProjectSave, SIGNAL(triggered()), helpDock, SLOT(saveProject()));
    connect(ui.actionProjectSaveAs, SIGNAL(triggered()), this, SLOT(ProjectSaveAs()));
    connect(ui.actionProjectBackup, SIGNAL(triggered()), this, SLOT(ProjectBackup()));
    connect(ui.actionProjectProperties, SIGNAL(triggered()), this, SLOT(ProjectProps()));
    connect(ui.actionAppExit, SIGNAL(triggered()), this, SLOT(exitApp()));
    //connect(prjprop, SIGNAL(createDB(QString)), prjsrc, SLOT(newDb(QString)));
    connect(prjprop, SIGNAL(createProject(ModuleProperties)), this, SLOT(createProject(ModuleProperties)));
    connect(prjprop, SIGNAL(updateProjectProperties(ModuleProperties)), this, SLOT(updateProjectProperties(ModuleProperties)));

    // Menu File
    connect(ui.actionRemoveItem, SIGNAL(triggered()), helpDock, SLOT(removeItem()));
    connect(ui.actionDeleteFile, SIGNAL(triggered()), helpDock, SLOT(deleteItem()));
    connect(ui.actionExport_Module, SIGNAL(triggered()), this/*exportm*//*helpDock*/, SLOT(exportModule()));


    // Menu Import
    connect(ui.actionImportChapter, SIGNAL(triggered()), this, SLOT(importChapter()));
    connect(ui.actionImportBook, SIGNAL(triggered()), this, SLOT(importBook()));
    connect(ui.actionImportModule, SIGNAL(triggered()), this, SLOT(importModule()));



    // Menu Edit
    connect(helpDock, SIGNAL(showLink(QString)), this, SLOT(showLink(QString)));
    connect(helpDock, SIGNAL(showSearchLink(QString,QStringList)), this, SLOT(showSearchLink(QString,QStringList)));
    connect(ui.actionHyperlink, SIGNAL(triggered()), browsers() -> currentBrowser(), SLOT(showLinkProperties()));
    connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(showAppSettings()));

    // Menu Format
    connect(appsets, SIGNAL(showContentsAV(bool)), helpDock, SLOT(showContentsAV(bool)));
    connect(appsets, SIGNAL(showContentsAVHeader(bool)), helpDock, SLOT(showContentsAVHeader(bool)));
    //connect(ui.actionEditFont_Settings, SIGNAL(triggered()), this, SLOT(showFontSettingsDialog()));
    connect(appsets, SIGNAL(updateApplicationFontSettings(FontSettings)), this, SLOT (updateAppFont(FontSettings)));
    
    // Menu Tabs
    connect(ui.actionOpenPage,  SIGNAL(triggered()), tabs, SLOT(newTab()));
    connect(ui.actionClosePage, SIGNAL(triggered()), tabs, SLOT(closeTab()));
    connect(ui.actionNextPage,  SIGNAL(triggered()), tabs, SLOT(nextTab()));
    connect(ui.actionPrevPage,  SIGNAL(triggered()), tabs, SLOT(previousTab()));


    // Menu Language
    connect(ui.actionLang_en, SIGNAL(triggered()), this, SLOT(setLangEn()) );
    connect(ui.actionLang_ru, SIGNAL(triggered()), this, SLOT(setLangRu()) );

    connect(new QShortcut(tr("Ctrl+W"), this), SIGNAL(activated()), tabs, SLOT(closeTab()));
    connect(new QShortcut(tr("Ctrl+]"), this), SIGNAL(activated()), tabs, SLOT(nextTab()));
    connect(new QShortcut(tr("Ctrl+["), this), SIGNAL(activated()), tabs, SLOT(previousTab()));
    connect(new QShortcut(tr("Ctrl+Shift+Insert"), this), SIGNAL(activated()), this, SLOT(globalShortcut_CtrlShiftInsert()));//!+! move to browsers() -> currentBrowser()

    Config *config = Config::configuration();

    QAction *viewsAction = createPopupMenu() -> menuAction();
    viewsAction -> setText(tr("Toolbars & panels"));
    ui.viewMenu -> addAction(viewsAction);

    const int tabIndex = config -> sideBarPage();
    helpDock -> tabWidget() -> setCurrentIndex(tabIndex);
    // The tab index is 0 by default, so we need to force an update to poulate the contents in this case.
    if (tabIndex == 0)
        helpDock -> currentTabChanged(tabIndex);

    ui.actionEditFind -> setShortcut(QKeySequence::Find);
    ui.actionEditFindNext -> setShortcut(QKeySequence::FindNext);
    ui.actionEditFindPrev -> setShortcut(QKeySequence::FindPrevious);
    
    QObject::connect(ui.actionEditFind, SIGNAL(triggered()), tabs, SLOT(find()));
    QObject::connect(ui.actionEditFindNext, SIGNAL(triggered()), tabs, SLOT(findNext()));
    QObject::connect(ui.actionEditFindPrev, SIGNAL(triggered()), tabs, SLOT(findPrevious()));

    qApp -> restoreOverrideCursor();
    //    ui.actionGoPrevious -> setEnabled(false);
    //    ui.actionGoNext -> setEnabled(false);
    //    ui.actionEditCopy -> setEnabled(false);
    //    connect(tabs -> currentBrowser(), SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailable(bool)));

    //apply settings
    appsets -> set();
    appsets -> apply();

    // set the current selected item in the treeview
    helpDialog() -> locateContents(tabs -> currentBrowser() -> source().toString());
    connect(tabs, SIGNAL(browserUrlChanged(QString)), helpDock, SLOT(locateContents(QString)));
    projectModified(false);
    setupCompleted = true;
}

//-------------------------------------------------
void MainWindow::exportModule()
{
//    QString test = "mytest";
//    qDebug() << "Debug: _MainWindow::exportModule()" << "test = " << test;

//    exportm  ->  file = "test";
//    exportm -> setFileBibleqtName(helpDock -> ui.listContents -> topLevelItem(0) -> data(0,LinkRole).toString().remove("file:"));
//    exportm -> setFileBibleqtName(helpDock->getFileBibleqtName());

//    exportm -> test();
    helpDock -> exportModule();
}

void MainWindow::importModule()
{
//    QString beginpath = "/home/warmonger";
//    QString fileName = QFileDialog::getOpenFileName(this,
//                      tr("Select bibleqt.ini"),
//                      beginpath,
//                      tr("Bibleqt.ini (*.ini)"));
    QString fileName = "/home/warmonger/Develop/git/bqella-build-desktop/build/bin/projects/rrter/export_rrter/bibleqt.ini";
    importm->importModule(fileName);

    ModuleProperties pr;
    pr.moduleBiblename = Config::configuration()->ModuleBiblename();
    pr.moduleBibleShortName = Config::configuration()->ModuleBibleShortName();
    pr.moduleCopyright = Config::configuration()->ModuleCopyright();
    pr.moduleBVersion = 1.00;
    pr.prjFN = importm->getPrjFN();
    pr.prjStartPage = importm->getStartPage();
    pr.prjTitle = Config::configuration()->ModuleBiblename();

//    qDebug() << pr.moduleBiblename;
//    qDebug() << pr.moduleBibleShortName;
//    qDebug() << pr.moduleBVersion;
//    qDebug() << pr.moduleCopyright;
//    qDebug() << pr.prjFN;
//    qDebug() << pr.prjStartPage;
//    qDebug() << pr.prjTitle;

    // не добавились автоматом ссылки на файлы книг
//    createProject(pr);
//    importm-> addContentToProjectFile(pr);
}

void MainWindow::importChapter()
{
//    QString beginpath = "/home/";
//    QString fileName = QFileDialog::getOpenFileName(this,
//                      tr("Select chapter file"),
//                      beginpath,
//                      tr("Chapter file (*.html *.txt *.htm)"));
//    importm->importChapter(fileName);


}

void MainWindow::importBook()
{
//    QString beginpath = "/home/";
//    QString path = QFileDialog::getOpenFileName(this,
//                      tr("Select book file"),
//                      beginpath,
//                      tr("Book file (*.html *.txt *.htm)"));
//    importm->importBook(path, "", "", 0);
//// еще надо вызывать диалог с вводом данных о книге
//    //PathName
//    //FullName
//    //ShortName
//    //ChapterQty
}

//-------------------------------------------------
void MainWindow::browserTabChanged()
{
    /*    if (tabs -> currentBrowser()) {
        ui.actionGoPrevious -> setEnabled(tabs -> currentBrowser() -> isBackwardAvailable());
        ui.actionGoNext -> setEnabled(tabs -> currentBrowser() -> isForwardAvailable());
    }*/
}


//-------------------------------------------------
void MainWindow::updateTabActions(int index)
{
    bool enabled = (index > 1) ? true : false;
    ui.actionPrevPage -> setEnabled(enabled);
    ui.actionNextPage -> setEnabled(enabled);
    ui.actionClosePage -> setEnabled(enabled);
}


//-------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *e)
{
    qDebug()<< "closeEvent()";
    saveSettings();
    e -> accept();
}

//-------------------------------------------------
void MainWindow::about()
{
    m_gui_About -> show();
}

//-------------------------------------------------
void MainWindow::on_actionAboutAssistant_triggered()
{
    about();
}

//-------------------------------------------------
void MainWindow::on_actionFilePrint_triggered()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);

    if (dlg -> exec() == QDialog::Accepted) {
        tabs -> currentBrowser() -> document() -> print(&printer);
    }

    delete dlg;
}

//-------------------------------------------------
void MainWindow::showLinkFromClient(const QString &link)
{
    setWindowState(windowState() & ~Qt::WindowMinimized);
    raise();
    activateWindow();
    QString l = urlifyFileName(link);
    showLink(l);
    if (isMinimized())
        showNormal();
}

//-------------------------------------------------
// open link in active tab. Link should be in format file://<absolute path>/filename.html
void MainWindow::showLink(const QString &link)
{
    QString lnk = unurlifyFileName(link);
    QFileInfo fi(lnk);
    if( (!lnk.isEmpty()) && fi.exists() && fi.isFile() ){    	
    	// don't open a new tab for the same url more then once
        if (link == tabs -> currentBrowser() -> source().toString())
    	    return;
        if (ui.actionSaveFile -> isEnabled()) //i.e. document was modified
            emit saveOpenedLink();
    	QUrl url(link);
        //qDebug() << "down!";
        tabs -> setSource(url.toString()); // w
        tabs -> currentBrowser() -> setFocus();
    }else{
        qWarning() << "Failed to open link: " << link;
        QMessageBox::warning(this, GL_Prog_Name, tr("failed to open file:\n%1").arg(lnk));
    }
}

//-------------------------------------------------
// open several links in new tabs
void MainWindow::showLinks(const QStringList &links)
{
    if (links.size() == 0) {
        qWarning("MainWindow::showLinks() - Empty link");
        return;
    }

    if (links.size() == 1) {
        showLink(urlifyFileName(links.first()));
        return;
    }

    QStringList::ConstIterator it = links.begin();
    // Initial showing, The tab is empty so update that without creating it first
    if (!tabs -> currentBrowser() -> source().isValid()) {
        QPair<HelpWindow*, QString> browser;
        browser.first = tabs -> currentBrowser();
        browser.second = links.first();
        pendingBrowsers.append(browser);
        tabs -> setAppTitle(tabs -> currentBrowser(), tr("..."));
    }
    ++it;

    while(it != links.end()) {
        QPair<HelpWindow*, QString> browser;
        browser.first = tabs -> newBackgroundTab();
        browser.second = *it;
        pendingBrowsers.append(browser);
        ++it;
    }

    startTimer(50);
    return;
}

//-------------------------------------------------
void MainWindow::removePendingBrowser(HelpWindow *win)
{
    if (!pendingBrowsers.count())
        return;

    QMutableListIterator<QPair<HelpWindow*, QString> > it(pendingBrowsers);
    while (it.hasNext()) {
        QPair<HelpWindow*, QString> browser = it.next();
        if (browser.first == win) {
            it.remove();
            break;
        }
    }
}

//-------------------------------------------------
void MainWindow::timerEvent(QTimerEvent *e)
{
    QPair<HelpWindow*, QString> browser = pendingBrowsers.first();
    pendingBrowsers.pop_front();
    
    if (pendingBrowsers.size() == 0)
        killTimer(e -> timerId());

    browser.first -> setSource(urlifyFileName(browser.second));
}

//-------------------------------------------------
MainWindow* MainWindow::newWindow()
{
    qDebug()<< "Debug: _MainWindow::newWindow()" << "newWindow()";
    saveSettings();
    MainWindow *mw = new MainWindow;
    mw -> move(geometry().topLeft());
    if (isMaximized())
        mw -> showMaximized();
    else
        mw -> show();
    //    mw -> on_actionGoHome_triggered();
    return mw;
}

//-------------------------------------------------
void MainWindow::saveSettings()
{
    Config *config = Config::configuration();
    qDebug() << "src = start to save settings";
    config -> setSideBarPage(helpDock -> tabWidget() -> currentIndex());
    config -> setWindowGeometry(saveGeometry());
    config -> setMainWindowState(saveState());
    
    // Create list of the tab urls
    QStringList lst;
    QList<HelpWindow*> browsers = tabs -> browsers();
    foreach (HelpWindow *browser, browsers){
        qDebug() << "src = " << browser -> source().toString();
        lst << relatifyFileName(browser -> source().toString(), config -> PrjDir());
    }
    config -> setSource(lst);
    config -> saveSettings();
}

//-------------------------------------------------
TabbedBrowser* MainWindow::browsers() const
{
    return tabs;
}

//-------------------------------------------------
void MainWindow::showSearchLink(const QString &link, const QStringList &terms)
{
    HelpWindow * hw = tabs -> currentBrowser();
    hw -> blockScrolling(true);
    hw -> setCursor(Qt::WaitCursor);
    if (hw -> source() == link)
        hw -> reload();
    else
        showLink(link);
    hw -> setCursor(Qt::ArrowCursor);

    hw -> viewport() -> setUpdatesEnabled(false);

    QTextCharFormat marker;
    //marker.setForeground(Qt::red);    //!+! mark words, but do not save this marks

    QTextCursor firstHit;

    QTextCursor c = hw -> textCursor();
    c.beginEditBlock();
    foreach (QString term, terms) {
        c.movePosition(QTextCursor::Start);
        hw -> setTextCursor(c);

        bool found = hw -> find(term, QTextDocument::FindWholeWords);
        while (found) {
            QTextCursor hit = hw -> textCursor();
            if (firstHit.isNull() || hit.position() < firstHit.position())
                firstHit = hit;

            hit.mergeCharFormat(marker);
            found = hw -> find(term, QTextDocument::FindWholeWords);
        }
    }

    if (firstHit.isNull()) {
        firstHit = hw -> textCursor();
        firstHit.movePosition(QTextCursor::Start);
    }
    firstHit.clearSelection();
    c.endEditBlock();
    hw -> setTextCursor(firstHit);

    hw -> blockScrolling(false);
    hw -> viewport() -> setUpdatesEnabled(true);
}


//-------------------------------------------------
HelpDialog* MainWindow::helpDialog() const
{
    return helpDock;
}

//-------------------------------------------------
void MainWindow::updateProfileSettings()
{
    Config *config = Config::configuration();

    /*	#ifndef Q_WS_MAC
    setWindowIcon(config -> applicationIcon());
#endif 
    ui.helpMenu -> clear();
    ui.helpMenu -> addAction(ui.actionAboutAssistant);
    ui.helpMenu -> addSeparator();
    ui.helpMenu -> addAction(ui.actionHelpWhatsThis);
*/
    if(!config -> profileTitle().isEmpty())
        setWindowTitle(config -> profileTitle());
}

//-------------------------------------------------
void MainWindow::setupPopupMenu(QMenu *m)
{
    m -> addMenu(menuSign);
    m -> addSeparator();
    m -> addAction(ui.actionNewWindow);
    m -> addAction(ui.actionOpenPage);
    m -> addAction(ui.actionClosePage);
    m -> addSeparator();

    m -> addSeparator();
    m -> addAction(ui.actionCopy);
    m -> addAction(ui.actionPaste);
    m -> addAction(ui.actionCut);
    m -> addSeparator();
    m -> addAction(ui.actionEditFind);
}

//-------------------------------------------------
void MainWindow::ProjectOpen()
{//warmongeR
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), Config::configuration() -> PrjDir(),
                                                    tr("Project bQella (*.pem)"));
    if (!fileName.isEmpty()){
        ProjectOpen(fileName);
    }
}

//-------------------------------------------------
void MainWindow::ProjectOpen(QString fileName)
{
    if (!fileName.isEmpty()){
        //Config::configuration() -> toAppLog(1, tr("Open project: %1", "For log").arg(fileName));
        browsers() -> currentBrowser() -> fileSave();
        Config::configuration() -> loadProject(fileName);
        helpDock -> enableProjectButtons();
        helpDock -> initTabs();
        browsers() -> closeAllTabs();
        helpDock -> insertContents();
        //Config::configuration() -> toAppLog(1, tr("- show start page: %1", "For log").arg(Config::configuration() -> CurFile()));
        showLink(urlifyFileName(Config::configuration() -> CurFile()));
    	projectModified(false);
        Config::configuration() -> toPrjLog(1, "-------");
        Config::configuration() -> toPrjLog(1, tr("Project is opened.", "For log"));
    }
}

//-------------------------------------------------
void MainWindow::ProjectSaveAs()
{//warmonger
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project As"), Config::configuration() -> CurPrjDir(), tr("Project bQella (*.pep)"));
    if ( !fileName.isEmpty() ){
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(GL_Project_File);
        helpDock -> saveProject(fileName);
    }
}


//-------------------------------------------------
void MainWindow::on_actionNewWindow_triggered()
{
    newWindow() -> show();
}

//-------------------------------------------------
void MainWindow::on_actionSaveFileAs_triggered()
{
    QString fileName;
    QUrl url = tabs -> currentBrowser() -> source();
    if (url.isValid()) {
        QFileInfo fi(url.toLocalFile());
        fileName = fi.fileName();
    }
    fileName = QFileDialog::getSaveFileName(this, tr("Save Page"), fileName);
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    //qDebug() << "opening file on_actionSaveFileAs_triggered: " << fileName;
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Save Page"), tr("Cannot open file for writing!"));
        return;
    }

    QFileInfo fi(fileName);
    QString fn = fi.fileName();
    int i = fn.lastIndexOf(QLatin1Char('.'));
    if (i > -1)
        fn = fn.left(i);
    QString relativeDestPath = fn + QLatin1String("_images");
    QDir destDir(fi.absolutePath() + QDir::separator() + relativeDestPath);
    bool imgDirAvailable = destDir.exists();
    if (!imgDirAvailable)
        imgDirAvailable = destDir.mkdir(destDir.absolutePath());

    // save images
    QTextDocument *doc = tabs -> currentBrowser() -> document() -> clone();
    if (url.isValid() && imgDirAvailable) {
        QTextBlock::iterator it;
        for (QTextBlock block = doc -> begin(); block != doc -> end(); block = block.next()) {
            for (it = block.begin(); !(it.atEnd()); ++it) {
                QTextFragment fragment = it.fragment();
                if (fragment.isValid()) {
                    QTextImageFormat fm = fragment.charFormat().toImageFormat();
                    if (fm.isValid() && !fm.name().isEmpty()) {
                        QUrl imagePath = tabs -> currentBrowser() -> source().resolved(fm.name());
                        if (!imagePath.isValid())
                            continue;
                        QString from = imagePath.toLocalFile();
                        QString destName = fm.name();
                        int j = destName.lastIndexOf(QLatin1Char('/'));
                        if (j > -1)
                            destName = destName.mid(j+1);
                        QFileInfo info(from);
                        if (info.exists()) {
                            if (!QFile::copy(from, destDir.absolutePath()
                                             + QDir::separator() + destName))
                                continue;
                            fm.setName(QLatin1String("./") + relativeDestPath + QLatin1String("/") + destName);
                            QTextCursor cursor(doc);
                            cursor.setPosition(fragment.position());
                            cursor.setPosition(fragment.position() + fragment.length(),
                                               QTextCursor::KeepAnchor);
                            cursor.setCharFormat(fm);
                        }
                    }
                }
            }
        }
    }
    QString src = doc -> toHtml(QByteArray("utf-8"));
    QTextStream s(&file);
    s.setCodec("utf-8");
    s << src;
    s.flush();
    file.close();
} //on_actionSaveFileAs_triggered()
//-------------------------------------------------
void MainWindow::updateAppFont(FontSettings settings)
{
    QFont font = settings.windowFont;
    if (this -> font() != font)
        qApp -> setFont(font, "QWidget");

    font = settings.browserFont;
    QList<HelpWindow*> browsers = tabs -> browsers();
    foreach (HelpWindow *browser, browsers) {
        if (browser -> font() != font)
            browser -> setFont(font);
    }
}

//-------------------------------------------------
void MainWindow::exitApp()
{
    browsers() -> currentBrowser() -> fileSave();
    helpDock -> saveProject();
    qApp -> closeAllWindows();
}

//-------------------------------------------------
void MainWindow::ProjectNew()
{
    ModuleProperties pr;
    //    qDebug() << "Debug: _MainWindow::ProjectNew()" << " prjdir = " <<  Config::configuration() -> PrjDir();

    bool newProject = true;

    pr.prjTitle = tr("New Project", "Default project name");
    pr.moduleBiblename = "";
    pr.moduleBibleShortName = "";
    pr.moduleCopyright = "";
    pr.moduleBVersion = 1.0;
    pr.prjFN = Config::configuration() -> PrjDir()+pr.moduleBiblename;
//    pr.moduleType = "Bible";

    prjprop -> setProperties(newProject, pr);
    prjprop -> show();
}

////-------------------------------------------------
//void MainWindow::ProjectNewDiary()
//{
//    ProjectNew();
//    helpDock -> dialogAutoItems -> activateTab(0);
//    helpDock -> dialogAutoItems -> CreateItems();
//    helpDock -> dialogAutoItems -> show();
//}

////-------------------------------------------------
//void MainWindow::ProjectNewNotebook()
//{
//    ProjectNew();
//    helpDock -> dialogAutoItems -> activateTab(1);
//    helpDock -> dialogAutoItems -> setCounterType(3);
//    helpDock -> dialogAutoItems -> setMaxCounter();
//    helpDock -> dialogAutoItems -> CreateItems();
//    helpDock -> dialogAutoItems -> show();
//}

//-------------------------------------------------
void MainWindow::ProjectProps()
{
    ModuleProperties pr;
    pr.prjTitle = Config::configuration() -> profile() -> props["title"];
    pr.prjStartPage = Config::configuration() -> profile() -> props["startpage"];
    pr.moduleBiblename = Config::configuration() -> profile() -> props["biblename"];
    pr.moduleBibleShortName = Config::configuration() -> profile() -> props["bibleshortname"];
    pr.moduleCopyright = Config::configuration() -> profile() -> props["copyright"];
    pr.moduleBVersion = QString(Config::configuration() -> profile() -> props["version"]).toDouble();
//    pr.moduleType = Config::configuration() -> profile() -> props["type"];
    bool newProject = false;
    prjprop -> setProperties(newProject, pr);
    prjprop -> show();
}
//-------------------------------------------------
void MainWindow::createProject(ModuleProperties pr)
{ 
    QString ind1="   ";
    QString fn = unurlifyFileName(pr.prjFN);
    qDebug() << " fn = " << fn;
    Config::configuration() -> toAppLog(1, tr("Create a new project: %1", "For log").arg(pr.prjTitle));
    Config::configuration() -> toAppLog(3, tr("- project file: %1", "For log").arg(fn));
    QFile f(fn);
    if (!f.open(QFile::WriteOnly)){
    	qDebug() << "Failed to create project: " << fn;
        statusBar() -> showMessage(tr("Failed to create project: %1").arg(fn), 7000);
        Config::configuration() -> toAppLog(1, tr("- failed", "For log"));
        return;
    }

    Config::configuration() -> toAppLog(3, tr("- project start page: %1", "For log").arg(pr.prjStartPage));
    QFileInfo fi(fn);
    QString name = fi.baseName();
    QString path = fi.absolutePath();
    Config::configuration() -> setDbName(path +"/"+ name +"-sources.db");
    name.remove(QChar(' '));
    QString spFN = relatifyFileName(pr.prjStartPage, path);
    //fi.setFile(spFN);
    QString spT = tr("   ___Instruction");//fi.baseName(); // name of first doc in project in listcontent
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << "<pemproject version=\"1.0\">" << endl << endl;

    QString version;
    version.setNum(pr.moduleBVersion);
//    qDebug() << "Debug: _MainWindow::createProject" << "version(str) = " << version << "version(double) = " << pr.moduleBVersion;

    ts << "<profile>" << endl;
    ts << ind1 << "<property name=\"title\">" << Qt::escape(pr.prjTitle) << "</property>" << endl;
    ts << ind1 << "<property name=\"name\">" << Qt::escape(name) << "</property>" << endl;
    ts << ind1 << "<property name=\"startpage\">" << Qt::escape(spFN) << "</property>" << endl;
    ts << ind1 << "<property name=\"biblename\">" << Qt::escape(pr.moduleBiblename) << "</property>" << endl;
    ts << ind1 << "<property name=\"bibleshortname\">" << Qt::escape(pr.moduleBibleShortName) << "</property>" << endl;
    ts << ind1 << "<property name=\"copyright\">" << Qt::escape(pr.moduleCopyright) << "</property>" << endl;
    ts << ind1 << "<property name=\"version\">" << pr.moduleBVersion << "</property>" << endl;
//    ts << ind1 << "<property name=\"type\">" << pr.moduleType << "</property>" << endl;
    ts << "</profile>" << endl << endl;

    ts << "<contents>" << endl;
    ts << ind1 << "<section title=\"" << Qt::escape(spT) << "\" ref=\""<< Qt::escape(spFN) << "\" icon=\"\">" << endl;
    ts << ind1 << "</section>" << endl;
    ts << "</contents>" << endl << endl;

    ts << "</pemproject>" << endl;
    f.close();

    Config::configuration() -> toAppLog(3, tr("- project sources DB: %1", "For log").arg(Config::configuration() -> DbName()));
    Config::configuration() -> toAppLog(1, tr("- done", "For log"));
    ProjectOpen(fn);
}

//-------------------------------------------------
void MainWindow::updateProjectProperties(ModuleProperties pr)
{
    QString p = unurlifyFileName(pr.prjFN);
    QFileInfo fi(p);
    Config::configuration() -> toPrjLog(1, tr("Update project properties:", "For log"));
    Config::configuration() -> toPrjLog(3, tr("- title      = %1", "For log").arg(pr.prjTitle));
    Config::configuration() -> toPrjLog(3, tr("- file name  = %1", "For log").arg(pr.prjFN));
    Config::configuration() -> toPrjLog(3, tr("- start page = %1", "For log").arg(pr.prjStartPage));
    Config::configuration() -> toPrjLog(3, tr("- module name      = %1", "For log").arg(pr.moduleBiblename));
    Config::configuration() -> toPrjLog(3, tr("- module short name  = %1", "For log").arg(pr.moduleBibleShortName));
    Config::configuration() -> toPrjLog(3, tr("- module copyright = %1", "For log").arg(pr.moduleCopyright));

    Config::configuration() -> profile() -> addProperty("title", pr.prjTitle);
    Config::configuration() -> profile() -> addProperty("startpage", pr.prjStartPage);
    Config::configuration() -> profile() -> addProperty("biblename", pr.moduleBiblename);
    Config::configuration() -> profile() -> addProperty("bibleshortname", pr.moduleBibleShortName);
    Config::configuration() -> profile() -> addProperty("copyright", pr.moduleCopyright);
//    Config::configuration() -> profile() -> addProperty("type", pr.moduleType);

    QString version;
    version.setNum(pr.moduleBVersion);
    qDebug() << "Debug: _MainWindow::updateProjectProperties:" << "version(str) = " << version << "version(double) = " << pr.moduleBVersion;
    Config::configuration() -> profile() -> addProperty("version", version);


    Config::configuration() -> setCurProject(p);
    Config::configuration() -> setCurPrjDir(fi.absolutePath());
    Config::configuration() -> setCurPrjSrc();
    Config::configuration() -> toPrjLog(1, tr("- done", "For log"));
}

//-------------------------------------------------
void MainWindow::setLangEn()
{
    ui.actionLang_en -> setChecked(true);
    ui.actionLang_ru -> setChecked(false);
    Config::configuration() -> setLang("English");
    msgReloadRequest();
}

//-------------------------------------------------
void MainWindow::setLangRu()
{
    ui.actionLang_en -> setChecked(false);
    ui.actionLang_ru -> setChecked(true);
    Config::configuration() -> setLang("Russian");
    msgReloadRequest();
}

//-------------------------------------------------
void MainWindow::msgReloadRequest()
{	
    if (setupCompleted)
        QMessageBox::warning(this, tr("Reload application"), tr("Changes will be applied after application reload.", "Append this warning in English after translation"));
}

//-------------------------------------------------
void MainWindow::showAppSettings()
{
    appsets -> show();
}

//-------------------------------------------------
void MainWindow::OpenInExternalApplication(QString app, QString FileName)
{
    QString fn = unurlifyFileName(FileName);
    QProcess *extApp = new QProcess(this);
    QStringList arguments;
    arguments << fn;
    extApp -> start(app, arguments);
    Config::configuration() -> toPrjLog(2, tr("Open file in external application: %1 %2", "For log").arg(app).arg(FileName));
}



//-------------------------------------------------
void MainWindow::ProjectBackup()
{
    QDateTime dt = QDateTime::currentDateTime();
    QString archiveFN = Config::configuration() -> BackupDir() + "/" + Config::configuration() -> profileName()+ dt.toString(" yyyy-MM-dd-hh-mm")+ ".7z";
    QString cmd =	"\"" + Config::configuration() -> ExternalArchiver() +"\" "+
            Config::configuration() -> ExternalArchiverOptions() +
            " \""+ archiveFN +"\" \""+
            Config::configuration() -> CurPrjDir() + "/\"";
    QProcess *extApp = new QProcess(this);
    extApp -> start(cmd);
    Config::configuration() -> toPrjLog(1,tr("Creating backup archive: %1", "For log").arg(cmd));
    QMessageBox::information(this, tr("Backup"), tr("Backup is in\n%1").arg(archiveFN), tr("OK"));
}

//-------------------------------------------------
void MainWindow::globalShortcut_CtrlShiftInsert()
{
    if (QApplication::focusWidget() -> objectName() == "raWorkArea"){
        //  insertDefaultSignature();
    }

}

//-------------------------------------------------
void MainWindow::projectModified(bool modified){
    ui.actionProjectSave -> setEnabled(modified);
}
