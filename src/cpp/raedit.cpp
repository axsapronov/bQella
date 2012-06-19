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
#include "raedit.h"
#include <qapplication.h>
#include <qevent.h>
#include <qdesktopwidget.h>
#include <qdebug.h>
#include <qabstracttextdocumentlayout.h>
#include <qtextcodec.h>
#include <qpainter.h>
#include <qdir.h>
#include <qwhatsthis.h>
#include <qtextobject.h>
#include <qdesktopservices.h>

static bool isAbsoluteFileName(const QString &name)
{
    return !name.isEmpty()
           && (name[0] == QLatin1Char('/')
				#if defined(Q_WS_WIN)
               || (name[0].isLetter() && name[1] == QLatin1Char(':')) || name.startsWith(QLatin1String("\\\\"))
				#endif
               || (name[0]  == QLatin1Char(':') && name[1] == QLatin1Char('/'))
              );
}

//====================== class raEdit ============================
raEdit::raEdit(QWidget *parent)
{
	doc = new QTextDocument();
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setReadOnly(false);
    setModeHtml(true);
    QObject::connect(document(), SIGNAL(contentsChanged()),this, SLOT(_q_documentModified()));
    //QObject::connect(document(), SIGNAL(linkActivated(QString)),this, SLOT(_q_activateAnchor(QString)));
    //QObject::connect(document(), SIGNAL(linkHovered(QString)),this, SLOT(_q_highlightLink(QString)));
}

raEdit::~raEdit()
{
}

QString raEdit::findFile(const QUrl &name) const
{
    QString fileName;
    if (name.scheme() == QLatin1String("qrc"))
        fileName = QLatin1String(":/") + name.path();
    else
        fileName = name.toLocalFile();

    if (isAbsoluteFileName(fileName))
        return fileName;

    foreach (QString path, searchPaths) {
        if (!path.endsWith(QLatin1Char('/')))
            path.append(QLatin1Char('/'));
        path.append(fileName);
        if (QFileInfo(path).isReadable())
            return path;
    }

    return fileName;
}

QUrl raEdit::resolveUrl(const QUrl &url) const
{
    if (!url.isRelative())
        return url;

    // For the second case QUrl can merge "#someanchor" with "foo.html"
    // correctly to "foo.html#someanchor"
    if (!(currentURL.isRelative()
          || (currentURL.scheme() == QLatin1String("file")
              && !isAbsoluteFileName(currentURL.toLocalFile())))
          || (url.hasFragment() && url.path().isEmpty())) {
        return currentURL.resolved(url);
    }

    // this is our last resort when current url and new url are both relative
    // we try to resolve against the current working directory in the local
    // file system.
    QFileInfo fi(currentURL.toLocalFile());
	//qDebug() << "opening file raEdit::resolvUrl: " << currentURL.toLocalFile();
    if (fi.exists()) {
        return QUrl::fromLocalFile(fi.absolutePath() + QDir::separator()).resolved(url);
    }

    return url;
}

void raEdit::_q_activateAnchor(const QString &href)
{
    if (href.isEmpty())
        return;
/*
#ifndef QT_NO_CURSOR
    viewport->setCursor(oldCursor);
#endif
*/
    const QUrl url = resolveUrl(href);

    if (!openLinks) {
        emit anchorClicked(url);
        return;
    }

    textOrSourceChanged = false;

#ifndef QT_NO_DESKTOPSERVICES
    if (openExternalLinks
        && url.scheme() != QLatin1String("file")
        && url.scheme() != QLatin1String("qrc")) {
        QDesktopServices::openUrl(url);
        return;
    }
#endif

    emit anchorClicked(url);

    if (textOrSourceChanged)
        return;

    setSource(url);
}

void raEdit::_q_highlightLink(const QString &anchor)
{
	//-pm- viewport->setCursor(oldCursor);
	qDebug() << "mouse over link";
    if (anchor.isEmpty()) {
        emit highlighted(QUrl());
        emit highlighted(QString());
    } else {
        const QUrl url = resolveUrl(anchor);
        emit highlighted(url);
        // convenience to ease connecting to QStatusBar::showMessage(const QString &)
        emit highlighted(url.toString());
    }
}

void raEdit::setSource(const QUrl &url)
{
    textOrSourceChanged = true;

    QString txt;

    bool doSetText = false;

    QUrl currentUrlWithoutFragment = currentURL;
    currentUrlWithoutFragment.setFragment(QString());
    QUrl newUrlWithoutFragment = currentURL.resolved(url);
    newUrlWithoutFragment.setFragment(QString());

    if (url.isValid()
    	 && (newUrlWithoutFragment != currentUrlWithoutFragment || forceLoadOnSourceChange)) {
        QVariant data = loadResource(QTextDocument::HtmlResource, resolveUrl(url));
        if (data.type() == QVariant::String) {
            txt = data.toString();
        } else if (data.type() == QVariant::ByteArray) {
#ifndef QT_NO_TEXTCODEC
            QByteArray ba = data.toByteArray();
            QTextCodec *codec = Qt::codecForHtml(ba);
            txt = codec->toUnicode(ba);
#else
         txt = data.toString();
#endif
        }
        if (txt.isEmpty())
            qWarning("raEdit: No document for %s", url.toString().toLatin1().constData());
/*!+! Загруженный текстовый файл - это сноска (примечание)
        if (isVisible()) {
            QString firstTag = txt.left(txt.indexOf(QLatin1Char('>')) + 1);
            if (firstTag.left(3) == QLatin1String("<qt") && firstTag.contains(QLatin1String("type")) && firstTag.contains(QLatin1String("detail"))) {
#ifndef QT_NO_WHATSTHIS
                QWhatsThis::showText(QCursor::pos(), txt, q);
#endif
                return;
            }
        }*/
        currentURL = resolveUrl(url);
        doSetText = true;
    }
    if (doSetText) {
        	QTextEdit::setHtml(txt);	
/* !+! for plain text mode
qDebug() << "--modeHTML= " << modeHtml;
    	if (modeHtml){
        	QTextEdit::setHtml(txt);
qDebug() << "--setHTML";
        }else{
        	QTextEdit::setPlainText(txt);
qDebug() << "--setPlainText";
       	}*/
    }
    forceLoadOnSourceChange = false;
/*
    if (!url.fragment().isEmpty()) {
        scrollToAnchor(url.fragment());
    } else {
//        hbar->setValue(0);
//        vbar->setValue(0);
    }*/
//    qDebug() << "--emit sourceChanged(url): " << url;
    emit sourceChanged(url);
}

/*!
    \property raEdit::source
    \brief the name of the displayed document.

    This is a an invalid url if no document is displayed or if the
    source is unknown.

    When setting this property raEdit tries to find a document
    with the specified name in the paths of the searchPaths property
    and directory of the current source, unless the value is an absolute
    file path. It also checks for optional anchors and scrolls the document
    accordingly

!!-> If the first tag in the document is \c{<qt type=detail>}, the
    document is displayed as a popup rather than as new document in
    the browser window itself. Otherwise, the document is displayed
    normally in the text browser with the text set to the contents of
    the named document with setHtml().
*/
QUrl raEdit::source() const
{
    return currentURL;
}

void raEdit::setSearchPaths(const QStringList &paths)
{
    searchPaths = paths;
}

/*!
    Reloads the current set source.
*/
void raEdit::reload()
{
    QUrl s = currentURL;
    currentURL = QUrl();
    setSource(s);
}

//   \reimp
void raEdit::mouseMoveEvent(QMouseEvent *e) 
{ 
//qDebug() << ":: mouse move event";
	if (e->modifiers() && Qt::ControlModifier){
		//!+! change cursor on hyperlink 
		QWidget::setCursor(Qt::PointingHandCursor);
	}else
		QWidget::setCursor(Qt::IBeamCursor);
			// set QObject::installEventFilter
	QTextEdit::mouseMoveEvent(e); 
}

void raEdit::mouseReleaseEvent(QMouseEvent *e){ QTextEdit::mouseReleaseEvent(e); }

void raEdit::focusOutEvent(QFocusEvent *ev) { QTextEdit::focusOutEvent(ev); }

bool raEdit::event(QEvent *e) { return QTextEdit::event(e); }

void raEdit::keyPressEvent(QKeyEvent *ev) { QTextEdit::keyPressEvent(ev); }

void raEdit::mousePressEvent(QMouseEvent *e)
{ 
	//if (e == QEvent::MouseButtonPress)... 	
	QTextEdit::mousePressEvent(e); 	
}

/*!
    \reimp
*//*
void raEdit::paintEvent(QPaintEvent *e)
{
    Q_D(raEdit);
    QPainter p(d->viewport);
    d->paint(&p, e);
}
*/
/*!
    This function is called when the document is loaded. The \a type
    indicates the type of resource to be loaded. For each image in
    the document, this function is called once.

    The default implementation ignores \a type and tries to locate
    the resources by interpreting \a name as a file name. If it is
    not an absolute path it tries to find the file in the paths of
    the \l searchPaths property and in the same directory as the
    current source. On success, the result is a QVariant that stores
    a QByteArray with the contents of the file.

    If you reimplement this function, you can return other QVariant
    types. The table below shows which variant types are supported
    depending on the resource type:

    \table
    \header \i ResourceType  \i QVariant::Type
    \row    \i QTextDocument::HtmlResource  \i QString or QByteArray
    \row    \i QTextDocument::ImageResource \i QImage, QPixmap or QByteArray
    \row    \i QTextDocument::StyleSheetResource \i QString or QByteArray
    \endtable
*/
QVariant raEdit::loadResource(int /*type*/, const QUrl &name)
{
    QByteArray data;
    QString fileName = findFile(resolveUrl(name));
    QFile f(fileName);
    if (f.open(QFile::ReadWrite)) {
        data = f.readAll();
        f.close();
    } else {
        qWarning("Cannot open resource '%s' for reading", name.toString().toLocal8Bit().data());
        return QVariant();
    }
    return data;
}
