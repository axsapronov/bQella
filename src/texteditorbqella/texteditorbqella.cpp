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


#include "texteditorbqella.h"
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

#include <QTextCursor>
#include <QTextBlock>
#include <QMenu>
#include <QContextMenuEvent>
#include <QFileInfo>
#include <QTextCodec>
#include <QSettings>
#include <QTextStream>

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

//====================== class TextEditorBQella ============================
TextEditorBQella::TextEditorBQella(QWidget *parent, QString SpellDic)
{
    Q_UNUSED(parent)
    docText = new QTextDocument();
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setReadOnly(false);
    setModeHtml(true);
    QObject::connect(document(), SIGNAL(contentsChanged()),this, SLOT(_q_documentModified()));
    //QObject::connect(document(), SIGNAL(linkActivated(QString)),this, SLOT(_q_activateAnchor(QString)));
    //QObject::connect(document(), SIGNAL(linkHovered(QString)),this, SLOT(_q_highlightLink(QString)));

    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this->document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    //    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    setWordWrapMode(QTextOption::NoWrap);
    loadSettings();

    createActions();
    // create misspell actions in context menu
    spell_dic = SpellDic.left(SpellDic.length()-4);
    pChecker = new Hunspell(spell_dic.toLatin1()+".aff",spell_dic.toLatin1()+".dic");

    QFileInfo fi(SpellDic);
    if (!(fi.exists() && fi.isReadable()))
    {
        delete pChecker;
        pChecker = 0;
    }
    // get user config dictionary
    QSettings setting;
    QString filePath = QFileInfo(setting.fileName()).absoluteFilePath();
    filePath = filePath + "/User_" + QFileInfo(spell_dic.toLatin1()+".dic").fileName();
    qDebug() << "ra3" << qPrintable(filePath);
    fi = QFileInfo(filePath);
    if (fi.exists() && fi.isReadable())
    {
        pChecker->add_dic(filePath.toLatin1());
    }
    else filePath = "";
    addedWords.clear();
}
//------------------------------------------------------------------------------
TextEditorBQella::~TextEditorBQella()
{

    // write user dictionary
    QSettings setting;
    QString fileName = QFileInfo(setting.fileName()).absoluteFilePath();
    fileName = fileName + "/User_" + QFileInfo(spell_dic.toLatin1()+".dic").fileName();
    qDebug() << "raE2" << qPrintable(fileName);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        in.readLine();
        while (!in.atEnd()) {
            QString line = in.readLine();
            if(!addedWords.contains(line)) addedWords << line;
        }
        file.close();
    }
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "write";

        QTextStream out(&file);
        QByteArray encodedString;
        QString spell_encoding=QString(pChecker->get_dic_encoding());
        QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
        out << addedWords.count() << "\n";
        foreach(QString elem, addedWords){
            encodedString = codec->fromUnicode(elem);
            out << encodedString.data() << "\n";
            //            std::cout << encodedString.data() << std::endl;
        }
    }
}
//------------------------------------------------------------------------------
QString TextEditorBQella::findFile(const QUrl &name) const
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
//------------------------------------------------------------------------------
QUrl TextEditorBQella::resolveUrl(const QUrl &url) const
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
    //qDebug() << "opening file TextEditorBQella::resolvUrl: " << currentURL.toLocalFile();
    if (fi.exists()) {
        return QUrl::fromLocalFile(fi.absolutePath() + QDir::separator()).resolved(url);
    }

    return url;
}
//------------------------------------------------------------------------------
void TextEditorBQella::_q_activateAnchor(const QString &href)
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
//------------------------------------------------------------------------------
void TextEditorBQella::_q_highlightLink(const QString &anchor)
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
//------------------------------------------------------------------------------
void TextEditorBQella::setSource(const QUrl &url)
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
//------------------------------------------------------------------------------
/*!
    \property TextEditorBQella::source
    \brief the name of the displayed document.

    This is a an invalid url if no document is displayed or if the
    source is unknown.

    When setting this property TextEditorBQella tries to find a document
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
QUrl TextEditorBQella::source() const
{
    return currentURL;
}
//------------------------------------------------------------------------------
void TextEditorBQella::setSearchPaths(const QStringList &paths)
{
    searchPaths = paths;
}
//------------------------------------------------------------------------------
/*!
    Reloads the current set source.
*/
void TextEditorBQella::reload()
{
    QUrl s = currentURL;
    currentURL = QUrl();
    setSource(s);
}
//------------------------------------------------------------------------------
//   \reimp
void TextEditorBQella::mouseMoveEvent(QMouseEvent *e)
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
//------------------------------------------------------------------------------
void TextEditorBQella::mouseReleaseEvent(QMouseEvent *e){ QTextEdit::mouseReleaseEvent(e); }
//------------------------------------------------------------------------------
void TextEditorBQella::focusOutEvent(QFocusEvent *ev) { QTextEdit::focusOutEvent(ev); }
//------------------------------------------------------------------------------
bool TextEditorBQella::event(QEvent *e) { return QTextEdit::event(e); }
//------------------------------------------------------------------------------
void TextEditorBQella::keyPressEvent(QKeyEvent *ev)
{
    QTextEdit::keyPressEvent(ev);

    //    if (ev->key() == Qt::Key_Return)
    //    {
    //        qDebug() << "ENTER PRESS";
    //        QTextEdit::append("LOL");
    //    }
}
//------------------------------------------------------------------------------
void TextEditorBQella::mousePressEvent(QMouseEvent *e)
{
    //if (e == QEvent::MouseButtonPress)...
    QTextEdit::mousePressEvent(e);
}
//------------------------------------------------------------------------------
/*!
    \reimp
*//*
void TextEditorBQella::paintEvent(QPaintEvent *e)
{
    Q_D(TextEditorBQella);
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
QVariant TextEditorBQella::loadResource(int /*type*/, const QUrl &name)
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
bool TextEditorBQella::canInsertFromMimeData( const QMimeData *source ) const
{
    if (source->hasImage() && Config::configuration()->AcceptDropImages())
        return true;
    else
        return QTextEdit::canInsertFromMimeData(source);
}

//------------------------------------------------------------------------------
void TextEditorBQella::insertFromMimeData( const QMimeData *source )
{
    if (source->hasImage() && Config::configuration()->AcceptDropImages())
    {
        QImage image = qvariant_cast<QImage>(source->imageData());
        emit insertImageFromClipboard(image);
    }
    else if (source->hasHtml())
    {
        emit insertHtmlFromClipboard(source->html());
    }
    else
    {
        QTextEdit::insertFromMimeData(source);
    }
}
//------------------------------------------------------------------------------
QPoint TextEditorBQella::getCursorLocation()
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
void TextEditorBQella::setCursorLocation( QPoint p )
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
void TextEditorBQella::setCursorLocation( int x, int y )
{
    setCursorLocation( QPoint(x,y) );
}
//------------------------------------------------------------------------------
uint TextEditorBQella::getLineCount()
{
    QTextDocument *doc = docText;
    QTextBlock block = doc->begin();
    int y=1;

    for( block=doc->begin(); block!= doc->end(); block = block.next())
        y++;
    return y;
}
//------------------------------------------------------------------------------
int TextEditorBQella::lineNumberAreaWidth()
{
    int digits = 1;
//    qDebug() << "==test==\n"
//             << docText->blockCount()
//             << this->document()->blockCount();
    int max = qMax(1, this->document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}
//------------------------------------------------------------------------------
void TextEditorBQella::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
//------------------------------------------------------------------------------
void TextEditorBQella::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
//------------------------------------------------------------------------------
void TextEditorBQella::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
//------------------------------------------------------------------------------
void TextEditorBQella::highlightCurrentLine()
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
void TextEditorBQella::loadSettings()
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
void TextEditorBQella::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = getFirstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            QSettings settings("CuteNotes", "CuteNotes");
            painter.setPen(QColor(settings.value("lineNumberColor").toString()));
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

//------------------------------------------------------------------------------
bool TextEditorBQella::setDict(const QString SpellDic)
{
    if(SpellDic!=""){
        //mWords.clear();
        spell_dic=SpellDic.left(SpellDic.length()-4);
        delete pChecker;
        pChecker = new Hunspell(spell_dic.toLatin1()+".aff",spell_dic.toLatin1()+".dic");
    }
    else spell_dic="";

    QFileInfo fi(SpellDic);
    if (!(fi.exists() && fi.isReadable())){
        delete pChecker;
        pChecker=0;
    }

    // get user config dictionary
    QSettings setting;
    QString filePath = QFileInfo(setting.fileName()).absoluteFilePath();
    filePath = filePath + "/User_"+QFileInfo(spell_dic.toLatin1()+".dic").fileName();
    qDebug() << "raE1" << qPrintable(filePath);
    fi=QFileInfo(filePath);
    if (fi.exists() && fi.isReadable()){
        pChecker->add_dic(filePath.toLatin1());
    }
    else filePath="";

    return (spell_dic!="");
}
//------------------------------------------------------------------------------
void TextEditorBQella::createActions()
{
    for (int i = 0; i < MaxWords; ++i)
    {
        misspelledWordsActs[i] = new QAction(this);
        misspelledWordsActs[i]->setVisible(false);
        connect(misspelledWordsActs[i], SIGNAL(triggered()), this, SLOT(correctWord()));
    }
}
//------------------------------------------------------------------------------
void TextEditorBQella::correctWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString replacement = action->text();
        QTextCursor cursor = cursorForPosition(lastPos);
        //QTextCursor cursor = textCursor();
        QString zeile = cursor.block().text();
        cursor.select(QTextCursor::WordUnderCursor);
        cursor.deleteChar();
        cursor.insertText(replacement);
    }
}
//------------------------------------------------------------------------------
void TextEditorBQella::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    lastPos=event->pos();
    QTextCursor cursor = cursorForPosition(lastPos);
    QString zeile = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = zeile.indexOf(QRegExp("\\W+"),pos);
    int begin = zeile.lastIndexOf(QRegExp("\\W+"),pos);
    zeile=zeile.mid(begin+1,end-begin-1);
    QStringList liste = getWordPropositions(zeile);
    if (!liste.isEmpty())
    {
        menu->addSeparator();
        QAction *a;
        a = menu->addAction(tr("Add .."), this, SLOT(slot_addWord()));
        a = menu->addAction(tr("Ignore .."), this, SLOT(slot_ignoreWord()));
        for (int i = 0; i < qMin(int(MaxWords),liste.size()); ++i) {
            misspelledWordsActs[i]->setText(liste.at(i).trimmed());
            misspelledWordsActs[i]->setVisible(true);
            menu->addAction(misspelledWordsActs[i]);
        }

    } // if  misspelled
    menu->exec(event->globalPos());
    delete menu;
}
//------------------------------------------------------------------------------
QStringList TextEditorBQella::getWordPropositions(const QString word)
{
    QStringList wordList;
    if(pChecker)
    {
        QByteArray encodedString;
        QString spell_encoding=QString(pChecker->get_dic_encoding());
        QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
        encodedString = codec->fromUnicode(word);
        bool check = pChecker->spell(encodedString.data());
        if(!check){
            char ** wlst;
            int ns = pChecker->suggest(&wlst,encodedString.data());
            if (ns > 0)
            {
                for (int i=0; i < ns; i++)
                {
                    wordList.append(codec->toUnicode(wlst[i]));
                    //free(wlst[i]);
                }
                //free(wlst);
                pChecker->free_list(&wlst, ns);
            }// if ns >0
        }// if check
    }
    return wordList;
}
//------------------------------------------------------------------------------
void TextEditorBQella::slot_addWord()
{
    QTextCursor cursor = cursorForPosition(lastPos);
    QString zeile = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = zeile.indexOf(QRegExp("\\W+"),pos);
    int begin = zeile.left(pos).lastIndexOf(QRegExp("\\W+"),pos);
    zeile = zeile.mid(begin + 1, end-begin - 1);
    qDebug() << "raEdit2" << qPrintable(zeile);
    QByteArray encodedString;
    QString spell_encoding = QString(pChecker->get_dic_encoding());
    QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
    encodedString = codec->fromUnicode(zeile);
    pChecker->add(encodedString.data());
    addedWords.append(zeile);
    emit addWord(zeile);
}
//------------------------------------------------------------------------------
void TextEditorBQella::slot_ignoreWord()
{
    QTextCursor cursor = cursorForPosition(lastPos);
    QString zeile = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = zeile.indexOf(QRegExp("\\W+"),pos);
    int begin = zeile.left(pos).lastIndexOf(QRegExp("\\W+"),pos);
    zeile=zeile.mid(begin+1,end-begin-1);
    QByteArray encodedString;
    QString spell_encoding=QString(pChecker->get_dic_encoding());
    QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
    encodedString = codec->fromUnicode(zeile);
    pChecker->add(encodedString.data());
    emit addWord(zeile);
}
//------------------------------------------------------------------------------
int TextEditorBQella::getCountFirstVisibleBlock()
{
    QTextDocument *pDoc = docText;
    int nFirstVisible = 0;
    for(QTextBlock block = pDoc->begin(); block != pDoc->end(); block = block.next())
    {
        nFirstVisible += 1;
        if (block.isVisible())
        {
            return nFirstVisible;
        }
    }
    return nFirstVisible;
}
//------------------------------------------------------------------------------
QTextBlock TextEditorBQella::getFirstVisibleBlock()
{
    QTextDocument *pDoc = this->document();
    //    int nFirstVisible = 0;
    for(QTextBlock block = pDoc->begin(); block != pDoc->end(); block = block.next())
    {
        //        nFirstVisible += 1;
        if (block.isVisible())
        {
            return block;
        }
    }
    QTextBlock block;
    return block;
}
//------------------------------------------------------------------------------
QRectF TextEditorBQella::blockBoundingGeometry(QTextBlock &block)
{
    if(block.isValid())
    {
        QRectF rect = block.layout()->boundingRect().translated(block.layout()->position());
        //        qDebug() << "block" << block.blockNumber() << ": " << rect;
        return rect;
    }
    else
        return QRectF();
}
//------------------------------------------------------------------------------
QPointF TextEditorBQella::contentOffset()
{
    //    Q_D(const QPlainTextEdit);
    //    return QPointF(-d->horizontalOffset(), -d->verticalOffset());
    return QPoint(0,0);
}
//------------------------------------------------------------------------------
QRectF TextEditorBQella::blockBoundingRect(QTextBlock &block)
{
    if(block.isValid())
    {
        QRectF rect = block.layout()->boundingRect();
        //        qDebug() << "block" << block.blockNumber() << ": " << rect;
        return rect;
    }
    else
        return QRectF();
}
//------------------------------------------------------------------------------
//TextEditorBQella::SpellTextEdit(QWidget *parent,QString SpellDic)
//    : QTextEdit(parent)
//{
//    createActions();
//    // create misspell actions in context menu
//    spell_dic=SpellDic.left(SpellDic.length()-4);
//    pChecker = new Hunspell(spell_dic.toLatin1()+".aff",spell_dic.toLatin1()+".dic");

//    QFileInfo fi(SpellDic);
//    if (!(fi.exists() && fi.isReadable())){
//        delete pChecker;
//        pChecker=0;
//    }
//    // get user config dictionary
//    QSettings setting;
//    QString filePath=QFileInfo(setting.fileName()).absoluteFilePath();
//    filePath=filePath+"/User_"+QFileInfo(spell_dic.toLatin1()+".dic").fileName();
//    std::cout << qPrintable(filePath) << std::endl;
//    fi=QFileInfo(filePath);
//    if (fi.exists() && fi.isReadable()){
//        pChecker->add_dic(filePath.toLatin1());
//    }
//    else filePath="";


//    addedWords.clear();
//}
////------------------------------------------------------------------------------
//TextEditorBQella::~SpellTextEdit()
//{
//    // write user dictionary
//    QSettings setting;
//    QString fileName=QFileInfo(setting.fileName()).absoluteFilePath();
//    fileName=fileName+"/User_"+QFileInfo(spell_dic.toLatin1()+".dic").fileName();
//    std::cout << qPrintable(fileName) << std::endl;
//    QFile file(fileName);
//    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        QTextStream in(&file);
//        in.readLine();
//        while (!in.atEnd()) {
//            QString line = in.readLine();
//            if(!addedWords.contains(line)) addedWords << line;
//        }
//        file.close();
//    }
//    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
//    {
//        std::cout << "write" << std::endl;

//        QTextStream out(&file);
//        QByteArray encodedString;
//        QString spell_encoding=QString(pChecker->get_dic_encoding());
//        QTextCodec *codec = QTextCodec::codecForName(spell_encoding.toLatin1());
//        out << addedWords.count() << "\n";
//        foreach(QString elem, addedWords){
//            encodedString = codec->fromUnicode(elem);
//            out << encodedString.data() << "\n";
//            std::cout << encodedString.data() << std::endl;
//        }
//    }
//}
