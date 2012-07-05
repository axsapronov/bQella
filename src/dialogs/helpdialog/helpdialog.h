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


#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include "ui_helpdialog.h"
#include "index.h"
#include "helpwindow.h"
#include "docuparser.h"

#include <QList>
#include <QPair>
#include <QListWidget>
#include <QTreeWidget>
#include <QMap>
#include <QStringList>
#include <QValidator>
#include <QMenu>
#include <QHash>
#include <QString>

class QProgressBar;
class MainWindow;
class raEdit;
class IndexListModel;
class Export;

class BookAddDialog;

//==================== class HelpNavigationListItem ====================

class HelpNavigationListItem : public QListWidgetItem
{
public:
    HelpNavigationListItem(QListWidget *ls, const QString &txt);

    void addLink(const QString &link);  //adds link to linkList
    QStringList links() const { return linkList; }

private:
    QStringList linkList;
}; // class HelpNavigationListItem

//==================== class SearchValidator ====================

class SearchValidator : public QValidator
{
    Q_OBJECT
public:
    SearchValidator(QObject *parent)
        : QValidator(parent) {}
    ~SearchValidator() {}
    QValidator::State validate(QString &str, int &) const;
}; // class SearchValidator

//==================== class HelpDialog ====================

class HelpDialog : public QWidget
{
    Q_OBJECT
public:
    HelpDialog(QWidget *parent, MainWindow *h);

    inline QTabWidget *tabWidget() const  { return ui.tabWidget; }
    QString titleOfLink(const QString &link);
    bool eventFilter(QObject *, QEvent *);
    bool lastWindowClosed() { return lwClosed; }

    void timerEvent(QTimerEvent *e);
    void saveProject(QString profileFN);  //save contents and keywords to xml
    void enableProjectButtons();

    bool autosavestart;
signals:
    void showLink(const QString &s);
    void showSearchLink(const QString &s, const QStringList &terms);

public slots:
    void initialize();	//setup connections and actions and call initTabs()
    void initTabs();	//clear all tabs
    void currentTabChanged(int index);
    void locateContents(const QString &link);  //find and select contents item corresponding to link
    void insertContents();
    void InsertContentsItem(QString title, QString fileName); //insert new document in contents
    void InsertChapter(QTreeWidgetItem * book, QString title, QString fileName); //

    void updateItemProperties(QString title, QString fileName); //update current item properties
    void showItemProperties();  //open item property dialog
    void removeItem(); 			//remove item from contents, ask for sure
    void removeItemDontAsk();	//remove item without asking
    void deleteItem(); 			//remove item from contents and delete linked file
    void showContentsAV(bool b);
    void showContentsAVHeader(bool b);
    void saveProject(); 		//save current project

    QString getTest();
    QString getFileBibleqtName();
    QString getFileName(int i);

    int getTopLevelItemCount();

    void exportModule();
    void exportBibleBook(QString filebook, QString i);

private slots:
    void newItem();
    //	void on_listContents_itemClicked(QTreeWidgetItem* item, int column);
    void on_BProjectAdd_clicked();
    void on_BProjectDelete_clicked();
    void loadIndexFile();
    void setupFullTextIndex();
    void showTopic();
    void toggleContents();
    void lastWinClosed();
    void setIndexingProgress(int prog);
    void showContentsItemMenu(const QPoint &pos);
    void showTreeItemMenu(const QPoint &pos);
    void processEvents();
    void loadProjectFromList(int prjIindex);
    void fillSubList();
    void cloneItem(QTreeWidgetItem *item, bool subItem);
    void cloneDone();
    void showTopic(QTreeWidgetItem *);
    void currentItemChanged(QTreeWidgetItem* curItem, QTreeWidgetItem* prevItem);
    void itemChanged(QTreeWidgetItem* item, int column);
    void sortDisable();
    void sortAscending();
    void sortDescending();
    void locateItemByName(QString name);
    void nextMatchedItem();

    QString getNameFolder(QString cur_dir);

    void fixedBookConfFile(QString,QTreeWidgetItem*,QString);
    // void prevMatchedItem();
    //void GoToMatchedItem(int n);
    


private:
    void removeOldCacheFiles(bool onlyFulltextSearchIndex = false);
    void buildKeywordDB();
    quint32 getFileAges();
    void showIndexTopic();
    void setupTitleMap();
    void saveBookmarks();
    void store(QTreeWidget *tw, QTextStream &ts); 	//for saving bookmarks
    void store(QTreeWidgetItem *i, QTextStream &ts); //for saving bookmarks
    void showContentsTopic();
    void showInitDoneMessage();
    void buildContentDict();
    QTreeWidgetItem * locateLink(QTreeWidgetItem *item, const QString &link);
    bool findItemByName(QString &name);
    bool findItemByName(QTreeWidgetItem *i, QString &name);
    void triggerAction(QTreeWidgetItem *item, QAction *action);




    Ui::HelpDialog ui;
    MainWindow *mw;
    Export * exportf;
    BookAddDialog *m_bookadddialog;


    typedef QList<ContentItem> ContentList;	//ContentItem structure is described in docuparser.h
    ContentList contentList;
    IndexListModel *indexModel;
    Index *fullTextIndex;
    QMap<QString, QString> titleMap;
    bool indexDone, bookmarksInserted, titleMapDone, contentsInserted;
    bool lwClosed;
    QString documentationPath;
    QStringList terms, foundDocs;
    bool initDoneMsgShown;
    void getAllContents();	//fill up contents from profile
    QString cacheFilesPath;	//path to store cache for search - it is user home dir
    QStringList keywordDocuments;	//for full text search
    int liTopLevelIndex; 			//for locate item procedures
    int liChildIndex, curChildIndex;//for locate item procedures
    QString str_NewItemTitle;
    bool newSameLevelItem;	//create a new item as a subitem or on the same level

    QMenu *itemPopup;
    QMenu *itemPopupContents;
    QMenu *itemPopupSubItems;
    QAction *actionItemProperties;
    QAction *actionItemInsert;
    QAction *actionItemAddFile;
    QAction *actionItemInsertSameLevel;
    QAction *actionItemAddFileSameLevel;
    QAction *actionItemRemove;
    QAction *actionItemDelete;
    QAction *actionItemAuto;
    QAction *actionOpenCurrentTab;
    QAction *actionOpenLinkInNewWindow;
    QAction *actionOpenLinkInNewTab;
    QAction *actionOpenLinkInExtEditor;
    QAction *actionOpenLinkInExtBrowser;
    QAction *actionItemChapterAdd;
    QAction *actionItemVerseAdd;
    QAction *actionItemBookAdd;

}; //class HelpDialog

#endif // HELPDIALOG_H
