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
 *    Author: Sapronov Alexander/WarmongeR
 *    E-Mail: sapronov.alexander92@.gmail.com
 *    Years: 2011, 2012
 *    WebSite: https://github.com/WarmongeR1/bQella
 */


#include "mainwindow.h"
#include "tabbedbrowser.h"
#include "helpdialog.h"
#include "config.h"
#include "pcommon.h"
#include "settings.h"
#include "about.h"
#include "export.h"
#include "importdialog.h"
#include "importbookdialog.h"
#include "frdialog.h"
#include "assistant.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>
#include <QStatusBar>
#include <QShortcut>
#include <QCloseEvent>


QList<MainWindow*> MainWindow::windows;

#if defined(Q_WS_WIN)
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif

//-------------------------------------------------
MainWindow::MainWindow():
    aboutd(new AboutDialog(this))
{
//    qDebug() << "[9]";
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
    dw -> setObjectName(QString("sidebar"));
    helpDock = new HelpDialog(dw, this);
    frdialog = new FRDialog();
    exportm = new Export();
    importm = new Import(this);
    importdi = new ImportBookDialog();
    assistant = new Assistant;

    //    HelpBrowser  helpBr("/home/files/Develop/git/next/bQella/resources/doc", "index.htm");
    //    helpBr = new HelpBrowser(":doc/doc", "index.htm", this);

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
    //    ui.toolBarTabs -> setVisible(false);
    //    ui.actionEditFind -> setVisible(false);
    //    ui.actionImportBook->setVisible(false);
    ui.actionPrint_Preview ->setVisible(false);

    //     importm->importModule("/home/files/Documents/Bible/unrar/NT_Greek_WH-E_UTF8/BIBLEQT.INI");
    //ui.lEImportFile->setText("/home/files/Documents/Bible/unrar/NT_Greek_WH-E_UTF8/BIBLEQT.INI");
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
//    qDebug() << "[7]";
    if(setupCompleted)
        return;

    qApp -> setOverrideCursor(QCursor(Qt::WaitCursor));
    statusBar() -> showMessage(tr("Initializing %1...").arg(GL_PROG_NAME));
    helpDock -> initialize();
//    qDebug() << "[14]";

    // Menu Project
    connect(ui.actionProjectNew, SIGNAL(triggered()), this, SLOT(ProjectNew()));
    //    connect(ui.actionNewPrjDiary, SIGNAL(triggered()), this, SLOT(ProjectNewDiary()));
    //    connect(ui.actionNewPrjNotebook, SIGNAL(triggered()), this, SLOT(ProjectNewNotebook()));
    connect(ui.actionProjectOpen, SIGNAL(triggered()), this, SLOT(ProjectOpen()));
    connect(ui.actionProjectSave, SIGNAL(triggered()), helpDock, SLOT(saveProject()));
    connect(ui.actionProjectSaveAs, SIGNAL(triggered()), this, SLOT(ProjectSaveAs()));
    connect(ui.actionProjectProperties, SIGNAL(triggered()), this, SLOT(ProjectProps()));
    connect(ui.actionAppExit, SIGNAL(triggered()), this, SLOT(exitApp()));
    connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    //connect(prjprop, SIGNAL(createDB(QString)), prjsrc, SLOT(newDb(QString)));
    connect(prjprop, SIGNAL(createProject(ModuleProperties)), this, SLOT(createProject(ModuleProperties)));
    connect(prjprop, SIGNAL(updateProjectProperties(ModuleProperties)), this, SLOT(updateProjectProperties(ModuleProperties)));

    connect(importm, SIGNAL(SuccessfulImport()), this, SLOT(importModuleSuccessful()));
    connect(importdi, SIGNAL(SuccessfulImportBook()), this, SLOT(importBookSuccessful()));

    // Menu File
    connect(ui.actionRemoveItem, SIGNAL(triggered()), helpDock, SLOT(removeItem()));
    connect(ui.actionDeleteFile, SIGNAL(triggered()), helpDock, SLOT(deleteItem()));
    connect(ui.actionExport_Module, SIGNAL(triggered()), this/*exportm*//*helpDock*/, SLOT(exportModule()));


    // Menu Import
    connect(ui.actionImportBook, SIGNAL(triggered()), this, SLOT(importBook()));
    connect(ui.actionImportModule, SIGNAL(triggered()), this, SLOT(importModule()));



    // Menu Edit
    connect(helpDock, SIGNAL(showLink(QString)), this, SLOT(showLink(QString)));
    connect(helpDock, SIGNAL(showSearchLink(QString,QStringList)), this, SLOT(showSearchLink(QString,QStringList)));
    connect(ui.actionHyperlink, SIGNAL(triggered()), browsers() -> currentBrowser(), SLOT(showLinkProperties()));
    connect(ui.actionAddTag, SIGNAL(triggered()), browsers() -> currentBrowser(), SLOT(showTagProperties()));

    connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(showAppSettings()));

    // find
    connect(ui.actionEditFind, SIGNAL(triggered()), tabs, SLOT(find()));
    connect(ui.actionEditFindNext, SIGNAL(triggered()), tabs, SLOT(findNext()));
    connect(ui.actionEditFindPrev, SIGNAL(triggered()), tabs, SLOT(findPrevious()));
    connect(ui.actionEditFindAndReplace, SIGNAL(triggered()), frdialog, SLOT(show()));



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


    // Menu about
    connect(ui.actionHelp, SIGNAL(triggered()), this, SLOT(showDocumentation()));



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



    qApp -> restoreOverrideCursor();
    //    ui.actionGoPrevious -> setEnabled(false);
    //    ui.actionGoNext -> setEnabled(false);
    //    ui.actionEditCopy -> setEnabled(false);
    //    connect(tabs -> currentBrowser(), SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailable(bool)));

    //apply settings
    appsets -> set();
    appsets -> apply();

    // set the current selected item in the treeview

//    qDebug() << "[31]";
    helpDialog() -> locateContents(tabs -> currentBrowser() -> source().toString());
    connect(tabs, SIGNAL(browserUrlChanged(QString)), helpDock, SLOT(locateContents(QString)));
    projectModified(false);
    setupCompleted = true;
}

//-------------------------------------------------
void MainWindow::exportModule()
{
    //    helpDock->autosavestart = false;
    browsers() -> currentBrowser() -> fileSave();
    helpDock -> exportModule();
}
//-------------------------------------------------
void MainWindow::importModule()
{
    importm->show();
}
//-------------------------------------------------
void MainWindow::importModuleSuccessful()
{
    ModuleProperties pr;
    pr.moduleBiblename = Config::configuration()->ModuleBiblename();
    pr.moduleBibleShortName = Config::configuration()->ModuleBibleShortName();

    pr.moduleCopyright = Config::configuration()->ModuleCopyright();
    pr.moduleBVersion = 1.00;
    pr.prjFN = importm -> getPrjFN();
    pr.prjStartPage = importm -> getStartPage();
    pr.prjTitle = Config::configuration()->ModuleBiblename();


    // добавить другие параметры (дохера буленов и строк из модулепропертис)
    ProjectOpen(pr.prjFN);
    browsers() -> currentBrowser() -> fileSave();
    helpDock->saveProject();
}
//-------------------------------------------------
void MainWindow::importBookSuccessful()
{
    ModuleProperties pr;
    pr.moduleBiblename = Config::configuration()->ModuleBiblename();
    pr.moduleBibleShortName = Config::configuration()->ModuleBibleShortName();

    pr.moduleCopyright = Config::configuration()->ModuleCopyright();
    pr.moduleBVersion = 1.00;
    pr.prjFN = Config::configuration()->CurProject();
    pr.prjStartPage = importm -> getStartPage();

    //    qDebug() << " prjFN = " << pr.prjFN << " strartpage  = " << pr.prjStartPage;
    pr.prjTitle = Config::configuration()->ModuleBiblename();


    // добавить другие параметры (дохера буленов и строк из модулепропертис)
    ProjectOpen(pr.prjFN);
    browsers() -> currentBrowser() -> fileSave();
    helpDock->saveProject();
}
//-------------------------------------------------
void MainWindow::importBook()
{
    importdi->show();
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
    aboutd -> show();
}
//-------------------------------------------------
void MainWindow::showDocumentation()
{
    assistant->showDocumentation("index.html");
}
//-------------------------------------------------
void MainWindow::on_actionAboutAssistant_triggered()
{
    about();
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

    QString mylink;
    if (link.indexOf(Config::configuration()->CurPrjDir() <=0))
    {
        QString nameoffile = link.split("/").last();
        mylink = Config::configuration()->CurPrjDir()+"/"+nameoffile;
    }

    //    qDebug() << "link " << link;
    QString lnk = unurlifyFileName(link);
    //    qDebug() << "lnk " << lnk;
    //    qDebug() << "mylink " << mylink;
    //    QString test = Config::configuration()->PrjDir();
    //    QString test2 = Config::configuration()->CurPrjDir();
    //    QString test3 = Config::configuration()->CurFile();
    //    qDebug() << "test = " << test;
    //    qDebug() << "test2 = " << test2;
    //    qDebug() << "test3 = " << test3;

    //    qDebug() << "_____lnk = " << lnk;
    QFileInfo fi(lnk);
    if( (!lnk.isEmpty()) && fi.exists() && fi.isFile() ){
        // don't open a new tab for the same url more then once
        if (link == tabs -> currentBrowser() -> source().toString())
            return;
        if (ui.actionSaveFile -> isEnabled()) //i.e. document was modified
            emit saveOpenedLink();
        QUrl url(link);
        //qDebug() << "down!";
//        qDebug() << "[48]";
        tabs -> setSource(url.toString()); // w

//        qDebug() << "[49]";

        tabs -> currentBrowser() -> setFocus();

    }
    else
    {
        qWarning() << "Debug: _MainWindow::showLink()" << "Failed to open link: " << link;
        QMessageBox::warning(this, GL_PROG_NAME, tr("failed to open file:\n%1").arg(lnk));
    }

}

void MainWindow::modifededitor(bool my)
{
    //    if (my)

    my = my;
    //    if (my and helpDock->autosavestart)
    //    {
    //        qDebug() << "teeest" ;
    //        browsers() -> currentBrowser() -> fileSave();
    //    }
}

//-------------------------------------------------
// open several links in new tabs
void MainWindow::showLinks(const QStringList &links)
{
    if (links.size() == 0) {
        qWarning("MainWindow::showLinks() - Empty link");
        return;
    }

    if (links.size() == 1)
    {
        showLink(urlifyFileName(links.first()));
        return;
    }

    QStringList::ConstIterator it = links.begin();
    /// Initial showing, The tab is empty so update that without creating it first
    if (!tabs -> currentBrowser() -> source().isValid())
    {
        QPair<HelpWindow*, QString> browser;
        browser.first = tabs -> currentBrowser();
        browser.second = links.first();
        pendingBrowsers.append(browser);
        tabs -> setAppTitle(tabs -> currentBrowser(), tr("..."));
    }
    ++it;

    while(it != links.end())
    {
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
        //        qDebug() << "src = " << browser -> source().toString();
        lst << relatifyFileName(browser -> source().toString(), config -> PrjDir());
        //        qDebug() << "lst = " << lst;
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
{
    helpDock->autosavestart = false;
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
//        qDebug() << "[5]";
        helpDock -> on_BProjectAdd_clicked();
        Config::configuration() -> toAppLog(1, tr("- show start page: %1", "For log").arg(Config::configuration() -> CurFile()));
        showLink(urlifyFileName(Config::configuration() -> CurFile()));
        projectModified(false);
        Config::configuration() -> toPrjLog(1, "-------");
        Config::configuration() -> toPrjLog(1, tr("Project is opened.", "For log"));
    }
}

//-------------------------------------------------
void MainWindow::ProjectSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Project As"), Config::configuration() -> CurPrjDir(), tr("Project bQella (*.pep)"));
    if ( !fileName.isEmpty() ){
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(GL_PROJECT_FILE);
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
    int i = fn.lastIndexOf(QChar('.'));
    if (i > -1)
        fn = fn.left(i);
    QString relativeDestPath = fn + QString("_images");
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
                        int j = destName.lastIndexOf(QChar('/'));
                        if (j > -1)
                            destName = destName.mid(j+1);
                        QFileInfo info(from);
                        if (info.exists()) {
                            if (!QFile::copy(from, destDir.absolutePath()
                                             + QDir::separator() + destName))
                                continue;
                            fm.setName(QString("./") + relativeDestPath + QString("/") + destName);
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
    helpDock->autosavestart = false;
    ModuleProperties pr;
    //    qDebug() << "Debug: _MainWindow::ProjectNew()" << " prjdir = " <<  Config::configuration() -> PrjDir();

    bool newProject = true;

    prjprop->setModeNewProject(true);

    pr.prjTitle = tr("New Project", "Default project name");
    pr.moduleBiblename = "";
    pr.moduleBibleShortName = "";
    pr.moduleCopyright = "";
    pr.moduleBVersion = 1.0;
    pr.prjFN = Config::configuration() -> PrjDir()+pr.moduleBiblename;

    pr.moduleType = false;
    pr.oldTestament = false;
    pr.newTestament = false;
    pr.apocrypha = false;
    pr.chapterZero = false;
    pr.englishPsalms = false;
    pr.strongNumber = false;
    pr.strongsDirectory = "none";
    pr.soundDirectory = "none";
    pr.noForcedLineBreaks = false;
    pr.language = "rus";
    pr.installFonts = "none";
    pr.desiredFontName = "none";
    pr.categories = "none";
    pr.desiredFontPath = "none";
    pr.defaultEncoding = "utf-8";
    pr.desiredUIFont = "none";
    pr.useChapterHead = false;
    pr.useRightAlignment = false;


    prjprop->setModeNewProject(true);
    prjprop->showUpdate();
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


    //    qDebug() << "before = " << pr.oldTestament << pr.moduleType << Config::configuration() -> profile() -> props["type"] << pr.newTestament ;

    pr.moduleType = QStringtoBool(Config::configuration() -> profile() -> props["moduletype"]);
    pr.oldTestament = QStringtoBool(Config::configuration() -> profile() -> props["oldtestament"]);
    pr.newTestament = QStringtoBool(Config::configuration() -> profile() -> props["newtestament"]);
    pr.apocrypha = QStringtoBool(Config::configuration() -> profile() -> props["apocrypha"]);
    pr.chapterZero = QStringtoBool(Config::configuration() -> profile() -> props["chapterzero"]);
    pr.englishPsalms = QStringtoBool(Config::configuration() -> profile() -> props["endlishpsalms"]);
    pr.noForcedLineBreaks = QStringtoBool(Config::configuration() -> profile() -> props["noforcedlinebreaks"]);
    pr.useChapterHead = QStringtoBool(Config::configuration() -> profile() -> props["usechapterhead"]);
    pr.useRightAlignment = QStringtoBool(Config::configuration() -> profile() -> props["userightalignment"]);
    pr.strongNumber = QStringtoBool(Config::configuration() -> profile() -> props["strongsnumber"]);
    pr.strongsDirectory = Config::configuration() -> profile() -> props["strongsdirectory"];
    pr.soundDirectory = Config::configuration() -> profile() -> props["sounddirectory"];
    pr.language = Config::configuration() -> profile() -> props["language"];
    pr.installFonts = Config::configuration() -> profile() -> props["installfonts"];
    pr.desiredFontName = Config::configuration() -> profile() -> props["desiredfontname"];
    pr.categories = Config::configuration() -> profile() -> props["categories"];
    pr.desiredFontPath = Config::configuration() -> profile() -> props["desiredfontpath"];
    pr.defaultEncoding = Config::configuration() -> profile() -> props["defaultencoding"];
    pr.desiredUIFont = Config::configuration() -> profile() -> props["desireduifont"];

    //    qDebug() << "after = " << pr.oldTestament << pr.moduleType << pr.newTestament ;


    //    pr.moduleType = Config::configuration() -> profile() -> props["type"];
    bool newProject = false;
    prjprop -> setProperties(newProject, pr);
    prjprop -> showUpdate();
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
    ts << ind1 << "<property name=\"strongsdirectory\">" << Qt::escape(pr.strongsDirectory) << "</property>" << endl;
    ts << ind1 << "<property name=\"sounddirectory\">" << Qt::escape(pr.soundDirectory) << "</property>" << endl;
    ts << ind1 << "<property name=\"language\">" << pr.language << "</property>" << endl;
    ts << ind1 << "<property name=\"installfonts\">" << Qt::escape(pr.installFonts) << "</property>" << endl;
    ts << ind1 << "<property name=\"desiredfontname\">" << Qt::escape(pr.desiredFontName) << "</property>" << endl;
    ts << ind1 << "<property name=\"categories\">" << Qt::escape(pr.categories) << "</property>" << endl;
    ts << ind1 << "<property name=\"desiredfontpath\">" << Qt::escape(pr.desiredFontPath) << "</property>" << endl;
    ts << ind1 << "<property name=\"defaultencoding\">" << Qt::escape(pr.defaultEncoding) << "</property>" << endl;
    ts << ind1 << "<property name=\"desireduifont\">" << Qt::escape(pr.desiredUIFont) << "</property>" << endl;

    ts << ind1 << "<property name=\"moduletype\">" << Qt::escape(BooltoQString(pr.moduleType)) << "</property>" << endl;
    ts << ind1 << "<property name=\"oldtestament\">" << Qt::escape(BooltoQString(pr.oldTestament)) << "</property>" << endl;
    ts << ind1 << "<property name=\"newtestament\">" << Qt::escape(BooltoQString(pr.newTestament)) << "</property>" << endl;
    ts << ind1 << "<property name=\"apocrypha\">" << Qt::escape(BooltoQString(pr.apocrypha)) << "</property>" << endl;
    ts << ind1 << "<property name=\"chapterzero\">" << Qt::escape(BooltoQString(pr.chapterZero)) << "</property>" << endl;
    ts << ind1 << "<property name=\"englishpsalms\">" << Qt::escape(BooltoQString(pr.englishPsalms)) << "</property>" << endl;
    ts << ind1 << "<property name=\"strongnumber\">" << Qt::escape(BooltoQString(pr.strongNumber)) << "</property>" << endl;
    ts << ind1 << "<property name=\"noforcedlinebreaks\">" << Qt::escape(BooltoQString(pr.noForcedLineBreaks)) << "</property>" << endl;
    ts << ind1 << "<property name=\"userightalignment\">" << Qt::escape(BooltoQString(pr.useRightAlignment)) << "</property>" << endl;
    ts << ind1 << "<property name=\"usechapterhead\">" << Qt::escape(BooltoQString(pr.useChapterHead)) << "</property>" << endl;


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

    //    Config::configuration() -> setCurProject(fn);
    //    Config::configuration() -> setCurPrjDir(fi.absolutePath());
    //    Config::configuration() -> setCurPrjSrc();
    qDebug() << " curprj" << Config::configuration()->CurProject() << " curprjdir = " << Config::configuration()->CurPrjDir() << " curprjsouse = " << Config::configuration()->CurPrjSrc();
    ProjectOpen(fn);
}

//-------------------------------------------------
void MainWindow::updateProjectProperties(ModuleProperties pr)
{
    QString fn = unurlifyFileName(pr.prjFN);
    //    fn = Config::configuration()->AppDir()+"/projects/"+fn;
    QFileInfo fi(fn);
    Config::configuration() -> toPrjLog(1, tr("Update project properties:", "For log"));
    Config::configuration() -> toPrjLog(3, tr("- title      = %1", "For log").arg(pr.prjTitle));
    Config::configuration() -> toPrjLog(3, tr("- file name  = %1", "For log").arg(pr.prjFN));
    Config::configuration() -> toPrjLog(3, tr("- start page = %1", "For log").arg(pr.prjStartPage));
    Config::configuration() -> toPrjLog(3, tr("- module name      = %1", "For log").arg(pr.moduleBiblename));
    Config::configuration() -> toPrjLog(3, tr("- module short name  = %1", "For log").arg(pr.moduleBibleShortName));
    Config::configuration() -> toPrjLog(3, tr("- module copyright = %1", "For log").arg(pr.moduleCopyright));
    // потом логирование остальных пунктов добавить

    Config::configuration() -> profile() -> addProperty("title", pr.prjTitle);
    Config::configuration() -> profile() -> addProperty("startpage", pr.prjStartPage);
    Config::configuration() -> profile() -> addProperty("biblename", pr.moduleBiblename);
    Config::configuration() -> profile() -> addProperty("bibleshortname", pr.moduleBibleShortName);
    Config::configuration() -> profile() -> addProperty("copyright", pr.moduleCopyright);

    Config::configuration() -> profile() -> addProperty("moduletype", BooltoQString(pr.moduleType));
    Config::configuration() -> profile() -> addProperty("oldtestament", BooltoQString(pr.oldTestament));
    Config::configuration() -> profile() -> addProperty("newtestament", BooltoQString(pr.newTestament));
    Config::configuration() -> profile() -> addProperty("apocrypha", BooltoQString(pr.apocrypha));
    Config::configuration() -> profile() -> addProperty("chapterzero", BooltoQString(pr.chapterZero));
    Config::configuration() -> profile() -> addProperty("endlishpsalms", BooltoQString(pr.englishPsalms));
    Config::configuration() -> profile() -> addProperty("noforcedlinebreaks", BooltoQString(pr.noForcedLineBreaks));
    Config::configuration() -> profile() -> addProperty("usechapterhead", BooltoQString(pr.useChapterHead));
    Config::configuration() -> profile() -> addProperty("userightalignment", BooltoQString(pr.useRightAlignment));
    Config::configuration() -> profile() -> addProperty("strongsnumber", BooltoQString(pr.strongNumber));

    Config::configuration() -> profile() -> addProperty("strongsdirectory", pr.strongsDirectory);
    Config::configuration() -> profile() -> addProperty("sounddirectory", pr.soundDirectory);
    Config::configuration() -> profile() -> addProperty("language", pr.language);
    Config::configuration() -> profile() -> addProperty("installfonts", pr.installFonts);
    Config::configuration() -> profile() -> addProperty("desiredfontname", pr.desiredFontName);
    Config::configuration() -> profile() -> addProperty("categories", pr.categories);
    Config::configuration() -> profile() -> addProperty("desiredfontpath", pr.desiredFontPath);
    Config::configuration() -> profile() -> addProperty("defaultencoding", pr.defaultEncoding);
    Config::configuration() -> profile() -> addProperty("desireduifont", pr.desiredUIFont);
    //    Config::configuration() -> profile() -> addProperty("type", pr.moduleType);

    QString version;
    version.setNum(pr.moduleBVersion);
//    qDebug() << "Debug: _MainWindow::updateProjectProperties:" << "version(str) = " << version << "version(double) = " << pr.moduleBVersion << "bibletype = " << pr.moduleType;
//    qDebug() << "Debug: _MainWindow::updateProjectProperties:" << "path = " << fn << " test = " << Config::configuration()->AppDir()+fn;
    Config::configuration() -> profile() -> addProperty("version", version);


    Config::configuration() -> setCurProject(fn);
    Config::configuration() -> setCurPrjDir(fi.absolutePath());
    Config::configuration() -> setCurPrjSrc();
    Config::configuration() -> toPrjLog(1, tr("- done", "For log"));
    helpDock->saveProject(fn);
    //    ProjectOpen(fn);
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
void MainWindow::globalShortcut_CtrlShiftInsert()
{
    if (QApplication::focusWidget() -> objectName() == "WorkArea"){
        //  insertDefaultSignature();
    }

}

//-------------------------------------------------
void MainWindow::projectModified(bool modified){
    ui.actionProjectSave -> setEnabled(modified);
}
