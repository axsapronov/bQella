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
#include "helpdialog.h"
#include "helpwindow.h"
#include "topicchooser.h"
#include "docuparser.h"
#include "mainwindow.h"
#include "config.h"
#include "tabbedbrowser.h"
#include "pcommon.h"

#include "bookadddialog.h"


#include <QtGui>
#include <QtDebug>
#include <QFile>   		
#include <QTextStream>	
#include <stdlib.h>
#include <limits.h>

//The roles are used by the view to indicate to the model which type of data it needs.
// Qt::UserRole =  32; The first role that can be used for application-specific purposes.
//These roles are set for QTreeWidgetItem::setData(), when building contents
// !+! move this enum to config.h since the same is in autoitems.cpp
enum { LinkRole   = Qt::UserRole + 1000, 	// reference = file name = link to source
       IconFNRole = Qt::UserRole + 1001 };	// icon file name

enum { ContTreeView = 1, ContSubItems = 2}; // to indicate current contents view ui.listContents=1 or ui.TWSubItems=2  
int ContCur = ContTreeView;

// for indent in xml file for save project procedure
QString ind1="   ";		//number of spaces (or tabs) for one indent
QString indent = ind1; 	//=(ind1 * number_of_indents) - subsection depth  


//-------------------------------------------------
struct IndexKeyword {
    //--- constructors ---
    IndexKeyword(const QString &kw, const QString &l)
        : keyword(kw), link(l) {}
    IndexKeyword() : keyword(QString()), link(QString()) {}

    //--- operators ---
    bool operator<(const IndexKeyword &ik) const {
        return keyword.toLower() < ik.keyword.toLower();
    }
    bool operator<=(const IndexKeyword &ik) const {
        return keyword.toLower() <= ik.keyword.toLower();
    }
    bool operator>(const IndexKeyword &ik) const {
        return keyword.toLower() > ik.keyword.toLower();
    }
    Q_DUMMY_COMPARISON_OPERATOR(IndexKeyword) //?-? why do we need it?

    //--- variables ---
    QString keyword;
    QString link;
};

//-------------------------------------------------
QDataStream &operator>>(QDataStream &s, IndexKeyword &ik)
{
    s >> ik.keyword;
    s >> ik.link;
    return s;
}

//-------------------------------------------------
QDataStream &operator<<(QDataStream &s, const IndexKeyword &ik)
{
    s << ik.keyword;
    s << ik.link;
    return s;
}

//-------------------------------------------------
bool caseInsensitiveLessThan(const QString &as, const QString &bs)
{
    const QChar *a = as.unicode();
    const QChar *b = bs.unicode();
    if (a == 0)
        return true;
    if ((b == 0) or (a == b))
        return false;
    int l=qMin(as.length(),bs.length());
    while (l-- && QChar::toLower(a->unicode()) == QChar::toLower(b->unicode()))
        a++,b++;
    if (l==-1)
        return (as.length() < bs.length());
    return QChar::toLower(a->unicode()) < QChar::toLower(b->unicode());
}

//==================== class IndexListModel ====================
class IndexListModel: public QStringListModel
{
public:
    IndexListModel(QObject *parent = 0)
        : QStringListModel(parent) {}

    void clear() { contents.clear(); setStringList(QStringList()); }

    QString description(int index) const { return stringList().at(index); }
    QStringList links(int index) const { return contents.values(stringList().at(index)); }
    void addLink(const QString &description, const QString &link) { contents.insert(description, link); }

    void publish() { filter(QString(), QString()); }

    QModelIndex filter(const QString &s, const QString &real);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    { return QStringListModel::flags(index) & ~Qt::ItemIsEditable; }

private:
    QMultiMap<QString, QString> contents;
};

//-------------------------------------------------
/**
 * \a real is kinda a hack for the smart search, need a way to match a regexp to an item
 * How would you say the best match for Q.*Wiget is QWidget?
 */
QModelIndex IndexListModel::filter(const QString &s, const QString &real)
{
    QStringList list;

    int goodMatch = -1;
    int perfectMatch = -1;
    if (s.isEmpty())
        perfectMatch = 0;

    const QRegExp regExp(s, Qt::CaseInsensitive);
    QMultiMap<QString, QString>::iterator it = contents.begin();
    QString lastKey;
    for (; it != contents.end(); ++it) {
        if (it.key() == lastKey)
            continue;
        lastKey = it.key();
        const QString key = it.key();
        if (key.contains(regExp) || key.contains(s, Qt::CaseInsensitive)) {
            list.append(key);
            if (perfectMatch == -1 && (key.startsWith(real, Qt::CaseInsensitive))) {
                if (goodMatch == -1)
                    goodMatch = list.count() - 1;
                if (real.length() == key.length()){
                    perfectMatch = list.count() - 1;
                }
            }  else if (perfectMatch > -1 && s == key) {
                perfectMatch = list.count() - 1;
            }
        }
    }

    int bestMatch = perfectMatch;
    if (bestMatch == -1)
        bestMatch = goodMatch;
    bestMatch = qMax(0, bestMatch);

    // sort the new list
    QString match;
    if (bestMatch >= 0 && list.count() > bestMatch)
        match = list[bestMatch];
    qSort(list.begin(), list.end(), caseInsensitiveLessThan);
    setStringList(list);
    for (int i = 0; i < list.size(); ++i) {
        if (list.at(i) == match){
            bestMatch = i;
            break;
        }
    }
    return index(bestMatch, 0, QModelIndex());
}

//==================== class SearchValidator ====================
QValidator::State SearchValidator::validate(QString &str, int &) const
{
    for (int i = 0; i < (int) str.length(); ++i) {
        QChar c = str[i];
        if (!c.isLetterOrNumber() && c != QLatin1Char('\'') && c != QLatin1Char('`')
                && c != QLatin1Char('\"') && c != QLatin1Char(' ') && c != QLatin1Char('-') && c != QLatin1Char('_')
                && c!= QLatin1Char('*'))
            return QValidator::Invalid;
    }
    return QValidator::Acceptable;
}

//==================== class HelpNavigationListItem ====================
HelpNavigationListItem::HelpNavigationListItem(QListWidget *ls, const QString &txt)
    : QListWidgetItem(txt, ls)
{
}

void HelpNavigationListItem::addLink(const QString &link)
{
    QString lnk = removeAnchorFromLink(link);
    if (linkList.filter(lnk, Qt::CaseInsensitive).count() > 0)
        return;
    linkList << link;
}

//==================== class HelpDialog ====================
HelpDialog::HelpDialog(QWidget *parent, MainWindow *h)
    : QWidget(parent), mw(h),
      m_bookadddialog(new BookAddDialog) ,
      lwClosed(false)
{
    ui.setupUi(this);
    ui.listContents->setUniformRowHeights(true);
    ui.listContents->header()->setStretchLastSection(false);
    ui.listContents->header()->setResizeMode(QHeaderView::ResizeToContents);
    ui.listBookmarks->setUniformRowHeights(true);
    ui.listBookmarks->header()->setStretchLastSection(false);
    ui.listBookmarks->header()->setResizeMode(QHeaderView::ResizeToContents);

    indexModel = new IndexListModel(this);
    ui.listIndex->setModel(indexModel);
    ui.listIndex->setLayoutMode(QListView::Batched);
    ui.listBookmarks->setItemHidden(ui.listBookmarks->headerItem(), true);
    ui.listContents->setItemHidden(ui.listContents->headerItem(), true);
    ui.TWSubItems->setItemHidden(ui.TWSubItems->headerItem(), !Config::configuration()->ShowSubItemsTitle());

    dialogAutoItems = new AutoItems(this);
    str_NewItemTitle = tr("New item");
}

//-------------------------------------------------
void HelpDialog::initialize()
{
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
    ui.tabWidget->setElideMode(Qt::ElideNone);	//show tab titles, do not elide them by placing "..."
    ui.tabWidget->removeTab(1); //hide Index tab 

    connect(ui.listContents, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContentsItemMenu(QPoint)));
    connect(ui.listContents, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(ui.listContents, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(itemChanged(QTreeWidgetItem*, int)));
    ui.listContents->viewport()->installEventFilter(this);	//for mouse
    ui.listContents->installEventFilter(this);				//for keyboard
    connect(ui.TWSubItems, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContentsItemMenu(QPoint))); //!+! does not connect
    ui.TWSubItems->viewport()->installEventFilter(this);
    ui.TWSubItems->installEventFilter(this);
    connect(ui.CBProjects, SIGNAL(activated(int)), this, SLOT(loadProjectFromList(int)));
    connect(dialogAutoItems, SIGNAL(cloneItem(QTreeWidgetItem*, bool)), this, SLOT(cloneItem(QTreeWidgetItem*, bool)));
    connect(dialogAutoItems, SIGNAL(cloneDone()), this, SLOT(cloneDone()));
    connect(ui.PBSortAscending, SIGNAL(pressed()), this, SLOT(sortAscending()));
    connect(ui.PBSortDescending, SIGNAL(pressed()), this, SLOT(sortDescending()));
    connect(ui.PBSortDisable, SIGNAL(pressed()), this, SLOT(sortDisable()));
    connect(ui.LELocateItemByName, SIGNAL(textChanged(QString)), this, SLOT(locateItemByName(QString)));
    connect(ui.LELocateItemByName, SIGNAL(returnPressed()), this, SLOT(nextMatchedItem()));
    //   connect(ui.actionItemRemove, SIGNAL(targed()), this, SLOT(removeItem()));
    //   connect(ui.actionItemDelete, SIGNAL(targed()), this, SLOT(deleteItem()));
    
    connect(ui.editIndex, SIGNAL(returnPressed()), this, SLOT(showTopic()));
    connect(ui.editIndex, SIGNAL(textEdited(QString)), this, SLOT(searchInIndex(QString)));

    connect(ui.listIndex, SIGNAL(activated(QModelIndex)), this, SLOT(showTopic()));
    connect(ui.listIndex, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showIndexItemMenu(QPoint)));

    connect(ui.listBookmarks, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(showTopic(QTreeWidgetItem*)));
    connect(ui.listBookmarks, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showTreeItemMenu(QPoint)));

    connect(ui.termsEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateSearchButton(const QString&)));
    connect(ui.resultBox, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showListItemMenu(QPoint)));
    connect(qApp, SIGNAL(lastWindowClosed()), SLOT(lastWinClosed()));

    //
    //connect(m_gui_Setting, SIGNAL(settingsChanged()), this , SLOT(mean_calculate()));
    connect(m_bookadddialog, SIGNAL(signalbookaddChanged()),this,SLOT(newItem()));
    //  connect(m_bookadddialog, SIGNAL(signalbookaddChanged()),this,SLOT(close()));


    if (Config::configuration()->ContentsSortOrder().toLower() == "ascending"){
        ui.PBSortAscending->setChecked(true);
        sortAscending();
    }else if (Config::configuration()->ContentsSortOrder().toLower() == "descending"){
        ui.PBSortDescending->setChecked(true);
        sortDescending();
    }else{
        ui.PBSortDisable->setChecked(true);
        sortDisable();
    }

    ui.editIndex->installEventFilter(this);
    ui.framePrepare->hide();
    ui.termsEdit->setValidator(new SearchValidator(ui.termsEdit));
    
    //Popup menu actions
    actionOpenCurrentTab = new QAction(this);
    actionOpenCurrentTab->setText(tr("Open in Current Tab"));

    actionOpenLinkInNewWindow = new QAction(this);
    actionOpenLinkInNewWindow->setText(tr("Open in New Window"));

    actionOpenLinkInNewTab = new QAction(this);
    actionOpenLinkInNewTab->setText(tr("Open in New Tab"));

    actionOpenLinkInExtEditor = new QAction(this);
    actionOpenLinkInExtEditor->setText(tr("Open in External Editor"));
    actionOpenLinkInExtBrowser = new QAction(this);
    actionOpenLinkInExtBrowser->setText(tr("Open in External Browser"));
    
    itemPopup = new QMenu(this);  //for Index,Bookmark and Search tabs
    itemPopup->addAction(actionOpenCurrentTab);
    itemPopup->addAction(actionOpenLinkInNewWindow);
    itemPopup->addAction(actionOpenLinkInNewTab);
    itemPopup->addAction(actionOpenLinkInExtEditor);
    itemPopup->addAction(actionOpenLinkInExtBrowser);




    

    actionItemProperties = new QAction(this);
    actionItemProperties->setText(tr("Item properties..."));
    actionItemProperties->setShortcut( QKeySequence(Qt::ControlModifier + Qt::Key_Return) ); //!+! shortcuts does not trigger action, see eventFilter()
    


    actionItemBookAdd = new QAction(this);
    actionItemBookAdd->setText(tr("Add book"));
    actionItemBookAdd->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_1));

    actionItemChapterAdd = new QAction(this);
    actionItemChapterAdd->setText(tr("Add chapter"));
    actionItemChapterAdd->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_2));

    actionItemVerseAdd = new QAction(this);
    actionItemVerseAdd->setText(tr("Add verse"));
    actionItemVerseAdd->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_3));







    actionItemInsert = new QAction(this);
    actionItemInsert->setText(tr("Insert new subitem (and create source file)"));
    actionItemInsert->setShortcut( QKeySequence(Qt::Key_Insert) );
    
    actionItemAddFile = new QAction(this);
    actionItemAddFile->setText(tr("Insert file as new subitem..."));
    actionItemAddFile->setShortcut( QKeySequence(Qt::ShiftModifier + Qt::Key_Insert) );
    
    actionItemInsertSameLevel = new QAction(this);
    actionItemInsertSameLevel->setText(tr("Insert new item (and create source file)"));
    //actionItemInsertSameLevel->setShortcut( QKeySequence(Qt::ControlModifier + Qt::Key_1) );
    
    actionItemAddFileSameLevel = new QAction(this);
    actionItemAddFileSameLevel->setText(tr("Insert file as new item..."));
    //actionItemAddFileSameLevel->setShortcut( QKeySequence(Qt::ControlModifier + Qt::Key_2) );

    actionItemAuto = new QAction(this);
    actionItemAuto->setText(tr("Automaticly create items..."));
    actionItemAuto->setShortcut( QKeySequence(Qt::ControlModifier + Qt::Key_A) );
    
    actionItemRemove = new QAction(this);
    actionItemRemove->setText(tr("Remove item (do not touch source file)"));
    actionItemRemove->setShortcut( QKeySequence(Qt::Key_Delete) );
    
    actionItemDelete = new QAction(this);
    actionItemDelete->setText(tr("Delete item and source file"));
    actionItemDelete->setShortcut( QKeySequence(Qt::ShiftModifier + Qt::Key_Delete) );

    itemPopupContents = new QMenu(this);  //for Contents tab
    //itemPopupContents->addAction(actionItemProperties);
    //itemPopupContents->addSeparator();
    itemPopupContents->addAction(actionItemBookAdd);
    itemPopupContents->addAction(actionItemChapterAdd);
    //itemPopupContents->addAction(actionItemVerseAdd);
    itemPopupContents->addSeparator();
    /*
    itemPopupContents->addAction(actionItemInsert);
    itemPopupContents->addAction(actionItemAddFile);
    itemPopupContents->addAction(actionItemInsertSameLevel);
    itemPopupContents->addAction(actionItemAddFileSameLevel);
    */
    itemPopupContents->addAction(actionItemRemove);
    /*
    itemPopupContents->addAction(actionItemDelete);
    itemPopupContents->addSeparator();
    itemPopupContents->addAction(actionItemAuto);

    itemPopupContents->addSeparator();
    itemPopupContents->addAction(actionOpenCurrentTab);
    itemPopupContents->addAction(actionOpenLinkInNewWindow);
    itemPopupContents->addAction(actionOpenLinkInNewTab);
    itemPopupContents->addAction(actionOpenLinkInExtEditor);
    itemPopupContents->addAction(actionOpenLinkInExtBrowser);
    */
    itemPopupContents->addSeparator();
    
    itemPopupSubItems = new QMenu(this); //for SubItems contents view
    itemPopupSubItems->addAction(actionItemProperties);
    itemPopupSubItems->addSeparator();
    itemPopupSubItems->addAction(actionOpenCurrentTab);
    itemPopupSubItems->addAction(actionOpenLinkInNewWindow);
    itemPopupSubItems->addAction(actionOpenLinkInNewTab);
    itemPopupSubItems->addAction(actionOpenLinkInExtEditor);
    itemPopupSubItems->addAction(actionOpenLinkInExtBrowser);
    
    //create projects list
    QString title;
    QStringList sl = Config::configuration()->ProfileFNs();    
    QStringList::iterator it = sl.begin();
    for (; it != sl.end(); ++it) {
    	title = Config::configuration()->getProjectProperty("title", *it);
    	if (!title.isEmpty()){ //it is empty when it's something wrong with project
            ui.CBProjects->insertItem(0, title );
            ui.CBProjects->setItemData(0, *it, LinkRole);
        }
    }

    int ind = sl.indexOf(Config::configuration()->CurProject());	//!+! stores index in unsorted list
    ui.CBProjects->setCurrentIndex(ind);
    bool prjInList = false;
    if (ind >=0)
        prjInList = true;
    ui.BProjectDelete->setEnabled(prjInList);
    ui.BProjectAdd->setEnabled(!prjInList);
    enableProjectButtons();
    newSameLevelItem = false;	// This should be set in create-item functions. Init here just in case

    initTabs();

    //ui.listContents->setColumnCount(3);
}

//-------------------------------------------------
void HelpDialog::initTabs()
{
    contentList.clear();

    initDoneMsgShown = false;
    fullTextIndex = 0;
    indexDone = false;
    titleMapDone = false;
    contentsInserted = false;
    bookmarksInserted = false;
    setupTitleMap();

    //ui.listContents->setColumnCount(3);
}

//-------------------------------------------------
void HelpDialog::processEvents()
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

//-------------------------------------------------
void HelpDialog::lastWinClosed()
{
    lwClosed = true;
}

//-------------------------------------------------
void HelpDialog::removeOldCacheFiles(bool onlyFulltextSearchIndex)
{
    if (!verifyDirectory(Config::configuration()->CacheDir())) {
        qWarning("Failed to create assistant directory");
        return;
    }
    QString pname = QLatin1String(".") + Config::configuration()->profileName();

    QStringList fileList;
    fileList << QLatin1String("indexdb40.dict")
             << QLatin1String("indexdb40.doc");

    if (!onlyFulltextSearchIndex)
        fileList << QLatin1String("indexdb40") << QLatin1String("contentdb40");

    QStringList::iterator it = fileList.begin();
    for (; it != fileList.end(); ++it) {
        if (QFile::exists(Config::configuration()->CacheDir() + QDir::separator() + *it + pname)) {
            QFile f(Config::configuration()->CacheDir() + QDir::separator() + *it + pname);
            f.remove();
        }
    }
}

//-------------------------------------------------
// ?-? do we need it
void HelpDialog::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
    static int opacity = 255;
    mw->setWindowOpacity((opacity-=4)/255.0);
    if (opacity<=0)
        qApp->quit();
}

//-------------------------------------------------
void HelpDialog::loadIndexFile()
{
    if (indexDone)
        return;

    setCursor(Qt::WaitCursor);
    indexDone = true;
    ui.labelPrepare->setText(tr("Prepare..."));
    ui.framePrepare->show();
    processEvents();

    QProgressBar *bar = ui.progressPrepare;
    bar->setMaximum(100);
    bar->setValue(0);

    keywordDocuments.clear();
    QList<IndexKeyword> lst;
    QString iFileName = Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("indexdb40.") +
            Config::configuration()->profileName();
    QFile indexFile(iFileName);
    if (!indexFile.open(QFile::ReadOnly)) {
        buildKeywordDB();
        processEvents();
        if (lwClosed)
            return;
        if (!indexFile.open(QFile::ReadOnly)) {
            QMessageBox::warning(mw, tr("%1").arg(GL_Prog_Name), tr("Failed to load keyword index file from:\n%1").arg(iFileName));

#if defined Q_WS_WIN || defined Q_WS_MACX
            startTimer(50);
#endif
            return;
        }
    }

    QDataStream ds(&indexFile);
    quint32 fileAges;
    ds >> fileAges;
    if (fileAges != getFileAges()) {
        indexFile.close();
        buildKeywordDB();
        if (!indexFile.open(QFile::ReadOnly)) {
            QMessageBox::warning(mw, tr("%1").arg(GL_Prog_Name), tr("Cannot open the index file %1").arg(iFileName));
            return;
        }
        ds.setDevice(&indexFile);
        ds >> fileAges;
    }
    ds >> lst;
    indexFile.close();

    bar->setValue(bar->maximum());
    processEvents();

    for (int i=0; i<lst.count(); ++i) {
        const IndexKeyword &idx = lst.at(i);
        indexModel->addLink(idx.keyword, idx.link);
        keywordDocuments << removeAnchorFromLink(idx.link);
    }

    indexModel->publish();

    ui.framePrepare->hide();
    showInitDoneMessage();
    setCursor(Qt::ArrowCursor);
}

//-------------------------------------------------
quint32 HelpDialog::getFileAges()
{
    QStringList addDocuFiles = Config::configuration()->docFiles();
    QStringList::const_iterator i = addDocuFiles.constBegin();

    quint32 fileAges = 0;
    for (; i != addDocuFiles.constEnd(); ++i) {
        QFileInfo fi(*i);
        qDebug() << "getFileAges: " << *i;
        if (fi.exists())
            fileAges += fi.lastModified().toTime_t();
    }

    return fileAges;
}

//-------------------------------------------------
void HelpDialog::buildKeywordDB()
{
    QStringList addDocuFiles = Config::configuration()->docFiles();
    QStringList::iterator i = addDocuFiles.begin();

    // Set up an indeterminate progress bar.
    ui.labelPrepare->setText(tr("Prepare..."));
    ui.progressPrepare->setMaximum(0);
    ui.progressPrepare->setMinimum(0);
    ui.progressPrepare->setValue(0);
    processEvents();

    QList<IndexKeyword> lst;
    quint32 fileAges = 0;
    for (i = addDocuFiles.begin(); i != addDocuFiles.end(); ++i) {
        QFile file(*i);
        qDebug() << "opening file buildKeywordDB= " << *i;

        if (!file.exists()) {
            QMessageBox::warning(this, tr("Warning"), tr("Documentation file %1 does not exist!\n"
                                                         "Skipping file.").arg(QFileInfo(file).absoluteFilePath()));
            continue;
        }
        fileAges += QFileInfo(file).lastModified().toTime_t();
        DocuParser *handler = DocuParser::createParser(*i);
        bool ok = handler->parse(&file);
        file.close();
        if (!ok){
            QString msg = QString::fromLatin1("In file %1:\n%2")
                    .arg(QFileInfo(file).absoluteFilePath())
                    .arg(handler->errorProtocol());
            QMessageBox::critical(this, tr("Parse Error"), tr(msg.toUtf8()));
            delete handler;
            continue;
        }

        QList<IndexItem*> indLst = handler->getIndexItems();
        int counter = 0;
        foreach (IndexItem *indItem, indLst) {
            QFileInfo fi(indItem->reference);
            lst.append(IndexKeyword(indItem->keyword, indItem->reference));

            if (++counter%100 == 0) {
                if (ui.progressPrepare)
                    ui.progressPrepare->setValue(counter);
                processEvents();
                if (lwClosed) {
                    return;
                }
            }
        }
        delete handler;
    }
    if (!lst.isEmpty())
        qSort(lst);

    QFile indexout(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("indexdb40.")
                   + Config::configuration()->profileName());
    if (verifyDirectory(Config::configuration()->CacheDir()) && indexout.open(QFile::WriteOnly)) {
        QDataStream s(&indexout);
        s << fileAges;
        s << lst;
        indexout.close();
    }
}

//-------------------------------------------------
void HelpDialog::setupTitleMap() //?-? check this procedure, may be remove some code
{
    if (titleMapDone)
        return;

    bool needRebuild = false;
    if (Config::configuration()->profileName() == QLatin1String("default")) {
        const QStringList docuFiles = Config::configuration()->docFiles();
        for (QStringList::ConstIterator it = docuFiles.begin(); it != docuFiles.end(); ++it) {
            qDebug() << "setupTitleMap: " << *it;
            if (!QFile::exists(*it)) {
                qDebug() << "need to create profile";
                /*Config::configuration()->saveProfile(Profile::create());
                Config::configuration()->loadProject();*/
                needRebuild = true;
                break;
            }
        }
    }

    if (Config::configuration()->docRebuild() || needRebuild) {
        removeOldCacheFiles();
        Config::configuration()->setDocRebuild(false);
        //Config::configuration()->saveProfile(Config::configuration()->curProfile);
    }
    if (contentList.isEmpty())
        getAllContents();

    titleMapDone = true;
    titleMap.clear();
    foreach (ContentItem item, contentList) {
        titleMap[item.reference] = item.title.trimmed();
    }
    /*
    for (QList<ContentItem>::Iterator it = contentList.begin(); it != contentList.end(); ++it) {
        //ContentList lst = (*it).second;
        ContentList lst = *it;
        foreach (ContentItem item, lst) {
            titleMap[item.reference] = item.title.trimmed();
        }
    }*/
    processEvents();
}

//-------------------------------------------------
void HelpDialog::getAllContents()
{
    QFile contentFile(Config::configuration()->profileName());
    contentList.clear();
    if (!contentFile.open(QFile::ReadOnly)) {
        buildContentDict();
        return;
    }

    QDataStream ds(&contentFile);
    quint32 fileAges;
    ds >> fileAges;
    if (fileAges != getFileAges()) {
        contentFile.close();
        removeOldCacheFiles(true);
        buildContentDict();
        return;
    }
    QString key;
    QList<ContentItem> lst;
    while (!ds.atEnd()) {
        ds >> key;
        ds >> lst;
        //contentList += qMakePair(key, QList<ContentItem>(lst));
        contentList += lst;
    }
    contentFile.close();
    processEvents();
}

//-------------------------------------------------
void HelpDialog::buildContentDict() //fill up contents = create TreeWidget nodes (TreeWidgetItems)
{
    QStringList docuFiles = Config::configuration()->docFiles();

    quint32 fileAges = 0;
    for (QStringList::iterator it = docuFiles.begin(); it != docuFiles.end(); ++it) {
        QFile file(*it);
        //        qDebug() << "buildContentDict: " << *it;   // ?-? it is always only one iteration: current project *.pp file. Should we remove 'for' cycle and docuFiles

        if (!file.exists()) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Project file %1 does not exist!\nSkipping file.").arg(QFileInfo(file).absoluteFilePath()));
            continue;
        }
        fileAges += QFileInfo(file).lastModified().toTime_t();
        DocuParser *handler = DocuParser::createParser(*it);
        if (!handler) {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("Project file %1 is not compatible!\nSkipping file.").arg(QFileInfo(file).absoluteFilePath()));
            continue;
        }
        bool ok = handler->parse(&file);
        file.close();
        if (ok) {
            contentList += handler->getContentItems();
            delete handler;
        } else {
            QString msg = QString::fromLatin1("In file %1:\n%2")
                    .arg(QFileInfo(file).absoluteFilePath())
                    .arg(handler->errorProtocol());
            QMessageBox::critical(this, tr("Parse Error"), tr(msg.toUtf8()));
            continue;
        }
    }

    QFile contentOut(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("contentdb40.") + Config::configuration()->profileName());
    if (contentOut.open(QFile::WriteOnly)) {
        QDataStream s(&contentOut);
        s << fileAges;
        for (QList<ContentItem>::Iterator it = contentList.begin(); it != contentList.end(); ++it) {
            s << *it;
        }
        contentOut.close();
    }
}

//-------------------------------------------------
void HelpDialog::currentTabChanged(int index)
{
    QString s = ui.tabWidget->widget(index)->objectName();
    if (s == QLatin1String("indexPage"))
        QTimer::singleShot(0, this, SLOT(loadIndexFile()));
    else if (s == QLatin1String("bookmarkPage"))
        insertBookmarks();
    else if (s == QLatin1String("contentPage"))
        QTimer::singleShot(0, this, SLOT(insertContents()));
    else if (s == QLatin1String("searchPage"))
        QTimer::singleShot(0, this, SLOT(setupFullTextIndex()));
}

//-------------------------------------------------
void HelpDialog::showInitDoneMessage()
{
    if (initDoneMsgShown)
        return;
    initDoneMsgShown = true;
    mw->statusBar()->showMessage(tr("Done"), 3000);
}

//-------------------------------------------------
void HelpDialog::showTopic(QTreeWidgetItem *item)
{
    if (item)
        showTopic();
}

//-------------------------------------------------
void HelpDialog::showTopic()
{
    QString tabName = ui.tabWidget->currentWidget()->objectName();
    if (tabName == QLatin1String("indexPage"))
        showIndexTopic();
    else if (tabName == QLatin1String("bookmarkPage"))
        showBookmarkTopic();
    else if (tabName == QLatin1String("contentPage")){
        showContentsTopic();
        ui.listContents->setFocus();
    }

    // if there is no title for the document set one to title of the item
    QString t = mw->browsers()->currentBrowser()->getTagTitle();
    if (t.isEmpty()) {
        if (ContCur == ContTreeView)
            t = ui.listContents->currentItem()->text(0);
        else if (ContCur == ContSubItems)
            t = ui.TWSubItems->currentItem()->text(0);
        mw->browsers()->currentBrowser()->setTagTitle(t);
        mw->browsers()->sourceChanged();
    }

    /*	-pm- the following is for debug
 // display item index
 QTreeWidgetItem *parent = ui.listContents->currentItem()->parent();
    int index;
    if (parent) {
        index = parent->indexOfChild(ui.listContents->currentItem());
    } else {
        index = ui.listContents->indexOfTopLevelItem(ui.listContents->currentItem());
    }
 QString ss = ui.listContents->currentItem()->data(0,LinkRole).toString();
 //t = mw->browsers()->currentBrowser()->getTagTitle();
 //mw->statusBar()->showMessage(tr("item N=%1; file=%2; title=%3").arg(index).arg(ss).arg(t), 5000);
*/
}

//-------------------------------------------------
void HelpDialog::showIndexTopic()
{
    int row = ui.listIndex->currentIndex().row();
    if (row == -1 || row >= indexModel->rowCount())
        return;

    QString description = indexModel->description(row);
    QStringList links = indexModel->links(row);

    bool blocked = ui.editIndex->blockSignals(true);
    ui.editIndex->setText(description);
    ui.editIndex->blockSignals(blocked);

    if (links.count() == 1) {
        emit showLink(links.first());
    } else {
        qSort(links);
        QStringList::Iterator it = links.begin();
        QStringList linkList;
        QStringList linkNames;
        for (; it != links.end(); ++it) {
            linkList << *it;
            linkNames << titleOfLink(*it);
        }
        QString link = TopicChooser::getLink(this, linkNames, linkList, description);
        if (!link.isEmpty())
            emit showLink(link);
    }

    ui.listIndex->setCurrentIndex(indexModel->index(indexModel->stringList().indexOf(description)));
    ui.listIndex->scrollTo(ui.listIndex->currentIndex(), QAbstractItemView::PositionAtTop);
}

//-------------------------------------------------
void HelpDialog::searchInIndex(const QString &searchString)
{
    QRegExp atoz(QLatin1String("[A-Z]"));
    int matches = searchString.count(atoz);
    if (matches > 0 && !searchString.contains(QLatin1String(".*")))
    {
        int start = 0;
        QString newSearch;
        for (; matches > 0; --matches) {
            int match = searchString.indexOf(atoz, start+1);
            if (match <= start)
                continue;
            newSearch += searchString.mid(start, match-start);
            newSearch += QLatin1String(".*");
            start = match;
        }
        newSearch += searchString.mid(start);
        ui.listIndex->setCurrentIndex(indexModel->filter(newSearch, searchString));
    }
    else
        ui.listIndex->setCurrentIndex(indexModel->filter(searchString, searchString));
}

//-------------------------------------------------
QString HelpDialog::titleOfLink(const QString &link)
{
    QString s = removeAnchorFromLink(link);
    s = titleMap[s];
    if (s.isEmpty())
        return link;
    return s;
}

//-------------------------------------------------
bool HelpDialog::eventFilter(QObject * o, QEvent * e)
{
    QString objName = o->objectName();
    int key = static_cast<QKeyEvent*>(e)->key();
    quint32 modifiers = QApplication::keyboardModifiers();
    if (o == ui.editIndex && e->type() == QEvent::KeyPress) {
        switch (static_cast<QKeyEvent*>(e)->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        case Qt::Key_PageUp:
            QApplication::sendEvent(ui.listIndex, e);
            break;
        default: break;
        } 
    } else if (objName == "qt_scrollarea_viewport") { //on mouse events, "qt_scrollarea_viewport" is an event sender for ui.listContents and ui.TWSubItems viewports
        if (e->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *me = static_cast<QMouseEvent*>(e);
            if (ContCur == ContTreeView) { //mouse released in ui.listContents
            	if (me->button() == Qt::LeftButton) {
            	    QTreeWidgetItem *item = ui.listContents->itemAt(me->pos());
            	    QRect vRect = ui.listContents->visualItemRect(item);            	    
            	    if (item && vRect.contains(me->pos())){		// show topic only if we clicked an item
            	        showTopic(item);
            	        if (Config::configuration()->ContentsAdditionalView()) 
                            fillSubList();
                        if (Config::configuration()->AutoCollapse()){
                            ui.listContents->collapseAll();
                        }
            	    }
            	}
            } else if (ContCur == ContSubItems) {  //mouse released in ui.TWSubItems  		
                if (me->button() == Qt::LeftButton) {
                    QTreeWidgetItem *item = ui.TWSubItems->itemAt(me->pos());
                    QRect vRect = ui.TWSubItems->visualItemRect(item);
                    if (item && vRect.contains(me->pos())){
                        showTopic(item);
                    }
            	}        		 	
            }
        }   	
    } else if (o == ui.listContents){ //on keyboard and widget events	
    	if (objName == "listContents"){  
            ContCur = ContTreeView;
      	}else if (objName == "TWSubItems"){ //!+! check this. It should be not in "if (o == ui.listContents)" section
            ContCur = ContSubItems;
        }

        if (e->type() == QEvent::KeyPress){	// can not use just shortcuts, see HelpDialog::initialize()
            switch (key) {
            case Qt::Key_Insert:
                //newSameLevelItem = false;
                if (modifiers == Qt::ShiftModifier){
                    triggerAction(ui.listContents->currentItem(), actionItemAddFile);
                    //mw->browsers()->currentBrowser()->fileOpen();
                }else if (modifiers == Qt::NoModifier){
                    triggerAction(ui.listContents->currentItem(), actionItemInsert);
                    //newItem();
                }
                break;
            case Qt::Key_Delete:
                if (modifiers == Qt::ShiftModifier){
                    deleteItem();
                }else if (modifiers == Qt::NoModifier){
                    removeItem();
                }
                break;
            case Qt::Key_F2:
                ui.listContents->openPersistentEditor(ui.listContents->currentItem(),0);
                break;
            case Qt::Key_Enter: case Qt::Key_Return:
                if (modifiers == Qt::ControlModifier){
                    showItemProperties();
                }
                break;
            case Qt::Key_1:
                if (modifiers == Qt::ControlModifier){
                    //triggerAction(ui.listContents->currentItem(), actionItemInsertSameLevel);
                    triggerAction(ui.listContents->currentItem(), actionItemBookAdd);
                }
                break;
            case Qt::Key_2:
                if (modifiers == Qt::ControlModifier){
                    //triggerAction(ui.listContents->currentItem(), actionItemAddFileSameLevel);
                    triggerAction(ui.listContents->currentItem(), actionItemChapterAdd);
                }
                break;
            case Qt::Key_3:
                if (modifiers == Qt::ControlModifier){
                    //triggerAction(ui.listContents->currentItem(), actionItemAddFileSameLevel);
                    triggerAction(ui.listContents->currentItem(), actionItemVerseAdd);
                }
                break;

            case Qt::Key_A:
                if (modifiers == Qt::ControlModifier){
                    CreateAutoItems();
                }
                break;
            case Qt::Key_S:
                if (modifiers == Qt::ControlModifier){
                    saveProject();
                }
                break;
            default: break;
            }
        }
    }
    return QWidget::eventFilter(o, e);
}

//-------------------------------------------------
void HelpDialog::addBookmark()
{
    if (!bookmarksInserted)
        insertBookmarks();
    QString link = relatifyFileName(mw->browsers()->currentBrowser()->source().toString(), Config::configuration()->CacheDir());
    QString title = mw->browsers()->currentBrowser()->documentTitle();
    mw->statusBar()->showMessage(tr("bookmark title=%1; link=%2").arg(title).arg(link), 5000);
    if (title.isEmpty())
        title = titleOfLink(link);

    QTreeWidgetItem *i = new QTreeWidgetItem(ui.listBookmarks, 0);
    i->setText(0, title);
    i->setData(0, LinkRole, link);
    ui.buttonRemove->setEnabled(true);
    saveBookmarks();

    mw->projectModified(true);
}

//-------------------------------------------------
void HelpDialog::on_buttonAdd_clicked()
{
    addBookmark();
}

//-------------------------------------------------
void HelpDialog::on_buttonRemove_clicked()
{
    if (!ui.listBookmarks->currentItem())
        return;

    delete ui.listBookmarks->currentItem();
    saveBookmarks();
    if (ui.listBookmarks->topLevelItemCount() != 0) {
        ui.listBookmarks->setCurrentItem(ui.listBookmarks->topLevelItem(0));
    }
    ui.buttonRemove->setEnabled(ui.listBookmarks->topLevelItemCount() > 0);

    mw->projectModified(true);
}

//-------------------------------------------------
void HelpDialog::insertBookmarks() //to list in Bookmarks tab. Add to Bookmark menu in  MainWindow::setupBookmarkMenu()
{
    if (bookmarksInserted)
        return;
    bookmarksInserted = true;

    ui.listBookmarks->clear();
    QFile f(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("bookmarks.") + Config::configuration()->profileName());
    if (!f.open(QFile::ReadOnly))
        return;
    QTextStream ts(&f);
    while (!ts.atEnd()) {
        QTreeWidgetItem *i = new QTreeWidgetItem(ui.listBookmarks, 0);
        i->setText(0, ts.readLine());
        i->setData(0, LinkRole, ts.readLine());
    }
    ui.buttonRemove->setEnabled(ui.listBookmarks->topLevelItemCount() > 0);

    showInitDoneMessage();
    mw->projectModified(true);
}

//-------------------------------------------------
void HelpDialog::showBookmarkTopic()
{
    if (!ui.listBookmarks->currentItem())
        return;

    QTreeWidgetItem *i = (QTreeWidgetItem*)ui.listBookmarks->currentItem();
    //turn relative path to format file://<absolute path>/filename.html
    //QDir dir(Config::configuration()->cacheDir);
    //QString lnk = dir.absoluteFilePath( i->data(0, LinkRole).toString() );
    //lnk = urlifyFileName( dir.cleanPath(lnk) );
    QString lnk = urlifyFileName( absolutifyFileName( i->data(0, LinkRole).toString(), Config::configuration()->CacheDir() ) );
    emit showLink(lnk);
}

//-------------------------------------------------
//for saving bookmarks to file. Used by HelpDialog::saveBookmarks()
void HelpDialog::store(QTreeWidgetItem *i, QTextStream &ts)
{
    //save relative path to open bookmark in any OS
    QString rfn = relatifyFileName(i->data(0, LinkRole).toString(), Config::configuration()->CacheDir());

    ts << i->text(0) << endl;
    ts << rfn << endl;

    for (int index = 0; index < i->childCount(); ++index)
        store(i->child(index), ts);
}

//-------------------------------------------------
//for saving bookmarks to file. Used by HelpDialog::saveBookmarks()
void HelpDialog::store(QTreeWidget *tw, QTextStream &ts)
{
    for (int index = 0; index < tw->topLevelItemCount(); ++index)
        store(tw->topLevelItem(index), ts);
}

//-------------------------------------------------
void HelpDialog::saveBookmarks()
{
    QFile f(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("bookmarks.") + Config::configuration()->profileName());
    if (!f.open(QFile::WriteOnly))
        return;

    QTextStream ts(&f);
    store(ui.listBookmarks, ts);
    f.close();
}

//-------------------------------------------------
void HelpDialog::insertContents()
{
    QTreeWidgetItem *rootEntry;
    QTreeWidgetItem *childEntry;
    ContentItem item;
    QStack<QTreeWidgetItem*> stack;
    int depth = 0;
    QTreeWidgetItem *lastItem[64];    // ? Why 64? Maximum depth of tree?
    
    if (contentsInserted)
        return;
    if (contentList.isEmpty())
        getAllContents();
    contentsInserted = true;
    if (!titleMapDone)
        setupTitleMap();
    ui.listContents->clear();
    stack.clear();
    setCursor(Qt::WaitCursor);
    for (int j = 0; j < 64; ++j)
        lastItem[j] = 0;

    for (QList<ContentItem>::Iterator it = contentList.begin(); it != contentList.end(); ++it) {
        item = *it;
        if (item.depth == 0) {
            rootEntry = new QTreeWidgetItem(ui.listContents, 0);
            rootEntry->setText(0, item.title);
            rootEntry->setData(0, LinkRole, item.reference);
            rootEntry->setData(0, IconFNRole, item.iconFN);
            if (!item.iconFN.isEmpty() )
            	rootEntry->setIcon(0, QIcon(item.iconFN));
            stack.push(rootEntry);
            depth = 1;
        }else{
            if (item.depth > depth)  {
                depth = item.depth;
                stack.push(childEntry);
            }
            if (item.depth == depth) {
                childEntry = new QTreeWidgetItem(stack.top(), lastItem[ depth ]);
                lastItem[ depth ] = childEntry;
                childEntry->setText(0, item.title);
                childEntry->setData(0, LinkRole, item.reference);
                childEntry->setData(0, IconFNRole, item.iconFN);
            	if (!item.iconFN.isEmpty() )
                    childEntry->setIcon(0, QIcon(item.iconFN));
            }
            else if (item.depth < depth) {
                stack.pop();
                depth--;
                item = *(--it);
            }
        }
        //processEvents(); // ? why do we need it ?
    }
    setCursor(Qt::ArrowCursor);
    showInitDoneMessage();
}

//-------------------------------------------------
void HelpDialog::showContentsTopic() //show topic on click in contens
{
    QTreeWidgetItem *i;
    i = (QTreeWidgetItem*)ui.listContents->currentItem(); //ContCur == ContTreeView
    if (ContCur == ContSubItems){
    	i = (QTreeWidgetItem*)ui.TWSubItems->currentItem();
    }
    if (!i)
        return;
    QString fn = unurlifyFileName(i->data(0, LinkRole).toString());
    QFileInfo fi(fn);
    if (fi.exists() && fi.isFile()){
        qDebug() << "-- opening file: " << fn << ", link = " << i->data(0, LinkRole).toString();
        emit showLink(i->data(0, LinkRole).toString());
    }else{ 
        int ret = QMessageBox::warning(mw,  tr("%1").arg(GL_Prog_Name), tr("Source file could not be found:\n %1\n"
                                                                           "You can create a new file or add one. Do you want to see item property?").arg(fn),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::Yes)
            showItemProperties();
    }
}

//-------------------------------------------------
QTreeWidgetItem * HelpDialog::locateLink(QTreeWidgetItem *item, const QString &link)
{
    QTreeWidgetItem *child = 0;
#ifdef Q_OS_WIN
    Qt::CaseSensitivity checkCase = Qt::CaseInsensitive;
#else
    Qt::CaseSensitivity checkCase = Qt::CaseSensitive;
#endif
    for (int i = 0, childCount = item->childCount(); i<childCount; i++) {
        child = item->child(i);
        //check whether it is this item
        if (link.startsWith(child->data(0, LinkRole).toString(), checkCase))
            break;
        //check if the link is a child of this item
        else if (child->childCount()) {
            child = locateLink(child, link);
            if (child)
                break;
        }
        child = 0;
    }
    return child;
}

//-------------------------------------------------
void HelpDialog::locateContents(const QString &link)
{
    //ensure the TOC is filled
    if (!contentsInserted)
        insertContents();
#ifdef Q_OS_WIN
    Qt::CaseSensitivity checkCase = Qt::CaseInsensitive;
#else
    Qt::CaseSensitivity checkCase = Qt::CaseSensitive;
#endif
    QString findLink(link);
    //Installations on a windows local drive will give the 'link' as <file:///C:/xxx>
    //and the contents in the TOC will be <file:C:/xxx>.
    //But on others the 'link' of format <file:///root/xxx>
    //and the contents in the TOC will be <file:/root/xxx>.
    if (findLink.contains(QLatin1String("file:///"))) {
        if (findLink[9] == QLatin1Char(':')) //on windows drives
            findLink.replace(0, 8, QLatin1String("file:"));
        else
            findLink.replace(0, 8, QLatin1String("file:/"));
    }

    bool topLevel = false;
    QTreeWidgetItem *item = 0;
    int totalItems = ui.listContents->topLevelItemCount();

    for (int i = 0; i < totalItems; i++ ) {
        // first see if we are one of the top level items
        item = (QTreeWidgetItem*)ui.listContents->topLevelItem(i);
        if (findLink.startsWith(item->data(0, LinkRole).toString(), checkCase)) {
            topLevel = true;
            break;
        }
    }

    if (!topLevel) {
        // now try to find it in the sublevel items
        for (int n = 0; n < totalItems; ++n) {
            item = (QTreeWidgetItem*)ui.listContents->topLevelItem(n);
            item = locateLink(item, findLink);
            if (item)
                break;
        }
    }

    //remove the old selection
    QList<QTreeWidgetItem *> selected = ui.listContents->selectedItems();
    foreach(QTreeWidgetItem *sel, selected)
        ui.listContents->setItemSelected(sel, false);

    //set the TOC item and show
    ui.listContents->setCurrentItem(item);
    ui.listContents->setItemSelected(item, true);
    ui.listContents->scrollToItem(item);
}

//-------------------------------------------------
void HelpDialog::toggleContents()
{
    if (!isVisible() || ui.tabWidget->currentIndex() != 0) {
        ui.tabWidget->setCurrentIndex(0);
        parentWidget()->show();
    }
    else
        parentWidget()->hide();
}

//-------------------------------------------------
void HelpDialog::toggleIndex()
{
    if (!isVisible() || ui.tabWidget->currentIndex() != 1 || !ui.editIndex->hasFocus()) {
        ui.tabWidget->setCurrentIndex(1);
        parentWidget()->show();
        ui.editIndex->setFocus();
    }
    else
        parentWidget()->hide();
}

//-------------------------------------------------
void HelpDialog::toggleBookmarks()
{
    if (!isVisible() || ui.tabWidget->currentIndex() != 2) {
        ui.tabWidget->setCurrentIndex(2);
        parentWidget()->show();
    }
    else
        parentWidget()->hide();
}

//-------------------------------------------------
void HelpDialog::toggleSearch()
{
    if (!isVisible() || ui.tabWidget->currentIndex() != 3) {
        ui.tabWidget->setCurrentIndex(3);
        parentWidget()->show();
    }
    else
        parentWidget()->hide();
}

//-------------------------------------------------
void HelpDialog::setupFullTextIndex()
{
    if (fullTextIndex)
        return;

    QString pname = Config::configuration()->profileName();
    //fullTextIndex = new Index(QStringList(), QDir::homePath()); // ### Is this correct ?
    fullTextIndex = new Index(QStringList(), Config::configuration()->CacheDir());
    if (!verifyDirectory(Config::configuration()->CacheDir())) {
        QMessageBox::warning(mw, tr("%1").arg(GL_Prog_Name),
                             tr("Failed to save fulltext search index to path: %1").arg(Config::configuration()->CacheDir()));
        return;
    }
    fullTextIndex->setDictionaryFile(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("indexdb40.dict.") + pname);
    fullTextIndex->setDocListFile(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("indexdb40.doc.") + pname);
    processEvents();

    connect(fullTextIndex, SIGNAL(indexingProgress(int)), this, SLOT(setIndexingProgress(int)));
    QFile f(Config::configuration()->CacheDir() + QDir::separator() + QLatin1String("indexdb40.dict.") + pname);
    if (!f.exists()) {
        QString doc;
        QSet<QString> documentSet;
        QMap<QString, QString>::ConstIterator it = titleMap.constBegin();
        for (; it != titleMap.constEnd(); ++it) {
            doc = removeAnchorFromLink(it.key());
            if (!doc.isEmpty())
                documentSet.insert(doc);
        }
        loadIndexFile();
        for ( QStringList::Iterator it = keywordDocuments.begin(); it != keywordDocuments.end(); ++it ) {
            if (!(*it).isEmpty())
                documentSet.insert(*it);
        }
        fullTextIndex->setDocList( documentSet.toList() );

        mw->statusBar()->clearMessage();
        setCursor(Qt::WaitCursor);
        ui.labelPrepare->setText(tr("Indexing files..."));
        ui.progressPrepare->setMaximum(100);
        ui.progressPrepare->reset();
        ui.progressPrepare->show();
        ui.framePrepare->show();
        processEvents();
        if (fullTextIndex->makeIndex() == -1)
            return;
        fullTextIndex->writeDict();
        ui.progressPrepare->setValue(100);
        ui.framePrepare->hide();
        setCursor(Qt::ArrowCursor);
        showInitDoneMessage();
    } else {
        setCursor(Qt::WaitCursor);
        mw->statusBar()->showMessage(tr("Reading dictionary..."));
        processEvents();
        fullTextIndex->readDict();
        mw->statusBar()->showMessage(tr("Done"), 3000);
        setCursor(Qt::ArrowCursor);
    }
    keywordDocuments.clear();
}

//-------------------------------------------------
void HelpDialog::setIndexingProgress(int prog)
{
    ui.progressPrepare->setValue(prog);
    processEvents();
}

//-------------------------------------------------
void HelpDialog::startSearch()
{
    QString str = ui.termsEdit->text();
    str = str.simplified();
    str = str.replace(QLatin1String("\'"), QLatin1String("\""));
    str = str.replace(QLatin1String("`"), QLatin1String("\""));
    QString buf = str;
    str = str.replace(QLatin1String("-"), QLatin1String(" "));
    str = str.replace(QRegExp(QLatin1String("\\s[\\S]?\\s")), QLatin1String(" "));
    terms = str.split(QLatin1Char(' '));
    QStringList termSeq;
    QStringList seqWords;
    QStringList::iterator it = terms.begin();
    for (; it != terms.end(); ++it) {
        (*it) = (*it).simplified();
        (*it) = (*it).toLower();
        (*it) = (*it).replace(QLatin1String("\""), QLatin1String(""));
    }
    if (str.contains(QLatin1Char('\"'))) {
        if ((str.count(QLatin1Char('\"')))%2 == 0) {
            int beg = 0;
            int end = 0;
            QString s;
            beg = str.indexOf(QLatin1Char('\"'), beg);
            while (beg != -1) {
                beg++;
                end = str.indexOf(QLatin1Char('\"'), beg);
                s = str.mid(beg, end - beg);
                s = s.toLower();
                s = s.simplified();
                if (s.contains(QLatin1Char('*'))) {
                    QMessageBox::warning(this, tr("Full Text Search"),
                                         tr("Using a wildcard within phrases is not allowed."));
                    return;
                }
                seqWords += s.split(QLatin1Char(' '));
                termSeq << s;
                beg = str.indexOf(QLatin1Char('\"'), end + 1);
            }
        } else {
            QMessageBox::warning(this, tr("Full Text Search"), tr("The closing quotation mark is missing."));
            return;
        }
    }
    setCursor(Qt::WaitCursor);
    foundDocs.clear();
    foundDocs = fullTextIndex->query(terms, termSeq, seqWords);
    QString msg = QString::fromLatin1("%1 documents found.").arg(foundDocs.count());
    mw->statusBar()->showMessage(tr(msg.toUtf8()), 3000);
    ui.resultBox->clear();
    for (it = foundDocs.begin(); it != foundDocs.end(); ++it)
        ui.resultBox->addItem(fullTextIndex->getDocumentTitle(*it));

    terms.clear();
    bool isPhrase = false;
    QString s;
    for (int i = 0; i < (int)buf.length(); ++i) {
        if (buf[i] == QLatin1Char('\"')) {
            isPhrase = !isPhrase;
            s = s.simplified();
            if (!s.isEmpty())
                terms << s;
            s = QLatin1String("");
        } else if (buf[i] == QLatin1Char(' ') && !isPhrase) {
            s = s.simplified();
            if (!s.isEmpty())
                terms << s;
            s = QLatin1String("");
        } else
            s += buf[i];
    }
    if (!s.isEmpty())
        terms << s;

    setCursor(Qt::ArrowCursor);
}
/*
//-------------------------------------------------
void HelpDialog::on_helpButton_clicked()
{
    emit showLink(MainWindow::urlifyFileName(
                  Config::configuration()->assistantDocPath() +
                  QLatin1String("/assistant-manual.html#full-text-searching")));
}
*/

//-------------------------------------------------
void HelpDialog::on_resultBox_itemActivated(QListWidgetItem *item)
{
    showResultPage(item);
}

//-------------------------------------------------
void HelpDialog::showResultPage(QListWidgetItem *item)
{
    if (item)
        emit showSearchLink(foundDocs[ui.resultBox->row(item)], terms);
}

//-------------------------------------------------
void HelpDialog::showIndexItemMenu(const QPoint &pos) //on click in index (keyword) list
{
    QListView *listView = qobject_cast<QListView*>(sender());
    if (!listView)
        return;

    QModelIndex idx = listView->indexAt(pos);
    if (!idx.isValid())
        return;

    QAction *action = itemPopup->exec(listView->viewport()->mapToGlobal(pos));
    if (action == actionOpenCurrentTab) {
        showTopic();
    } else if (action) {
        HelpWindow *hw = mw->browsers()->currentBrowser();
        QString itemName = idx.data().toString();
        ui.editIndex->setText(itemName);
        QStringList links = indexModel->links(idx.row());
        if (links.count() == 1) {
            if (action == actionOpenLinkInNewWindow)
                hw->openLinkInNewWindow(links.first());
            else
                hw->openLinkInNewPage(links.first());
        } else {
            QStringList::Iterator it = links.begin();
            QStringList linkList;
            QStringList linkNames;
            for (; it != links.end(); ++it) {
                linkList << *it;
                linkNames << titleOfLink(*it);
            }
            QString link = TopicChooser::getLink(this, linkNames, linkList, itemName);
            if (!link.isEmpty()) {
                if (action == actionOpenLinkInNewWindow)
                    hw->openLinkInNewWindow(link);
                else if (action == actionOpenLinkInExtEditor) 
                    mw->OpenInExternalApplication(Config::configuration()->ExternalEditor(), link);
                else if (action == actionOpenLinkInExtBrowser)
                    mw->OpenInExternalApplication(Config::configuration()->ExternalBrowser(), link);
                else
                    hw->openLinkInNewPage(link);
            }
        }
    }
}

//-------------------------------------------------
void HelpDialog::showListItemMenu(const QPoint &pos) // show doc from search result list
{
    QListWidget *listWidget = qobject_cast<QListWidget*>(sender());
    if (!listWidget)
        return;
    QListWidgetItem *item = listWidget->itemAt(pos);
    if (!item)
        return;

    QAction *action = itemPopup->exec(listWidget->viewport()->mapToGlobal(pos));
    if (action == actionOpenCurrentTab) {
        showResultPage(item);
    } else if (action) {
        HelpWindow *hw = mw->browsers()->currentBrowser();
        QString link = foundDocs[ui.resultBox->row(item)];
        if (action == actionOpenLinkInNewWindow)
            hw->openLinkInNewWindow(link);
        else if (action == actionOpenLinkInExtEditor)
            mw->OpenInExternalApplication(Config::configuration()->ExternalEditor(), link);
        else if (action == actionOpenLinkInExtBrowser)
            mw->OpenInExternalApplication(Config::configuration()->ExternalBrowser(), link);
        else
            hw->openLinkInNewPage(link);
    }
}

//-------------------------------------------------
void HelpDialog::showContentsItemMenu(const QPoint &pos)
{
    QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(sender());
    if (!treeWidget)
        return;

    QTreeWidgetItem *item = treeWidget->itemAt(pos);
    if (!item)
        return;

    QAction *action;
    action = itemPopupContents->exec(treeWidget->viewport()->mapToGlobal(pos)); //ContCur == ContTreeView
    if (ContCur == ContSubItems) 
    	action = itemPopupSubItems->exec(treeWidget->viewport()->mapToGlobal(pos));
    triggerAction(item, action);
}

//-------------------------------------------------
void HelpDialog::triggerAction(QTreeWidgetItem *item, QAction *action)
{
    if (action) {

        if (action == actionItemProperties){
            showItemProperties();
        }else if (action == actionItemInsert){
            newSameLevelItem = false;
            newItem();
       	}else if (action == actionItemAddFile){
            newSameLevelItem = false;
            mw->browsers()->currentBrowser()->fileOpen();
        }else if (action == actionItemInsertSameLevel){
            newSameLevelItem = true;
            newItem();


            //---------------

        }else if (action == actionItemBookAdd){
            newSameLevelItem = true;  //add book
            //m_bookadddialog = new BookAddDialog;
            m_bookadddialog->show(); // вывод диалога с вводом данных о книге



            // newItem();

        }else if (action == actionItemChapterAdd){
            //newSameLevelItem = true;  //add chapter
            newSameLevelItem = false;
            newItem();


        }else if (action == actionItemVerseAdd){
            //newSameLevelItem = true;
            newSameLevelItem = false;
            newItem();  //add verse

            //------

       	}else if (action == actionItemAddFileSameLevel){
            newSameLevelItem = true;
            mw->browsers()->currentBrowser()->fileOpen();
       	}else if (action == actionItemRemove){
            removeItem();
       	}else if (action == actionItemDelete){
            deleteItem();
       	}else if (action == actionItemAuto){
            CreateAutoItems();
        }else if (action == actionOpenCurrentTab) {
            showContentsTopic();
        }else if (action == actionOpenLinkInNewWindow){
            mw->browsers()->currentBrowser()->openLinkInNewWindow( item->data(0, LinkRole).toString() );
        }else if (action == actionOpenLinkInNewTab){
            mw->browsers()->currentBrowser()->openLinkInNewPage( item->data(0, LinkRole).toString() );
        }else if (action == actionOpenLinkInExtEditor){
            mw->OpenInExternalApplication(Config::configuration()->ExternalEditor(), item->data(0, LinkRole).toString() );	
        }else if (action == actionOpenLinkInExtBrowser){
            mw->OpenInExternalApplication(Config::configuration()->ExternalBrowser(), item->data(0, LinkRole).toString() );
        }else
            qWarning() << "The Popup item menu is not discribed in HelpDialog::showContentsItemMenu()!";
    }
}

//-------------------------------------------------
void HelpDialog::showTreeItemMenu(const QPoint &pos) //bookmark popup menu
{
    QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(sender());
    if (!treeWidget)
        return;

    QTreeWidgetItem *item = treeWidget->itemAt(pos);
    if (!item)
        return;

    QAction *action = itemPopup->exec(treeWidget->viewport()->mapToGlobal(pos));
    if (action == actionOpenCurrentTab) {
        if (ui.tabWidget->currentWidget()->objectName() == QLatin1String("contentPage"))
            showContentsTopic();
        else
            showBookmarkTopic();
    } else if (action) {
        QTreeWidgetItem *i = (QTreeWidgetItem*)item;
        if (action == actionOpenLinkInNewWindow)
            mw->browsers()->currentBrowser()->openLinkInNewWindow(i->data(0, LinkRole).toString());
        else if (action == actionOpenLinkInExtEditor)
            mw->OpenInExternalApplication(Config::configuration()->ExternalEditor(), i->data(0, LinkRole).toString() ); //!+! does not work yet
        else if (action == actionOpenLinkInExtBrowser)
            mw->OpenInExternalApplication(Config::configuration()->ExternalBrowser(), i->data(0, LinkRole).toString() ); //!+! does not work yet
        else
            mw->browsers()->currentBrowser()->openLinkInNewPage(i->data(0, LinkRole).toString());
    }
}

//-------------------------------------------------
void HelpDialog::showItemProperties()
{	
    QTreeWidgetItem *item;
    item = ui.listContents->currentItem(); //ContCur == ContTreeView
    if (ContCur == ContSubItems)
        item = ui.TWSubItems->currentItem();
    QString title = item->text(0);
    QString fName = unurlifyFileName(item->data(0, LinkRole).toString());
    QString iconFN= unurlifyFileName(item->data(0, IconFNRole).toString());
    mw->browsers()->currentBrowser()->updateItem(title, fName, iconFN);
    qDebug() << " item prop title = " << title << ", link = " << fName;
}

//-------------------------------------------------
void HelpDialog::on_termsEdit_returnPressed()
{
    startSearch();
}

//-------------------------------------------------
void HelpDialog::updateSearchButton(const QString &txt)
{
    ui.searchButton->setDisabled(txt.isEmpty());
}

//-------------------------------------------------
void HelpDialog::on_searchButton_clicked()
{
    startSearch();
}

//-------------------------------------------------
//insert new document in content
// нужная херь
void HelpDialog::InsertContentsItem(QString title, QString fileName, QString iconFN) 
{


    QString strfor = fileName;
    strfor.remove("file:");


    QTreeWidgetItem *newEntry;
    if (newSameLevelItem)
    {
        qDebug() << "\n" << m_bookadddialog->bookChapterQty << " " << m_bookadddialog->bookFullName << " " << m_bookadddialog->bookShortName;
        qDebug() << "\n" << title << " " << fileName << " " << iconFN << " " << Config::configuration()->CurPrjDir() << "\n";

        QString fileNameFor = fileName;
        QString fileNameFor2 = fileName;
        QFile file1(fileNameFor2.remove("file:"));


        if(file1.exists())
        {
            if(!file1.open(QIODevice::Append | QIODevice::Text))
            {
                qDebug() << "Error write";
            }
            else
            {
                file1.write(QString("\nPathName = %1"
                                    "\nFullName = %2"
                                    "\nShortName = %3"
                                    "\nChapterQty = %4")
                            .arg(fileNameFor.remove(Config::configuration()->CurPrjDir()).remove("file:/"))
                            .arg(m_bookadddialog->bookFullName)
                            .arg(m_bookadddialog->bookShortName)
                            .arg(m_bookadddialog->bookChapterQty)
                            .toUtf8());
            }
        }
        else
            qDebug() << "Error exist";
        file1.close();

        Config::configuration()->toPrjLog(3," + Item on same Level");
        if ( ui.listContents->indexOfTopLevelItem(ui.listContents->currentItem()) >= 0 ){ // this is a top-level item
            newEntry = new QTreeWidgetItem(ui.listContents, 0);
            ui.listContents->addTopLevelItem(newEntry);
        }else{
            QTreeWidgetItem *afterItem = (QTreeWidgetItem*)ui.listContents->currentItem()->parent();
            newEntry = new QTreeWidgetItem(ui.listContents->currentItem()->parent(), afterItem, 0);
        }
        newEntry->setText(0,m_bookadddialog->bookFullName);





    }else{
        Config::configuration()->toPrjLog(3," + subItem");

        QTreeWidgetItem *afterItem = (QTreeWidgetItem*)ui.listContents->currentItem();
        newEntry = new QTreeWidgetItem(ui.listContents->currentItem(), afterItem, 0   );
        newEntry->setText(0,title);
    }

    if (!iconFN.isEmpty())
        newEntry->setIcon(0, QIcon(iconFN));
    newEntry->setData(0, LinkRole, fileName);
    newEntry->setData(0, IconFNRole, iconFN);


    ui.listContents->setCurrentItem(newEntry);


    //qDebug() << "\n" << strfor << "\n";
    if (newSameLevelItem)
    {
        qDebug() << "\nbook!\n";
        fixedBookConfFile(strfor,newEntry,title);
    }
    else
    {
        qDebug() << "\nchapter!\n";
        qDebug() << newEntry->parent()->data(0,LinkRole);
        qDebug() << newEntry->parent()->data(0,LinkRole).toString();
        qDebug() << newEntry->parent()->data(0,LinkRole).toString().remove("file:");
        fixedBookConfFile(newEntry->parent()->data(0,LinkRole).toString().remove("file:"),newEntry->parent(),title);

    }



    showContentsTopic();


    QString t = mw->browsers()->currentBrowser()->getTagTitle();
    if (t.isEmpty()) {
        mw->browsers()->currentBrowser()->setTagTitle(ui.listContents->currentItem()->text(0));
        mw->browsers()->sourceChanged();
    }
    mw->projectModified(true);



    //


    //

    // не устанавливает нужный заголовок файла
    if (newSameLevelItem and m_bookadddialog->bookCheckAutoChapterCreate)
    {
        for (int i=1; i<=m_bookadddialog->bookChapterQty; ++i)
        {
            QString title = tr("%1").arg(i);
            QString iconFN =""; //!+! assign default item icon
            QString fileName;
            bool uniqFN = false;
            int counter = 1;
            while (!uniqFN){
                fileName = Config::configuration()->CurPrjDir() + "/book_"+m_bookadddialog->bookFullName+"_chapter_"+QString::number(counter)+".htm";
                uniqFN = !QFile::exists(fileName);
                counter++;
            }
            qDebug() << "- fn = " << fileName << "counter = " << counter;
            createEmptyHtml(fileName, title);
            InsertChapter(newEntry,title, urlifyFileName(fileName), iconFN);// отвечает за добавление файла в список
        }
    }
}




void HelpDialog::InsertChapter(QTreeWidgetItem * book, QString title, QString fileName, QString iconFN)
{
    QTreeWidgetItem *newEntry;
    Config::configuration()->toPrjLog(3," + subItem");
    QTreeWidgetItem *afterItem = book;
    newEntry = new QTreeWidgetItem(book, afterItem, 0);
    if (!iconFN.isEmpty())
        newEntry->setIcon(0, QIcon(iconFN));
    newEntry->setText(0,title);
    newEntry->setData(0, LinkRole, fileName);
    newEntry->setData(0, IconFNRole, iconFN);
    ui.listContents->setCurrentItem(newEntry/*,2*/); // размещение по колонкам. Колонки колонками, а результат фигня
    showContentsTopic();
    QString t = mw->browsers()->currentBrowser()->getTagTitle();
    if (t.isEmpty()) {
        mw->browsers()->currentBrowser()->setTagTitle(ui.listContents->currentItem()->text(0));
        mw->browsers()->sourceChanged();
    }
    mw->projectModified(true);
}

void HelpDialog::fixedBookConfFile(QString filename, QTreeWidgetItem* item, QString title)
{
    title = title;
    qDebug() << "\nfilename" << filename << "\n";

    // до сюда работает
    QFile file(filename);
    QString string;

    if(file.exists())
    {
        //qDebug() << "до сюда работает 1";
        if(!file.open(QIODevice::ReadWrite  | QIODevice::Text))
        {
            qDebug() << "Error write";
        }
        else
        {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("UTF-8"));

            //qDebug() << "до сюда работает 2";
            QString str;
            while (!stream.atEnd())
            {
                str = stream.readLine();
                str.toUtf8();
                qDebug() << "str = " << str;
                //                    if (newSameLevelItem and (str == "</body></html>"))
                //                    {
                //                        str="";
                //                    }

                //if (str.indexOf("ChapterQty ="))
                if (item->childCount())
                {

                    int stint = str.indexOf("ChapterQty =");

                    if (stint >0)
                    {
                        qDebug() << "str do  " << str;
                        str.remove(stint, str.length()-stint);
                        qDebug() << "str remove " << str;
                        str.append(QString("ChapterQty = %1</p></body></html>").arg(item->childCount()));
                        qDebug() << "str append " << str;
                    }


                    //                         if (item->childCount() )
                    //                         {
                    //                             if (str.indexOf("ChapterQty =")>=0)
                    //                             {

                    //                             int stint = str.indexOf("ChapterQty =");
                    //                             qDebug() << "str do  " << str;
                    //                             str.remove(stint, str.length()-stint);
                    //                             qDebug() << "str remove " << str;
                    //                             str.append(QString("ChapterQty = %1</p></body></html>").arg(item->childCount()));
                    //                             qDebug() << "str append " << str;
                    //                             }


                    //                         }

                }

                if (str.left(13) == "ChapterQty = ")
                {
                    //qDebug() << "\n" << "find!";
                    //str = QString("ChapterQty = %1").arg(m_bookadddialog->bookChapterQty);
                    //qDebug() << "\n" << ui.listContents->currentItem() << " " << ui.listContents->currentItem()->childCount();
                    str = QString("ChapterQty = %1\n").arg(item->childCount());

                }

                string.append(str+"\n");
                // не перезаписывается, а дописывается в конец файла

            }

            //                if (newSameLevelItem)
            //                    string.append("</body>\n</html>");

            qDebug() << "\nstring = " << string;
        }
    }
    else
        qDebug() << "Error exist";
    file.close();

    file.remove();

    // createEmptyHtml(f)
    //createEmptyHtml(filename, title, string);


    //qDebug() << "до сюда работает 1";
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error write";
    }
    else
    {
        file.write(QString("%1").arg(string).toUtf8());


    }
    file.close();






}

//-------------------------------------------------
//for saving contents to file. Used by HelpDialog::saveContents()
static void store2xml(QTreeWidgetItem *i, QTextStream &ts) 
{	
    ts  << indent << "<section title=\"" << i->text(0)
        << "\" ref=\"" << relatifyFileName(i->data(0, LinkRole).toString(),  Config::configuration()->CurPrjDir())
        << "\" icon=\"" << relatifyFileName(i->data(0, IconFNRole).toString(),  Config::configuration()->CurPrjDir()) + "\" >" << endl;

    for (int index = 0; index < i->childCount(); ++index){
        indent += ind1;
        store2xml(i->child(index), ts);
        indent.remove(0,ind1.length());
    }
    ts << indent << "</section>" << endl;
}

//-------------------------------------------------
//for saving contents to file. Used by HelpDialog::saveContents()
static void store2xml(QTreeWidget *tw, QTextStream &ts)  
{
    ts << "<contents>" << endl;
    for (int index = 0; index < tw->topLevelItemCount(); ++index){
        store2xml(tw->topLevelItem(index), ts);
    }
    ts << "</contents>" << endl;
}

//-------------------------------------------------
void HelpDialog::saveProject()
{
    saveProject(Config::configuration()->CurProject());
}

//-------------------------------------------------
void HelpDialog::saveProject(QString profileFN)
{
    QString str;
    QMap<QString, QString>::iterator i;
    //QFile f(Config::configuration()->curprjDir + QDir::separator() + Config::configuration()->profileName() + "2.pp");
    QFile f(profileFN);
    if (!f.open(QFile::WriteOnly))
        return;

    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << "<raproject version=\"1.0\">\n\n"

          // save profile settings
       << "<profile>\n";
    for (i = Config::configuration()->profile()->props.begin(); i != Config::configuration()->profile()->props.end(); ++i){
        if (i.key() == "startpage"){
            str = relatifyFileName(i.value(), Config::configuration()->CurPrjDir());
        }else
            str = i.value();
        ts << ind1 << "<property name=\"" << i.key() << "\">" << Qt::escape(str) << "</property>" << endl;
    }
    ts << "</profile>\n\n";

    // save contents
    store2xml(ui.listContents, ts);
    ts << "\n</raproject>\n";
    f.close();
    mw->projectModified(false);
    mw->statusBar()->showMessage(tr("Project has been saved"), 5000);
}

//-------------------------------------------------
void HelpDialog::updateItemProperties(QString title, QString fileName, QString iconFN) //update item properties
{
    QTreeWidgetItem *item;
    item = ui.listContents->currentItem(); //ContCur == ContTreeView
    if (ContCur == ContSubItems)
        item = ui.TWSubItems->currentItem();
    QString oldFN = item->data(0, LinkRole).toString();
    item->setText(0, title);
    item->setData(0, LinkRole, fileName);
    item->setData(0, IconFNRole, iconFN);
    if (!iconFN.isEmpty() )
    	item->setIcon(0, QIcon(iconFN));
    if (oldFN != fileName) //reload 
    	showContentsTopic();
    mw->projectModified(true);
}

//-------------------------------------------------
void HelpDialog::removeItem()
{
    int ret = QMessageBox::warning(this, GL_Prog_Name, tr("Delete just contents item? Source file will be untouched."),
                                   QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Yes){
        removeItemDontAsk();
    }
}

//-------------------------------------------------
void HelpDialog::removeItemDontAsk()
{
    QTreeWidgetItem *parent = ui.listContents->currentItem()->parent();
    int index;

    if (parent) {
        index = parent->indexOfChild(ui.listContents->currentItem());
        delete parent->takeChild(index);
    } else {
        index = ui.listContents->indexOfTopLevelItem(ui.listContents->currentItem());
        delete ui.listContents->takeTopLevelItem(index);
    }
    mw->projectModified(true);
}

//-------------------------------------------------
void HelpDialog::deleteItem()
{
    QTreeWidgetItem *item = ui.listContents->currentItem();
    QString fName = item->data(0, LinkRole).toString();
    if (fName.startsWith("file:")) {
        fName.remove(0,5);
    }
    if (QFile::exists(fName)){
        int ret = QMessageBox::warning(this, GL_Prog_Name, tr("Delete contents item AND source file?"),
                                       QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (ret == QMessageBox::Yes){
            QFile::remove(fName);
            removeItemDontAsk();
        }
    }
}

//-------------------------------------------------
void HelpDialog::on_BProjectAdd_clicked()
{
    QString prjFN = Config::configuration()->CurProject();
    if (!Config::configuration()->ProfileFNs().contains(prjFN) ){
        Config::configuration()->addProject(prjFN);
        ui.CBProjects->insertItem(0, Config::configuration()->profileTitle() );
        ui.CBProjects->setItemData(0, prjFN, LinkRole);
        //!+! add icon for project QComboBox::setItemIcon ( int index, const QIcon & icon )
        enableProjectButtons();
        mw->projectModified(true);
    }
}

//-------------------------------------------------
void HelpDialog::on_BProjectDelete_clicked()
{
    if (ui.CBProjects->itemData(ui.CBProjects->currentIndex(), LinkRole) != "Empty link"){
        Config::configuration()->delProject( Config::configuration()->CurProject() );
        ui.CBProjects->removeItem(ui.CBProjects->currentIndex());
        /* ?+? set <Not in the list> if project not in the list
  QString title = tr("<Not in the list>");
     ui.CBProjects->insertItem(0, title );
  ui.CBProjects->setItemData(0, "Empty link", LinkRole); */
        ui.CBProjects->setCurrentIndex(-1);
        enableProjectButtons();
        mw->projectModified(true);
    }
}

//-------------------------------------------------
void HelpDialog::loadProjectFromList(int prjIndex)
{
    //!+! check if project modified and ask for saving. Don not save automaticly
    saveProject();
    mw->ProjectOpen(ui.CBProjects->itemData(prjIndex, LinkRole).toString());
}

//-------------------------------------------------
void HelpDialog::enableProjectButtons()
{
    int i;
    int n = ui.CBProjects->count();
    int ind = -1;
    bool prjInList = false;

    for (i=0; i<n; i++){
        if (ui.CBProjects->itemData(i, LinkRole).toString() == Config::configuration()->CurProject())
            ind = i;
    }
    if (ind >=0)
        prjInList = true;
    ui.BProjectDelete->setEnabled(prjInList);
    ui.BProjectAdd->setEnabled(!prjInList);
    ui.CBProjects->setCurrentIndex(ind);
}

//-------------------------------------------------
void HelpDialog::showContentsAV(bool b)
{
    ui.TWSubItems->setVisible(b);
}

//-------------------------------------------------
void HelpDialog::showContentsAVHeader(bool b)
{	
    ui.TWSubItems->setItemHidden(ui.TWSubItems->headerItem(), !b);
}

//-------------------------------------------------
void HelpDialog::fillSubList()
{
    int index;
    QTreeWidgetItem *i = ui.listContents->currentItem();
    int n = i->childCount();
    QString iconFN;

    ui.TWSubItems->clear();
    ui.TWSubItems->setHeaderLabel(i->text(0));

    for (index = 0; index < n; ++index){
        QTreeWidgetItem *item = new QTreeWidgetItem( ui.TWSubItems, 0);
        iconFN = i->child(index)->data(0,IconFNRole).toString();
    	item->setText(0, i->child(index)->text(0));
    	item->setData(0, LinkRole, i->child(index)->data(0,LinkRole));
    	item->setData(0, IconFNRole, iconFN);
        if (!iconFN.isEmpty() )
            item->setIcon(0, QIcon(iconFN));
    }
}

//-------------------------------------------------
void HelpDialog::CreateAutoItems()
{
    dialogAutoItems->show();
}

//-------------------------------------------------
void HelpDialog::cloneItem(QTreeWidgetItem *item, bool subItem)
{ 
    if (subItem){
        ui.listContents->currentItem()->addChild(item);
    }else //TopLevelItem
        ui.listContents->addTopLevelItem(item);
    qDebug() <<  "inserted item title = " << item->text(0) << ", link = " << item->data(0, LinkRole);
}

//-------------------------------------------------
void HelpDialog::cloneDone()
{
    saveProject();
    mw->ProjectOpen( Config::configuration()->CurProject() );
}

//-------------------------------------------------
void HelpDialog::currentItemChanged(QTreeWidgetItem* curItem,QTreeWidgetItem* prevItem)
{
    ui.listContents->closePersistentEditor(prevItem);
    showTopic(curItem);
    //ui.listContents->setFocus();
}

//-------------------------------------------------
void HelpDialog::newItem()
{    
    // определяем уровень дерева для нового элемента, если больше 3, то отправляет внах
    int depth = 1;
    for(QTreeWidgetItem *parent = NULL, *cur = ui.listContents->currentItem(); parent = cur->parent(); cur = parent, ++depth);
    qDebug("level = %d", depth);
    if (depth < 2)
    {
	if (! Config::configuration()->ItemAutoProperties()){ 
            mw->browsers()->currentBrowser()->fileNew();
	}else{//autocreate all properties for new item
            QString title = str_NewItemTitle;
            QString iconFN =""; //!+! assign default item icon
            QString fileName;
            bool uniqFN = false;
            int counter = 1;

            if (newSameLevelItem)
            {
                while (!uniqFN){
                    //fileName = Config::configuration()->CurPrjDir() + "/book_"+QString::number(counter)+".htm";
                    fileName = Config::configuration()->CurPrjDir() + "/book_"+m_bookadddialog->bookFullName+".htm";
                    uniqFN = !QFile::exists(fileName);
                    counter++;
                }
            }
            else
            {
                while (!uniqFN){
                    //fileName = Config::configuration()->CurPrjDir() + "/book_"+m_bookadddialog->bookFullName+"_chapter_"+QString::number(counter)+".htm";
                    fileName = Config::configuration()->CurPrjDir() + "/book_"+ui.listContents->currentItem()->text(0)+"_chapter_"+QString::number(counter)+".htm";
                    uniqFN = !QFile::exists(fileName);
                    counter++;
                }
                title = QString("%1").arg(counter-1);
            }
            qDebug() << "- fn = " << fileName << "counter = " << counter;
            createEmptyHtml(fileName, title);
            InsertContentsItem(title, urlifyFileName(fileName), iconFN);// отвечает за добавление файла в список
            ui.listContents->setFocus();
            //ui.listContents->openPersistentEditor(ui.listContents->currentItem(),0);  // редактировать название в дереве
	}	
	mw->projectModified(true);
    }

}

//-------------------------------------------------
void HelpDialog::sortAscending()
{
    ui.listContents->setSortingEnabled(true);
    ui.listContents->sortItems(0, Qt::AscendingOrder);
    Config::configuration()->setContentsSortOrder("Ascending");
    //!+! sort just children of selected item
    //void QTreeWidgetItem::sortChildren ( int column, Qt::SortOrder order )
}

//-------------------------------------------------
void HelpDialog::sortDescending()
{
    ui.listContents->setSortingEnabled(true);
    ui.listContents->sortItems(0, Qt::DescendingOrder);
    Config::configuration()->setContentsSortOrder("Descending");
}

//-------------------------------------------------
void HelpDialog::sortDisable()
{
    ui.listContents->setSortingEnabled(false);
    Config::configuration()->setContentsSortOrder("Disable");
}
/*
//-------------------------------------------------
//for building up list of items, to fast locate. HelpDialog::locateItemByName()
static void pickItems(QTreeWidgetItem *i, QList<QTreeWidgetItem> &iList, QString &name) 
{	
 if (i->text(0).contains(name, Qt::CaseInsensitive) )
  iList.append(i);

    for (int index = 0; index < i->childCount(); ++index){
        pickItems(i->child(index), iList, name);
 }
}

//-------------------------------------------------
//for building up list of items, to fast locate. HelpDialog::locateItemByName()
static void pickItems(QTreeWidget *tw, QList<QTreeWidgetItem> &iList, QString &name)  
{
 iList.clear();
    for (int index = 0; index < tw->topLevelItemCount(); ++index){
        pickItems(tw->topLevelItem(index), iList, name);
    }
}
*/

//-------------------------------------------------
bool HelpDialog::findItemByName(QTreeWidgetItem *i, QString &name) //for building up list of items, to fast locate. HelpDialog::locateItemByName()
{	
    if (curChildIndex > liChildIndex) //compare only if we came to the next child (for nextMatchedItem() )
        if (i->text(0).contains(name, Qt::CaseInsensitive) ){
            ui.listContents->setCurrentItem(i);
            Config::configuration()->toPrjLog(3,"item found. liTopLevelIndex=" + QString::number(liTopLevelIndex)+
                                              " liChildIndex=" + QString::number(liChildIndex)+
                                              " curChildIndex=" + QString::number(curChildIndex) );
            liChildIndex = curChildIndex;
            return true;
        }
    for (int index = 0; index < i->childCount(); ++index){
        curChildIndex++;
        if (findItemByName(i->child(index), name) ) 
            return true;
    }
    return false;
}

//-------------------------------------------------
bool HelpDialog::findItemByName(QString &name)  //for building up list of items, to fast locate. HelpDialog::locateItemByName()
{
    for (int index = liTopLevelIndex; index < ui.listContents->topLevelItemCount(); ++index){
        curChildIndex = 0;
        if (findItemByName(ui.listContents->topLevelItem(index), name)) 
            return true;
        liTopLevelIndex++;
        liChildIndex = -1;
    }
    Config::configuration()->toPrjLog(3,"pickItemStartIndex = " + QString::number(liTopLevelIndex));
    return false; 
}

//-------------------------------------------------
void HelpDialog::locateItemByName(QString name)
{
    if(!name.isEmpty()){
        //ui.listContents->keyboardSearch(name); //works only for opened (expanded) items
        //ui.listContents->findItems(name, Qt::MatchStartsWith); //works only for TopLevelItems
        liTopLevelIndex = 0; liChildIndex = -1;
        findItemByName(name);
        ui.LELocateItemByName->setFocus();
    }
}

//-------------------------------------------------
void HelpDialog::nextMatchedItem()
{
    QString name = ui.LELocateItemByName->text();
    if(!name.isEmpty()){
        Config::configuration()->toPrjLog(3,"locate next. name = " + name);
        if(!findItemByName(name))
            locateItemByName(name);
        ui.LELocateItemByName->setFocus();
    }
}

//-------------------------------------------------
void HelpDialog::itemChanged(QTreeWidgetItem* item, int column)
{
    if (mw->browsers()->currentBrowser()->getTagTitle() == str_NewItemTitle)
        mw->browsers()->currentBrowser()->setTagTitle( item->text(column) );
    mw->projectModified(true);
}




void HelpDialog::exportCreateDir(QString current_dir)
{
    // если папки export нету, то создаем, если есть, то удаляем содержимое
    QDir dir(current_dir);
    if (!QDir(QString("%1export/").arg(current_dir)).exists())
    {
        dir.mkdir("export");
    }
    else
    {
        dir.setPath((QString("%1export").arg(current_dir)));
        QStringList lstFiles = dir.entryList(QDir::Files); //Получаем список файлов

        //Удаляем файлы
        foreach (QString entry, lstFiles)
        {
            QString entryAbsPath = dir.absolutePath() + "/" + entry;
            qDebug() << entryAbsPath;
            QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
            QFile::remove(entryAbsPath);
        }
    }

}

void HelpDialog::exportBibleqtIni(QString string)
{
    //экспортируем ini файл
    QFile file(string);
    //    qDebug() << string;
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error write";
    }
    else
    {
        QString stru =""+tr("BibleName = %1"
                            "\nBibleShortName = %2"
                            "\nCopyright = %3"
                            "\nDefaultEncoding = utf-8"
                            "\nChapterSign = <h4>"
                            "\nVerseSign = <p>"
                            "\nBookQty = %4")
                .arg(Config::configuration()->ModuleBiblename())
                .arg(Config::configuration()->ModuleBibleShortName())
                .arg(Config::configuration()->ModuleCopyright())
                .arg(ui.listContents->topLevelItemCount()-1);

        file.write(QString("%1").arg(stru).toUtf8());
    }
    file.close();
}

void HelpDialog::exportBibleqtIniInfo(QString file,int i)
{
    QString filename = ui.listContents->topLevelItem(i)->data(0,LinkRole).toString().remove("file:");
    QString string = exportChapter(filename,ui.listContents->topLevelItem(i)->childCount(),false);

    QFile filebibleqt(file);
    if(!filebibleqt.open(QIODevice::Append))
    {
        qDebug() << "Error write";
    }
    else
    {
        filebibleqt.write(QString("%1").arg(string).toUtf8());
    }
    filebibleqt.close();
    qDebug() << string;

}

QString HelpDialog::exportChapter (QString filename,int i,bool chapt)
{
    // подготовливаем главы
    QFile file(filename);
    QString str;
    if(file.exists())
    {
        if(!file.open(QIODevice::ReadWrite  | QIODevice::Text))
        {
            qDebug() << "Error write";
        }
        else
        {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("UTF-8"));

            str = stream.readAll();

            QRegExp title("<title>[1]</title>");
            QRegExp rx("<[^>]*>");
            QRegExp rxp("(<p.*?>)");
            rxp.setPatternSyntax(QRegExp::RegExp);

//            QRegExp rxe("(?<=style/=\")[^]+?(?=\")");

            if (chapt)
            {
                qDebug() << "hahaha1";
                QString title = QString("<title>%1</title>").arg(i);
                QString chapter = tr("\n?h4?Глава %1?/h4?").arg(i);
                str.replace(title,chapter);
                //                QString title = QString("<title>%1</title>").arg(i-1);
                //                str.remove(title);

            }
            else
            {
                str.remove(title);
                qDebug() << "hahaha";
            }


            //            QRegExp rx2("^(?<=^|>)[^><]+?(?=<|$)");


            //            QString teststr = str; //"<h4>test</h4>";

//            teststr = teststr.replace("<h4>","?h4?").replace("</h4>","?/h4?").remove("p, li { white-space: pre-wrap; }").remove("<title>1</title>").replace(rxp,"?p?").remove("</p>").remove(rx);


            //            qDebug() << " =========begin=====";
            //            qDebug() << teststr;
            //            qDebug() << teststr;
            //            qDebug() << "1";
            //            qDebug() << teststr.replace("<h4>","?h4?");
            //            qDebug() << "2";
            //            qDebug() << teststr.replace("</h4>","?/h4?");
            //            qDebug() << "3";
            //            qDebug() << teststr.remove("p, li { white-space: pre-wrap; }");
            //            qDebug() << "4";
            //            qDebug() << teststr.remove(title);
            //            qDebug() << "5";
            //            qDebug() << teststr.replace(rxp,"?p?"); // этот вырезает
            //            qDebug() << "6";
            //            qDebug() << teststr.remove("</p>");
            //            qDebug() << "7";
            //            //            qDebug() << teststr.remove(rx);
            //            qDebug() << " =========center=====";

            //            //                        str.replace("FullName","\nFullName").replace("ShortName","\nShortName").replace("ChapterQty","\nChapterQty");
            //            teststr = teststr.remove("p, li { white-space: pre-wrap; }").remove(title).replace(rxp,"?p?").remove("</p>").remove(rx).replace("FullName","\nFullName").replace("ShortName","\nShortName").replace("ChapterQty","\nChapterQty");;
            //            qDebug() << teststr;

            //            qDebug() << " =======end====== ";

            str.remove("p, li { white-space: pre-wrap; }")
                    .remove(title)
                    .replace(rxp,"?p?")
                    .remove("</p>")
                    .remove(rx)
                    .replace("FullName","\nFullName")
                    .replace("ShortName","\nShortName")
                    .replace("ChapterQty","\nChapterQty");

            //qDebug() << "text = " << str;
        }
    }
    else
        qDebug() << "Error exist";
    file.close();
    //file.remove();
    return str;
}

void HelpDialog::exportBibleBook(QString filenamebook, int i)
{
    QFile filebook(filenamebook);
    filebook.remove();
    if(!filebook.open(QIODevice::Append))
    {
        qDebug() << "Error write";
    }
    else
    {
        filebook.write(QString("<html>\n<head>\n<title>NAME</title>\n</head>\n<body>").toUtf8());
        for (int j=ui.listContents->topLevelItem(i)->childCount()-1; j>=0 ;--j)
        {
            QString filenamechapter = ui.listContents->topLevelItem(i)->child(j)->data(0,LinkRole).toString().remove("file:");
            int icount = j+1;
            filebook.write(QString("%1").arg(exportChapter(filenamechapter, icount, true)).toUtf8());
        }
        filebook.write(QString("\n</body>\n</html>").toUtf8());
    }
    filebook.close();
}




void HelpDialog::exportModule()
{
    QString fileBibleqtName = ui.listContents->topLevelItem(0)->data(0,LinkRole).toString().remove("file:");
    QString curdir = QString(fileBibleqtName.replace("Bibleqt.ini",""));
    exportCreateDir(curdir);
    exportBibleqtIni(QString("%1export/bibleqt.ini").arg(curdir));

    for (int i = 1; i < ui.listContents->topLevelItemCount(); ++i)  //0 это Bibleqt.ini
    {
        QString filename = ui.listContents->topLevelItem(i)->data(0, LinkRole).toString().remove("file:");
        filename = curdir+"export/"+filename.split("/").last();
        exportBibleqtIniInfo(QString("%1export/bibleqt.ini").arg(curdir),i);
        exportBibleBook(filename, i);
    }
}




