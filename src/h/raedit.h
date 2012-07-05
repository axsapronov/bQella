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


#ifndef QRAEDITOR_H
#define QRAEDITOR_H

#include <QtGui/qtextedit.h>
#include <QtCore/qurl.h>


QT_MODULE(Gui)
//====================== class raEdit ============================
class raEdit : public QTextEdit  //this is a remake of QTextBrowser
{
	//The Q_OBJECT macro must appear in the private section of a class definition that declares its own signals and slots or that uses other services provided by Qt's meta-object system.
    Q_OBJECT

	//This macro is used for declaring properties in classes that inherit QObject. Properties behave like class data members, but they have additional features accessible through the Meta-Object System.
	//qthelp://com.trolltech.qt.442/qdoc/properties.html
    Q_PROPERTY(QUrl source READ source WRITE setSource)
	
public:
    explicit raEdit(QWidget* parent = 0);
    virtual ~raEdit();

    QUrl source() const;
    void setSearchPaths(const QStringList &paths);
    virtual QVariant loadResource(int type, const QUrl &name);
    void setOpenExternalLinks(bool open){ openExternalLinks = open; }
    void setOpenLinks(bool open)		{ openLinks = open; }

//-pm- following brought from class raEditPrivate : public QTextEditPrivate
//-pm- to avoid the use of additional .h files such as qtextedit_p.h
    QUrl currentURL;
    QStringList searchPaths;

    /*flag necessary to give the linkClicked() signal some meaningful
      semantics when somebody connected to it calls setText() or setSource() */
    bool textOrSourceChanged;
    bool forceLoadOnSourceChange;
    bool openExternalLinks;
    bool openLinks;

//#ifndef QT_NO_CURSOR
    //QCursor oldCursor;
//#endif
    QString findFile(const QUrl &name) const;

    void _q_activateAnchor(const QString &href);
    void _q_highlightLink(const QString &href);

    void setSource(const QUrl &url);
    QUrl resolveUrl(const QUrl &url) const;
	//-pm- end of raEditPrivate section

	bool ModeHtml() { return modeHtml; }
	void setModeHtml(bool b) { modeHtml = b; }

signals:
    void sourceChanged(const QUrl &);
    void highlighted(const QUrl &);
    void highlighted(const QString &);
    void anchorClicked(const QUrl &);
//    void modifed(const bool &);

public slots:
   // virtual void setSource(const QUrl &name);
    virtual void reload();
    inline void _q_documentModified()
    {
        textOrSourceChanged = true;
//        emit modifed(textOrSourceChanged);
        forceLoadOnSourceChange = !currentURL.path().isEmpty();
    }

protected:
    bool event(QEvent *e);
    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void focusOutEvent(QFocusEvent *ev);
    virtual void mouseMoveEvent(QMouseEvent *ev);
    virtual void mousePressEvent(QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);
    //virtual bool focusNextPrevChild(bool next);
	// virtual void paintEvent(QPaintEvent *e);
private:
	bool modeHtml;
	QTextDocument *doc;

};

#endif // QRAEDITOR_H
