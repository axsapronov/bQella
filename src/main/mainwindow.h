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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "projectproperty.h"
#include "settings.h"
#include "config.h"
#include "raedit.h"
#include "fontsettingsdialog.h"

#include <QMap>
#include <QPointer>

class QMenu;
class QDockWidget;
class QTreeWidget;
class TabbedBrowser;
class HelpDialog;
class HelpWindow;
class AboutDialog;
class Export;
class Import;
class FRDialog;
class Assistant;
class ImportBookDialog;

//====================== class MainWindow ============================

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();
    Ui::MainWindow ui;

    TabbedBrowser *browsers() const;
    HelpDialog *helpDialog() const;

    void setupPopupMenu(QMenu *menu);
    void removePendingBrowser(HelpWindow *win);
    void msgReloadRequest();
    void OpenInExternalApplication(QString app, QString FileName);

    void showDocumentation(const QString &file);
signals:
    void saveOpenedLink(); //used to save opened file before open a new one

public slots:
    MainWindow *newWindow();


    void modifededitor(bool my);

    void setup();
    void showLink(const QString &link);
    void showLinks(const QStringList &links);
    void saveSettings();
    void ProjectOpen(QString fileName);
    void updateAppFont(FontSettings settings);
    void projectModified(bool modified);

private slots:
    void ProjectNew();
//    void ProjectNewDiary();
//    void ProjectNewNotebook();
    void createProject(ModuleProperties);
    void updateProjectProperties(ModuleProperties);
    void ProjectOpen();
    void ProjectSaveAs();
    void ProjectProps();
    void setLangEn();
    void setLangRu();
    void showAppSettings();
    void globalShortcut_CtrlShiftInsert();    
    void on_actionNewWindow_triggered();
    void on_actionAboutAssistant_triggered();
    void on_actionSaveFileAs_triggered();
    void about();
    void showLinkFromClient(const QString &link);
    void showSearchLink(const QString &link, const QStringList &terms);
    void updateProfileSettings();
    void browserTabChanged();
    void updateTabActions(int index);

    void showDocumentation();
    // export
    void exportModule();

    // import
    void importModule();
    void importBook();
    void importModuleSuccessful();

    void exitApp();

protected:
    void closeEvent(QCloseEvent *);
    void timerEvent(QTimerEvent *);

private:
    //    bool insertActionSeparator();
    QList<QAction*> goActions;
    uint setupCompleted:1;
    QMap<QAction*, QString> bookmarks;
    TabbedBrowser *tabs;
    ProjectProperties *prjprop;
    //ProjectSettings *prjset;
    AppSettings *appsets;
    HelpDialog *helpDock;
    FRDialog *frdialog;
    Assistant *assistant;

    Export * exportm;

    // import
    Import * importm;
    ImportBookDialog * importdi;

    AboutDialog *aboutd;    // about dialog


    QDockWidget *dw;

    static QList<MainWindow*> windows;
    //    QMap<QAction*,QString> *goActionDocFiles;
    QList<QPair<HelpWindow*,QString> > pendingBrowsers;
    QMenu *menuSign;
};

#endif // MAINWINDOW_H
