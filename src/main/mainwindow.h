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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "projectproperty.h"
#include "settings.h"
#include "config.h"
#include "raedit.h"
#include "fontsettingsdialog.h"
//#include "projectsettings.h"


#include <QPointer>
#include <QMap>
#include <QTreeWidget>

class TabbedBrowser;
class HelpDialog;
class HelpWindow;
class Export;
class QMenu;
class QDockWidget;

class GUIAbout;

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
    Export *exportModule();


    void setupPopupMenu(QMenu *menu);
    void removePendingBrowser(HelpWindow *win);
    void msgReloadRequest();
    void OpenInExternalApplication(QString app, QString FileName);

signals:
    void saveOpenedLink(); //used to save opened file before open a new one

public slots:
    MainWindow *newWindow();

    void setup();
    void showLink(const QString &link);
    void showLinks(const QStringList &links);
    void saveSettings();
    void ProjectOpen(QString fileName);
    void updateAppFont(FontSettings settings);
    void projectModified(bool modified);

private slots:
    void ProjectNew();
    void ProjectNewDiary();
    void ProjectNewNotebook();
    void createProject(QString prjTitle, QString prjFN, QString prjStartPage);
    void updateProjectProperties(QString prjTitle, QString prjFN, QString prjStartPage);
    void ProjectOpen();
    void ProjectSaveAs();
    void ProjectBackup();
    void ProjectProps();
    void ProjectSrc();
    void setLangEn();
    void setLangRu();
    void showAppSettings();
    void globalShortcut_CtrlShiftInsert();    
    void on_actionNewWindow_triggered();
    void on_actionFilePrint_triggered();
    void on_actionAboutAssistant_triggered();
    void on_actionSaveFileAs_triggered();
    void about();
    void showLinkFromClient(const QString &link);
    void showSearchLink(const QString &link, const QStringList &terms);
    void updateProfileSettings();
    void browserTabChanged();
    void updateTabActions(int index);
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
    QDockWidget *dw;
    static QList<MainWindow*> windows;
    //    QMap<QAction*,QString> *goActionDocFiles;
    QList<QPair<HelpWindow*,QString> > pendingBrowsers;
    QMenu *menuSign;

    GUIAbout *m_gui_About;
};

#endif // MAINWINDOW_H