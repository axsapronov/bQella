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


#include "raedit.h"
#include "config.h"

#include <qapplication.h>
#include <qevent.h>
#include <qdesktopwidget.h>
#include <QtDebug>
#include <qabstracttextdocumentlayout.h>
#include <qtextcodec.h>
#include <qpainter.h>
#include <QDir>
#include <qwhatsthis.h>
#include <qtextobject.h>
#include <qdesktopservices.h>



static bool isAbsoluteFileName(const QString &name)
{
    return !name.isEmpty()
            && (name[0] == QChar('/')
            #if defined(Q_WS_WIN)
                || (name[0].isLetter() && name[1] == QChar(':')) || name.startsWith(QString("\\\\"))
            #endif
                || (name[0]  == QChar(':') && name[1] == QChar('/'))
                );
}

//====================== class raEdit ============================
raEdit::raEdit(QWidget *parent)
{
    docText = new QTextDocument();
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setReadOnly(false);
    setModeHtml(true);
    QObject::connect(document(), SIGNAL(contentsChanged()),this, SLOT(_q_documentModified()));
    //QObject::connect(document(), SIGNAL(linkActivated(QString)),this, SLOT(_q_activateAnchor(QString)));
    //QObject::connect(document(), SIGNAL(linkHovered(QString)),this, SLOT(_q_highlightLink(QString)));

    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    setWordWrapMode(QTextOption::NoWrap);
    loadSettings();
}

raEdit::~raEdit()
{
}

QString raEdit::findFile(const QUrl &name) const
{
    QString fileName;
    if (name.scheme() == QString("qrc"))
        fileName = QString(":/") + name.path();
    else
        fileName = name.toLocalFile();

    if (isAbsoluteFileName(fileName))
        return fileName;

    foreach (QString path, searchPaths) {
        if (!path.endsWith(QChar('/')))
            path.append(QChar('/'));
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
          || (currentURL.scheme() == QString("file")
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
    viewport -> setCursor(oldCursor);
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
            && url.scheme() != QString("file")
            && url.scheme() != QString("qrc")) {
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
    //-pm- viewport -> setCursor(oldCursor);
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
    //здесь
    QString txt;

    bool doSetText = false;

    QUrl currentUrlWithoutFragment = currentURL;
    currentUrlWithoutFragment.setFragment(QString());
    QUrl newUrlWithoutFragment = currentURL.resolved(url);
    newUrlWithoutFragment.setFragment(QString());

    if (url.isValid()
            && (newUrlWithoutFragment != currentUrlWithoutFragment || forceLoadOnSourceChange))
    {
        //        QVariant data = loadResource(QTextDocument::UserResource, resolveUrl(url));
        QVariant data = loadResource(QTextDocument::HtmlResource, resolveUrl(url));
        if (data.type() == QVariant::String)
        {
            txt = data.toString();
        }
        else if (data.type() == QVariant::ByteArray)
        {
#ifndef QT_NO_TEXTCODEC
            QByteArray ba = data.toByteArray();
            QTextCodec *codec = Qt::codecForHtml(ba);
            txt = codec -> toUnicode(ba);
#else
            txt = data.toString();
#endif
        }
        if (txt.isEmpty())
            qWarning("Warning: _raEdit::setSource(): No document for %s", url.toString().toUtf8().constData());
        /* load text file
        if (isVisible()) {
            QString firstTag = txt.left(txt.indexOf(QChar('>')) + 1);
            if (firstTag.left(3) == QString("<qt") && firstTag.contains(QString("type")) && firstTag.contains(QString("detail"))) {
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
//        hbar -> setValue(0);
//        vbar -> setValue(0);
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

!! ->  If the first tag in the document is \c{<qt type=detail>}, the
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
    if (e -> modifiers() && Qt::ControlModifier){
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

void raEdit::keyPressEvent(QKeyEvent *ev)
{
    QTextEdit::keyPressEvent(ev);

    //    if (ev->key() == Qt::Key_Return)
    //    {
    //        qDebug() << "ENTER PRESS";
    //        QTextEdit::append("LOL");
    //    }
}

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
    QPainter p(d -> viewport);
    d -> paint(&p, e);
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
    //    qDebug() << "filename KNJM = " << fileName;
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

//------------------------------------------------------------------------------
bool raEdit::canInsertFromMimeData( const QMimeData *source ) const
{
    if (source->hasImage() && Config::configuration()->AcceptDropImages())
        return true;
    else
        return QTextEdit::canInsertFromMimeData(source);
}

//------------------------------------------------------------------------------
void raEdit::insertFromMimeData( const QMimeData *source )
{
    if (source->hasImage() && Config::configuration()->AcceptDropImages()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        emit insertImageFromClipboard(image);
    }else if (source->hasHtml()) {
        emit insertHtmlFromClipboard(source->html());
    }else{
        QTextEdit::insertFromMimeData(source);
    }
}









//------------------------------------------------------------------------------
QPoint raEdit::getCursorLocation()
{
    QTextDocument *doc = docText;
    QTextBlock block = doc->begin();
    int cursorOffset = this->textCursor().position();
    int off = 0;
    int x=1, y=1;

    while( cursorOffset>=(off + block.length()) )
    {
        off += block.length();
        block = block.next();
        y+= 1;
    }
    x = cursorOffset - off +1;

    return QPoint( x, y );
}
//------------------------------------------------------------------------------
void raEdit::setCursorLocation( QPoint p )
{
    QTextDocument *doc = docText;
    QTextBlock block = doc->begin();
    int off = 0;
    int y=1;

    for( ; y<p.y(); y++ )
    {
        off += block.length();

        if (block != doc->end())
            block = block.next();
        else
            return;
    }

    off+= p.x();

    QTextCursor c = this->textCursor();
    c.setPosition( off );
    this->setTextCursor( c );
}
//------------------------------------------------------------------------------
void raEdit::setCursorLocation( int x, int y )
{
    setCursorLocation( QPoint(x,y) );
}
//------------------------------------------------------------------------------
uint raEdit::getLineCount()
{
    QTextDocument *doc = docText;
    QTextBlock block = doc->begin();
    int y=1;

    for( block=doc->begin(); block!= doc->end(); block = block.next())
        y++;
    return y;
}
//------------------------------------------------------------------------------
int raEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, docText->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}
//------------------------------------------------------------------------------
void raEdit::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
//------------------------------------------------------------------------------
void raEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
//------------------------------------------------------------------------------
void raEdit::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
//------------------------------------------------------------------------------
void raEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;
        QSettings settings("CuteNotes", "CuteNotes");
        QColor color = settings.value("lineHighlightColor").toString().isEmpty() ?
                    QColor(Qt::lightGray) : settings.value("lineHighlightColor").toString();
        QColor lineColor = color;

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}
//------------------------------------------------------------------------------
void raEdit::loadSettings()
{
    QSettings settings("bQella", "bQella");
    QFont font;
    font.fromString(settings.value("Font").toString());
    setStyleSheet("color:" + settings.value("fontColor").toString());
    setFont(font);
    //This calls the necessary functions to update CodeEditor
    highlightCurrentLine();
}
//------------------------------------------------------------------------------
void raEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
//    QPainter painter(lineNumberArea);
//    painter.fillRect(event->rect(), Qt::lightGray);
//    QTextBlock block = firstVisibleBlock();
//    int blockNumber = block.blockNumber();
//    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
//    int bottom = top + (int) blockBoundingRect(block).height();
//    while (block.isValid() && top <= event->rect().bottom()) {
//        if (block.isVisible() && bottom >= event->rect().top()) {
//            QString number = QString::number(blockNumber + 1);
//            QSettings settings("CuteNotes", "CuteNotes");
//            painter.setPen(QColor(settings.value("lineNumberColor").toString()));
//            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
//                             Qt::AlignRight, number);
//        }

//        block = block.next();
//        top = bottom;
//        bottom = top + (int) blockBoundingRect(block).height();
//        ++blockNumber;
//    }
}
//------------------------------------------------------------------------------
