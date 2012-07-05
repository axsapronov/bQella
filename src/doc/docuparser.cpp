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

#include "docuparser.h"
//#include "profile.h"
#include "config.h"
#include "pcommon.h"



//-------------------------------------------------
QDataStream &operator>>(QDataStream &s, ContentItem &ci)
{
    s >> ci.title;
    s >> ci.reference;
    s >> ci.depth;
    return s;
}

//-------------------------------------------------
QDataStream &operator<<(QDataStream &s, const ContentItem &ci)
{
    s << ci.title;
    s << ci.reference;
    s << ci.depth;
    return s;
}

//-------------------------------------------------
DocuParser *DocuParser::createParser(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) {
        return 0;
    }
	
    return new DocuParserRAP;
}

//===================== class DocuParser =====================
bool DocuParser::parse(QFile *file)
{
    QXmlInputSource source(file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    fname = QFileInfo(*file).absoluteFilePath();
    return reader.parse(source);
}

//-------------------------------------------------
QString DocuParser::errorProtocol() const {    return errorProt; }

QList<ContentItem> DocuParser::getContentItems(){  return contentList; }
QList<IndexItem*> DocuParser::getIndexItems(){    return indexList; }

DocuParserRAP::DocuParserRAP()
    : prof(new Profile)
{
}

//===================== class DocuParserRAP =====================
bool DocuParserRAP::startDocument()
{
    state = StateInit;
    errorProt = QString("");
    contentRef = QString("");
    indexRef = QString("");
	indexKey = QString("");
    depth = 0;
    contentList.clear();
    indexList.clear();

    return true;
}

//-------------------------------------------------
bool DocuParserRAP::startElement(const QString &, const QString &, 
								 const QString &qname, const QXmlAttributes &attr)
{
//    QString lower = qname.toLower();
    QString lower = qname;

    switch(state) {

    case StateInit:
        if(lower == QString("pemproject"))
			state = StateProfile;
        break;

	case StateProfile:
        if(lower == QString("property")) {
            state = StateProperty;
            propertyName = attr.value(QString("name"));
        }
        break;

	case StateProperty: //values are set at the endElement tag </property>
        break;

	case StateContents: 
		if(lower == QString("contents")) 
			state = StateSection;
        break;

    case StateSection:
        if(lower == QString("section")) {
            docTitle = attr.value(QString("title"));
            contentRef = urlifyFileName(absolutifyFileName(attr.value(QString("ref")), Config::configuration() -> CurPrjDir()));
            iconFileName = absolutifyFileName(attr.value(QString("icon")),Config::configuration() -> CurPrjDir());
			contentList.append(ContentItem(docTitle, contentRef, iconFileName, depth));
			depth++; 
        }
        break;

	case StateKeywords: 
        if (lower == QString("keyword")) {
            indexRef = absolutifyFileName(attr.value(QString("ref")), Config::configuration() -> CurPrjDir());
			indexKey = attr.value(QString("key"));
			state = StateKeyword;
        } 
        break;

	case StateKeyword:
		break;

    default:
        break;
    }

    return true;
}

//-------------------------------------------------
bool DocuParserRAP::endElement(const QString &nameSpace, const QString &localName, const QString &qName)
{
    Q_UNUSED(nameSpace);
    Q_UNUSED(localName);
    Q_UNUSED(qName);

    switch(state) {
    case StateInit:
        break;
    case StateProfile:
		state = StateContents;
        break;
    case StateProperty:
        state = StateProfile;
        if(propertyName.isEmpty() || propertyValue.isEmpty()){
            return false;
        }else{
            static const QStringList lst = QStringList()  // lst is a list of properties with file names
				<< QString("startpage");
            if (lst.contains(propertyName)){ //see if propertyValue is a file name then convert string to full path name
                propertyValue = absolutifyFileName(propertyValue, Config::configuration() -> CurPrjDir());
			}
        }
        prof -> addProperty(propertyName, propertyValue);
        break;
	case StateSection:
        if(contentRef.isEmpty())  return false;
		depth--;
		break;
	case StateContents:
		state = StateKeywords; 
        break;
    case StateKeyword:
		state = StateKeywords;
		if(indexRef.isEmpty())  return false;
        break;
    case StateKeywords:
        state = StateInit;
        break;
    }
    return true;
}

//-------------------------------------------------
bool DocuParserRAP::characters(const QString& ch)
{
    QString str = ch.simplified();
    if (str.isEmpty())
        return true;

    switch (state) {
    case StateInit:
    case StateContents:
    case StateSection:
    case StateKeywords:
        return false; //no value for these tags
        break;
    case StateKeyword:
        indexList.append(new IndexItem(str, indexRef)); // !+! need to add indexKey for future use
        break;
    case StateProperty:
        propertyValue = ch;
        break;
    default:
        return false;
    }
    return true;
}

//-------------------------------------------------
bool DocuParserRAP::fatalError(const QXmlParseException& exception)
{
    errorProt += QString::fromLatin1("fatal parsing error: %1 in line %2, column %3\n")
        .arg(exception.message())
        .arg(exception.lineNumber())
        .arg(exception.columnNumber());

    return QXmlDefaultHandler::fatalError(exception);
}
