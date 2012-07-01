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


#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMap>
#include <QPointer>
#include <QToolBar>
#include "raedit.h"
#include "itemproperty.h"
#include "linkproperty.h"
#include "docproperty.h"
#include "tableproperty.h"
#include "cellsplit.h"
//#include "imageproperty.h"


class MainWindow;
class QKeyEvent;
class QMime;
class QMouseEvent;
class QMenu;
//------- taken from TextEdit -------
class QAction;
class QComboBox;
class QFontComboBox;
class QTextCharFormat;
//------- end of TextEdit section -------

//==================== class HelpWindow ====================

class HelpWindow : public raEdit
{
    Q_OBJECT
public:
    HelpWindow( MainWindow *m, QWidget *parent = 0);
    void setSource( const QUrl &name );
    void blockScrolling( bool b );
    void openLinkInNewWindow( const QString &link );
    void openLinkInNewPage( const QString &link );
    void addMimePath( const QString &path );

    QString getTagTitle();

    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *);
    
    //    bool ModeSourceHTML;

signals:
    void chooseWebBrowser();
    void choosePDFReader();
    void insertContentsItem(QString title, QString fileName, QString iconFN);
    void updateContentsItem(QString title, QString fileName, QString iconFN);
    
public slots:
    void updateItem(QString title, QString fileName, QString iconFN);
    void setTagTitle(QString title);
    void fileNew();
    void fileOpen();
    bool fileSave();

    //void insertTable(int rows, int columns, const QTextTableFormat & format);
    void imageInsert(QString html);
    void imageUpdate(QString html);
    //void imageInsert(int height, int width, QImage img);
    //void imageUpdate(int height, int width, QImage img);

    void insertRichText(QString text);	//insert HTML text at cursor. Used from other modules, f.e. MainWindow::setMenuSign()

protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    //------- taken from TextEdit -------
    virtual void closeEvent(QCloseEvent *e);

protected slots:
    void ensureCursorVisible();

private:

    bool hasAnchorAt(const QPoint& pos);

    MainWindow *mw;
    ItemProperties *itemprop;
    LinkProperties *linkprop;
    DocProperties *docprop;
    //	ImageProperties *imageprop;


    QString lastAnchor; //url for document in helpwindow
    bool blockScroll;
    bool shiftPressed;
    //bool ctrlPressed;
    bool newWindow;
    bool fwdAvail;
    bool backAvail;


    //------- taken from TextEdit -------
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    bool load(const QString &f);
    bool maybeSave();
    void setCurrentFileName(const QString fName);

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;
    QToolBar *tb;
    int selCur, selStart, selEnd;
    
private slots:
    void openLinkInNewWindow();
    void openLinkInNewPage();
    void showLinkProperties();
    void removeLink();
    void updateLink(QString lText, QString lLocation);
    void loadNewItemFile();
    void showDocProperties();

    //------- taken from TextEdit -------
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();


    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textStyle(int styleIndex);
    void textColor();
    //    void textAlign(QAction *a);
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textAlignJustify();

    void currentCharFormatChanged(const QTextCharFormat &format); 
    void cursorPositionChanged();
    void clipboardDataChanged();

}; // class HelpWindow

#endif // HELPWINDOW_H
