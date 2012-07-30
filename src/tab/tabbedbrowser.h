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


#ifndef TABBEDBROWSER_H
#define TABBEDBROWSER_H

#include "ui_tabbedbrowser.h"

class MainWindow;
class HelpWindow;
class QStyleSheet;
class QMimeSourceFactory;
class QTimer;
class LineNumberArea;


class LineNumberArea : public QWidget
{
public:

    LineNumberArea(QWidget *parent)
    {
//        codeEditor = editor;
    }

    QSize sizeHint() const
    {
//        return QSize(codeEditor->lineNumberAreaWidth(), 0);
        return (QSize(2,2));
    }

protected:
    void paintEvent(QPaintEvent *event)
    {
//        codeEditor->lineNumberAreaPaintEvent(event);
    }

//private:
//    CodeEditor *codeEditor;
};


class TabbedBrowser : public QWidget
{
    Q_OBJECT
public:
    TabbedBrowser(MainWindow *parent);
    virtual ~TabbedBrowser();

    MainWindow *mainWindow() const;
    HelpWindow *currentBrowser() const;
    QStringList sources() const;
    QList<HelpWindow*> browsers() const;

    HelpWindow* newBackgroundTab();
    HelpWindow* createHelpWindow();

    void setAppTitle(HelpWindow*, const QString &);

signals:
    void tabCountChanged(int count);
    void browserUrlChanged(const QString &link);
    void showDocProperties();

protected:
        void keyPressEvent(QKeyEvent *);
        bool eventFilter(QObject *o, QEvent *e);

public slots:
    void init();
    void forward();
    void backward();
    void setSource(const QString &ref);
    void reload();
    void home();
    void nextTab();
    void previousTab();
    void newTab(const QString &lnk);

    void updateTitle(const QString &title);
    void newTab();
    void transferFocus();
    void initHelpWindow(HelpWindow *win);
    void setup();
    void copy();
    void closeTab();
    void closeAllTabs();
    void sourceChanged();


    void find();
    void findNext();
    void findPrevious();

private slots:
    void find(QString, bool forward = false, bool backward = false);
    void openTabMenu(const QPoint& pos);

private:
    Ui::TabbedBrowser ui;
    QWidget *lastCurrentTab;
    QFont tabFont;
    QWidget *lineNumberArea;

    QString fixedFontFam;
    QColor lnkColor;
    bool underlineLnk;
        QTimer *autoHideTimer;
};

#endif // TABBEDBROWSER_H
