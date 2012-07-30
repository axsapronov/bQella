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
#include "config.h"
#include "pcommon.h"

#include <QApplication>


int main( int argc, char ** argv )
{
//    Q_INIT_RESOURCE(ra); //Initialize the resources specified by the .qrc file

//    QString aDocPath;
    bool hideSidebar = false;
    bool withGUI = true;
    QApplication a(argc, argv, withGUI);
    a.setOrganizationName(GL_PROG_AUTHOR);
    a.setApplicationName(GL_PROG_NAME);

    Config *conf = new Config();
    conf -> setAppDir(QDir::currentPath() + "/");
    conf -> setCacheDir(conf -> AppDir() + "cache/");
    conf -> setImgDir(conf -> AppDir() + "images/");
    conf -> setPrjDir(conf -> AppDir() + "projects/");
    conf -> setIniFile(conf -> AppDir() + "project.ini");
    conf -> setErrPage(conf -> AppDir() + "last-error.html");
    conf -> setAppLogFN(conf -> AppDir() + "log/project.log");
    toLog(conf -> AppLogFN(), "-------");
    toLog(conf -> AppLogFN(), "Project bQella started.");
    conf -> loadSettings();
    conf -> loadProject(conf -> CurProject());

    if (!conf) {
        fprintf( stderr, "Profile '%s' does not exist!\n", conf -> CurProject().toUtf8().constData() );
        fflush( stderr );
        return -1;
    }
    //!+! load language based on locale on first startup or if it was not set directly
    QTranslator translator;
    if (conf -> Lang() == "Russian" or conf->Lang().isEmpty()) translator.load("bqella_ru",":lang/lang");
    if (conf -> Lang() == "Deutch") translator.load("bqella_de",":lang/lang");
    if (conf -> Lang() == "France") translator.load("bqella_fr",":lang/lang");
    a.installTranslator(&translator);
    conf -> hideSideBar( hideSidebar );
//    qDebug() << "[10]";
    QPointer<MainWindow> mw = new MainWindow();

//    qDebug() << "[11]";
    FontSettings settings = conf -> fontSettings();
    if (mw -> font() != settings.windowFont)
        a.setFont(settings.windowFont, "QWidget");
    toLog(conf -> AppLogFN(), "Done with setup. Show main window.");
    mw -> show();


//    qDebug() << "[12]";
    QStringList links = conf -> source();
    if (links.isEmpty())
    {
        //!+! or option "Remember opened files" is not set
        mw -> showLink( urlifyFileName(conf -> CurFile()) );
    }
    else
    {//        qDebug() << "[13]";
        mw -> showLinks( links );
    }
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );

    int appExec = a.exec();
    delete (MainWindow*)mw;
    toLog(conf -> AppLogFN(),QString("%1 shuts down normally.").arg(GL_PROG_NAME));
    return appExec;
}

/********************* Comments to project *********************

--------------------- Files description ---------------------

/**
  * @file
  *
  *
  * config	- configuration of the application, global functions and variables
  * docuparser 	- parse profile file
  * helpdialog	- slidebar functions: content, index, bookmarks, search
  * helpwindow	- working area functions: text edit, font, file
  * index 	- procedures to build up index in index tab on helpdialog slidebar
  * mainwindow 	- main menu functions
  * profile 	- configuration of a project
  * msc		- general purpose procedures with no class relations
  *
  *
  * ------- Forms aviable from (classes interaction) -------
  *
  * HelpDialog		Ui::HelpDialog ui;		MainWindow *mw;
  * HelpWindow		MainWindow *mw;			ItemProperties *itemprop;
  * MainWindow		Ui::MainWindow ui;		TabbedBrowser *tabs;[private]	HelpDialog *helpDock;[private]
  * TabbedBrowser *browsers();		HelpDialog *helpDialog();
  * TabbedBrowser	Ui::TabbedBrowser ui;	MainWindow *mainWindow();		HelpWindow *currentBrowser();
  *
  * To access from HelpDialog to HelpWindow functions use:
  * mw -> browsers() -> currentBrowser() -> ...
  *
  */

