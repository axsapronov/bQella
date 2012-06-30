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
#include "srclistparser.h"
#include "pcommon.h"

#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QtXml>
#include <QtDebug> //to use qWarning and qDebug messages

//===================== class SrcListParser =====================
SrcListParser::SrcListParser()
{
}

//-------------------------------------------------
SrcListParser *SrcListParser::createParser(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)) {
        return 0;
    }
	SrcListParser *parser = new SrcListParser;
   	parser -> setFileName(fileName);
    return parser; 
}

//-------------------------------------------------
bool SrcListParser::parse()
{	
	QFile file(fName);
	QXmlInputSource source(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
	bool p = reader.parse(source);
    return p;
}

//-------------------------------------------------
bool SrcListParser::startDocument()
{
    errorString = QString("");
    srcID = 0;	srcTitle = QString("");	srcAuthor = QString("");	srcComment = QString("");
    lnkID = 0;	lnkComment = QString("");	lnkRef = QString("");
    srcList.clear();
    lnkList.clear();

    return true;
}

//-------------------------------------------------
bool SrcListParser::startElement(const QString &, const QString &, const QString &qname, const QXmlAttributes &attr)
{
    QString lower = qname.toLower();
	curTag = lower;
	if(lower == QString("source")) {
		srcID++;
        srcTitle = attr.value(QString("title"));
		srcAuthor = attr.value(QString("author"));
		srcComment = attr.value(QString("comment"));
//		qDebug() << "title = " << srcTitle << ", author= " << srcAuthor << ", comment= " << srcComment << ", ID= " << srcID;
	}else if(lower == QString("link")) {
		lnkID++;
		lnkComment = attr.value(QString("comment"));
	}
	
    return true;
}

//-------------------------------------------------
bool SrcListParser::endElement(const QString &nameSpace, const QString &localName, const QString &qName)
{
    Q_UNUSED(nameSpace);// These variables required for the function, but we don't use them. 
    Q_UNUSED(localName);// So tell compiler to not warn about this.

    QString lower = qName.toLower();
	if(lower == QString("source")) {
		srcList.append(SrcItem(srcID, srcTitle, srcAuthor, srcComment));
	}
    return true;
}

//-------------------------------------------------
bool SrcListParser::characters(const QString& ch)
{
    QString str = ch.simplified();
    if (str.isEmpty())  //ignore link if it is empty
        return true;

	if(curTag == QString("link")){
			lnkRef = str;
			lnkList.append(LnkItem(lnkID, srcID, lnkComment, lnkRef));			
//			qDebug() << "link comment = " << lnkComment << ", ref= " << lnkRef << ", srcID= " << srcID;
	}else return false;

    return true;
}

//-------------------------------------------------
bool SrcListParser::fatalError(const QXmlParseException& exception)
{
    errorString += QString::fromLatin1("Fatal error encountered during parsing sources list: %1 in line %2, column %3")
        .arg(exception.message())
        .arg(exception.lineNumber())
        .arg(exception.columnNumber());

    return QXmlDefaultHandler::fatalError(exception);
}
