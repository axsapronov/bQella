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
#include "tabbedbrowser.h"
#include "mainwindow.h"
#include "helpwindow.h"
#include "config.h"

#include <QStyleOptionTab>
#include <QToolTip>
#include <QFileInfo>
#include <QToolButton>
#include <QIcon>
#include <QStyle>
#include <QTimer>
#include <QStackedWidget>
#include <QTimer>
#include <QTextBlock>
#include <QKeyEvent>
#include <QtDebug> //to use qWarning and qDebug messages
#include <QProcess> //to execute external application


TabbedBrowser::TabbedBrowser(MainWindow *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    init();

    QStackedWidget *stack = qFindChild<QStackedWidget*>(ui.tab);
    Q_ASSERT(stack);
    stack -> setContentsMargins(0, 0, 0, 0);
    connect(stack, SIGNAL(currentChanged(int)), parent, SLOT(browserTabChanged()));

    QPalette p = palette();
    p.setColor(QPalette::Inactive, QPalette::Highlight,
        p.color(QPalette::Active, QPalette::Highlight));
    p.setColor(QPalette::Inactive, QPalette::HighlightedText,
        p.color(QPalette::Active, QPalette::HighlightedText));
    setPalette(p);
}

TabbedBrowser::~TabbedBrowser()
{
}

MainWindow *TabbedBrowser::mainWindow() const
{
    return static_cast<MainWindow*>(parentWidget());
}

void TabbedBrowser::forward()
{
//    currentBrowser() -> forward();
//    emit browserUrlChanged(currentBrowser() -> source().toString());
}

void TabbedBrowser::backward()
{
//    currentBrowser() -> backward();
//    emit browserUrlChanged(currentBrowser() -> source().toString());
}

void TabbedBrowser::setSource( const QString &ref )
{
    HelpWindow * win = currentBrowser();
    win -> setSource(ref);
}

void TabbedBrowser::reload()
{
    currentBrowser() -> reload();
}

void TabbedBrowser::home()
{
//!+! go to start page    currentBrowser() -> home();
}

HelpWindow *TabbedBrowser::currentBrowser() const
{
    return static_cast<HelpWindow*>(ui.tab -> currentWidget());
}

void TabbedBrowser::nextTab()
{
    if(ui.tab -> currentIndex()<=ui.tab -> count()-1)
        ui.tab -> setCurrentIndex(ui.tab -> currentIndex()+1);
}

void TabbedBrowser::previousTab()
{
    int idx = ui.tab -> currentIndex()-1;
    if(idx>=0)
        ui.tab -> setCurrentIndex(idx);
}

HelpWindow *TabbedBrowser::createHelpWindow()
{
    MainWindow *mainWin = mainWindow();
    HelpWindow *win = new HelpWindow(mainWin, 0);
    win -> setFrameStyle(QFrame::NoFrame);
    win -> setPalette(palette());
    win -> setSearchPaths(Config::configuration() -> mimePaths());
    ui.tab -> addTab(win, tr("..."));
    connect(win, SIGNAL(highlighted(QString)),
             (const QObject*) (mainWin -> statusBar()), SLOT(showMessage(QString)));
    //connect(win, SIGNAL(backwardAvailable(bool)),  mainWin, SLOT(backwardAvailable(bool)));
    //connect(win, SIGNAL(forwardAvailable(bool)),   mainWin, SLOT(forwardAvailable(bool)));
    connect(win, SIGNAL(sourceChanged(QUrl)), this, SLOT(sourceChanged()));

    ui.tab -> cornerWidget(Qt::TopRightCorner) -> setEnabled(ui.tab -> count() > 1);
        win -> installEventFilter(this);
        win -> viewport() -> installEventFilter(this);
    ui.editFind -> installEventFilter(this);
    return win;
}

HelpWindow *TabbedBrowser::newBackgroundTab()
{
    HelpWindow *win = createHelpWindow();
    emit tabCountChanged(ui.tab -> count());
    return win;
}

void TabbedBrowser::newTab(const QString &lnk)
{
    QString link(lnk);
    if(link.isNull()) {
        HelpWindow *w = currentBrowser();
        if(w)
            link = w -> source().toString();
    }
	HelpWindow *win = createHelpWindow();
	win -> setObjectName("raWorkArea");
    ui.tab -> setCurrentIndex(ui.tab -> indexOf(win));
    if(!link.isNull()) {
         win -> setSource(link);
    }
    emit tabCountChanged(ui.tab -> count());
}


void TabbedBrowser::init()
{

    lastCurrentTab = 0;
    while(ui.tab -> count()) {
        QWidget *page = ui.tab -> widget(0);
        ui.tab -> removeTab(0);
        delete page;
    }

    connect(ui.tab, SIGNAL(currentChanged(int)),
             this, SLOT(transferFocus()));

    QTabBar *tabBar = qFindChild<QTabBar*>(ui.tab);
    QStyleOptionTab opt;
    if (tabBar) {
        opt.init(tabBar);
        opt.shape = tabBar -> shape();
        tabBar -> setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tabBar, SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(openTabMenu(const QPoint&)));
    }

    // workaround for sgi style
    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Button, pal.color(QPalette::Active, QPalette::Window));
    pal.setColor(QPalette::Disabled, QPalette::Button, pal.color(QPalette::Disabled, QPalette::Window));
    pal.setColor(QPalette::Inactive, QPalette::Button, pal.color(QPalette::Inactive, QPalette::Window));

    QToolButton *newTabButton = new QToolButton(this);
    ui.tab -> setCornerWidget(newTabButton, Qt::TopLeftCorner);
    newTabButton -> setCursor(Qt::ArrowCursor);
    newTabButton -> setAutoRaise(true);
    newTabButton -> setIcon(QIcon(Config::configuration() -> ImgDir() + QString("tab_add.png")));
    QObject::connect(newTabButton, SIGNAL(clicked()), this, SLOT(newTab()));
    newTabButton -> setToolTip(tr("Add page"));

    QToolButton *closeTabButton = new QToolButton(this);
    closeTabButton -> setPalette(pal);
    ui.tab -> setCornerWidget(closeTabButton, Qt::TopRightCorner);
    closeTabButton -> setCursor(Qt::ArrowCursor);
    closeTabButton -> setAutoRaise(true);
    closeTabButton -> setIcon(QIcon(Config::configuration() -> ImgDir() + QString("tab_close.png")));
    QObject::connect(closeTabButton, SIGNAL(clicked()), this, SLOT(closeTab()));
    closeTabButton -> setToolTip(tr("Close page"));
    closeTabButton -> setEnabled(false);

        QObject::connect(ui.toolClose, SIGNAL(clicked()), ui.frameFind, SLOT(hide()));
        QObject::connect(ui.toolPrevious, SIGNAL(clicked()), this, SLOT(findPrevious()));
        QObject::connect(ui.toolNext, SIGNAL(clicked()), this, SLOT(findNext()));
        QObject::connect(ui.editFind, SIGNAL(returnPressed()), this, SLOT(findNext()));
        QObject::connect(ui.editFind, SIGNAL(textEdited(const QString&)), this, SLOT(find(QString)));
        ui.frameFind -> setVisible(false);
        ui.labelWrapped -> setVisible(false);
        autoHideTimer = new QTimer(this);
        autoHideTimer -> setInterval(5000);
        autoHideTimer -> setSingleShot(true);
        QObject::connect(autoHideTimer, SIGNAL(timeout()), ui.frameFind, SLOT(hide()));
}

void TabbedBrowser::updateTitle(const QString &title)
{
    ui.tab -> setTabText(ui.tab -> indexOf(currentBrowser()), title.trimmed());
}

void TabbedBrowser::newTab()
{
    newTab(QString());
}

void TabbedBrowser::transferFocus()
{
    if(currentBrowser()) {
        currentBrowser() -> setFocus();
    }
    mainWindow() -> setWindowTitle(Config::configuration() -> profileTitle()
                             + QString(" - ")
                             + currentBrowser() -> documentTitle());
}

void TabbedBrowser::initHelpWindow(HelpWindow * /*win*/)
{
}

void TabbedBrowser::setup()
{
    newTab(QString());
}

void TabbedBrowser::copy()
{
    currentBrowser() -> copy();
}

void TabbedBrowser::closeTab()
{
    if(ui.tab -> count()==1)
        return;
    HelpWindow *win = currentBrowser();
    mainWindow() -> removePendingBrowser(win);
    ui.tab -> removeTab(ui.tab -> indexOf(win));
    QTimer::singleShot(0, win, SLOT(deleteLater()));
    ui.tab -> cornerWidget(Qt::TopRightCorner) -> setEnabled(ui.tab -> count() > 1);
    emit tabCountChanged(ui.tab -> count());
}

void TabbedBrowser::closeAllTabs()
{
    int i;
    int n = ui.tab -> count();
    HelpWindow *win;
    
    if( n> 1) {
    	for (i=1; i<n; i++){ //leave only one tab
    		win = currentBrowser();
    		mainWindow() -> removePendingBrowser(win);
    		ui.tab -> removeTab(ui.tab -> indexOf(win));
    		QTimer::singleShot(0, win, SLOT(deleteLater())); //delete the object 'win'
   		}
    	ui.tab -> cornerWidget(Qt::TopRightCorner) -> setEnabled(false);
    	emit tabCountChanged(ui.tab -> count());
    }
	currentBrowser() -> setSource( Config::configuration() -> profile() -> props["startpage"] );
    //Config::configuration() -> curFile = "";  //?+? may be display empty page on close all tabs?
}


QStringList TabbedBrowser::sources() const
{
    QStringList lst;
    int cnt = ui.tab -> count();
    for(int i=0; i<cnt; i++) {
        lst.append(((raEdit*) ui.tab -> widget(i)) -> source().toString());
    }
    return lst;
}

QList<HelpWindow*> TabbedBrowser::browsers() const
{
    QList<HelpWindow*> list;
    for (int i=0; i<ui.tab -> count(); ++i) {
        Q_ASSERT(::qobject_cast<HelpWindow*>(ui.tab -> widget(i)));
        list.append(static_cast<HelpWindow*>(ui.tab -> widget(i)));
    }
    return list;
}

void TabbedBrowser::sourceChanged()
{
    HelpWindow *win = ::qobject_cast<HelpWindow *>(QObject::sender());
    Q_ASSERT(win);
    QString docTitle(win -> documentTitle());
    if (docTitle.isEmpty())
        docTitle = QString("[- no title -]");
    // Make the classname in the title a bit more visible (otherwise
    // we just see the "Qt 4.0 : Q..." which isn't really helpful ;-)
/*    QString qtTitle = QString("RA : ");
    if (docTitle.startsWith(qtTitle))
        docTitle = docTitle.mid(qtTitle.length());*/
    setAppTitle(win, docTitle);
    ui.frameFind -> hide();
    ui.labelWrapped -> hide();
    win -> setTextCursor(win -> cursorForPosition(QPoint(0, 0))); //!+! move curcor when in SourceView mode
}

void TabbedBrowser::setAppTitle(HelpWindow *win, const QString &title)
{
    const QString tt = title.trimmed();
    ui.tab -> setTabText(ui.tab -> indexOf(win), tt);
    if (win == currentBrowser())
        mainWindow() -> setWindowTitle(Config::configuration() -> profileTitle() + QString(" - ") + tt);
}

void TabbedBrowser::keyPressEvent(QKeyEvent *e)
{
        int key = e -> key();
        QString ttf = ui.editFind -> text();
        QString text = e -> text();

        if (ui.frameFind -> isVisible()) {
                switch (key) {
                case Qt::Key_Escape:
                        ui.frameFind -> hide();
            			ui.labelWrapped -> hide();
                        return;
                case Qt::Key_Backspace:
                        ttf.chop(1);
                        break;
                case Qt::Key_Return:
        case Qt::Key_Enter:
                        // Return/Enter key events are not accepted by QLineEdit
                        return;
                default:
                        if (text.isEmpty()) {
                                QWidget::keyPressEvent(e);
                                return;
                        }
                        ttf += text;
                }
        } else {
                if (text.isEmpty() || text[0].isSpace() || !text[0].isPrint()) {
                        QWidget::keyPressEvent(e);
                        return;
                }
        if (text.startsWith(QLatin1Char('/'))) {
            ui.editFind -> clear();
            find();
            return;
        }
                ttf = text;
                ui.frameFind -> show();
        }

        ui.editFind -> setText(ttf);
        find(ttf, false, false);
}

void TabbedBrowser::findNext()
{
        find(ui.editFind -> text(), true, false);
}

void TabbedBrowser::findPrevious()
{
        find(ui.editFind -> text(), false, true);
}

void TabbedBrowser::find()
{
        ui.frameFind -> show();
        ui.editFind -> setFocus(Qt::ShortcutFocusReason);
        ui.editFind -> selectAll();
        autoHideTimer -> stop();
}

void TabbedBrowser::find(QString ttf, bool forward, bool backward)
{
        HelpWindow *browser = currentBrowser();
        QTextDocument *doc = browser -> document();
        QString oldText = ui.editFind -> text();
        QTextCursor c = browser -> textCursor();
        QTextDocument::FindFlags options;
        QPalette p = ui.editFind -> palette();
        p.setColor(QPalette::Active, QPalette::Base, Qt::white);

        if (c.hasSelection())
                c.setPosition(forward ? c.position() : c.anchor(), QTextCursor::MoveAnchor);

        QTextCursor newCursor = c;

        if (!ttf.isEmpty()) {
                if (backward)
                        options |= QTextDocument::FindBackward;

                if (ui.checkCase -> isChecked())
                        options |= QTextDocument::FindCaseSensitively;

                if (ui.checkWholeWords -> isChecked())
                        options |= QTextDocument::FindWholeWords;

                newCursor = doc -> find(ttf, c, options);
                ui.labelWrapped -> hide();

                if (newCursor.isNull()) {
                        QTextCursor ac(doc);
                        ac.movePosition(options & QTextDocument::FindBackward
                                                        ? QTextCursor::End : QTextCursor::Start);
                        newCursor = doc -> find(ttf, ac, options);
                        if (newCursor.isNull()) {
                                p.setColor(QPalette::Active, QPalette::Base, QColor(255, 102, 102));
                                newCursor = c;
                        } else
                                ui.labelWrapped -> show();
                }
        }

        if (!ui.frameFind -> isVisible())
                ui.frameFind -> show();
        browser -> setTextCursor(newCursor);
        ui.editFind -> setPalette(p);
        if (!ui.editFind -> hasFocus())
                autoHideTimer -> start();
}

bool TabbedBrowser::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui.editFind) {
        if (e -> type() == QEvent::FocusIn && autoHideTimer -> isActive())
            autoHideTimer -> stop();
    } else if (e -> type() == QEvent::KeyPress && ui.frameFind -> isVisible()) { 
                QKeyEvent *ke = static_cast<QKeyEvent *>(e);
                if (ke -> key() == Qt::Key_Space) {
                        keyPressEvent(ke);
                        return true;
                }
        }

        return QWidget::eventFilter(o, e);
}

void TabbedBrowser::openTabMenu(const QPoint& pos)
{
    QTabBar *tabBar = qFindChild<QTabBar*>(ui.tab);
    QMenu *m = new QMenu(tabBar);
    QAction *docProp_action = m -> addAction(tr("Document properties"));
    QAction *extEditor_action = m -> addAction(tr("Open in external editor"));
    QAction *extBrowser_action = m -> addAction(tr("Open in external browser"));
    m -> addSeparator();
    QAction *new_action = m -> addAction(tr("New Tab"));
    QAction *close_action = m -> addAction(tr("Close Tab"));
    QAction *close_others_action = m -> addAction(tr("Close Other Tabs"));
    if (tabBar -> count() < 2) {
        close_action -> setEnabled(false);
        close_others_action -> setEnabled(false);
    }
    QAction *action_picked = m -> exec(tabBar -> mapToGlobal(pos));
    if (action_picked) {
        if (action_picked == new_action) {
            newTab();
        }else if (action_picked == close_action) {
            for (int i=0; i< tabBar -> count(); ++i) {
                if (tabBar -> tabRect(i).contains(pos)) {
                    HelpWindow *win = static_cast<HelpWindow*>(ui.tab -> widget(i));
                    mainWindow() -> removePendingBrowser(win);
                    QTimer::singleShot(0, win, SLOT(deleteLater()));
                    ui.tab -> cornerWidget(Qt::TopRightCorner) -> setEnabled(ui.tab -> count() > 1);
                    emit tabCountChanged(ui.tab -> count());
                    break;
                }
            }
        }else if (action_picked == close_others_action) {
            int current_tab_index = -1;
            for (int i=0; i< tabBar -> count(); ++i) {
                if (tabBar -> tabRect(i).contains(pos)) {
                    current_tab_index = i;
                    break;
                }
            }
            for (int i=tabBar -> count()-1; i>=0; --i) {
                if (i == current_tab_index) {
                    continue;
                } else {
                    HelpWindow *win = static_cast<HelpWindow*>(ui.tab -> widget(i));
                    mainWindow() -> removePendingBrowser(win);
                    QTimer::singleShot(0, win, SLOT(deleteLater()));
                    if (i < current_tab_index)
                        --current_tab_index;
                }
            }
            ui.tab -> cornerWidget(Qt::TopRightCorner) -> setEnabled(ui.tab -> count() > 1);
            emit tabCountChanged(ui.tab -> count());
        }else if (action_picked == docProp_action) {
        	emit showDocProperties();
        	//mainWindow() -> browsers() -> docprop -> show();	
       	}else if (action_picked == extEditor_action){
        	currentBrowser() -> fileSave();
        	mainWindow() -> OpenInExternalApplication(Config::configuration() -> ExternalEditor(), currentBrowser() -> source().toString());		
       	}else if (action_picked == extBrowser_action){
        	currentBrowser() -> fileSave();
        	mainWindow() -> OpenInExternalApplication(Config::configuration() -> ExternalBrowser(), currentBrowser() -> source().toString());		
       	}
    }
    delete m;
}
