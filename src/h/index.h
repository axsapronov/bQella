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


#ifndef INDEX_H
#define INDEX_H

#include <QStringList>
#include <QHash>
#include <QDataStream>
#include <QObject>
#include <QList>
#include <QFile>
#include <QVector>

struct Document {
    Document( int d, int f ) : docNumber( d ), frequency( f ) {}
    Document() : docNumber( -1 ), frequency( 0 ) {}
    bool operator==( const Document &doc ) const {
        return docNumber == doc.docNumber;
    }
    bool operator<( const Document &doc ) const {
        return frequency > doc.frequency;
    }
    bool operator<=( const Document &doc ) const {
        return frequency >= doc.frequency;
    }
    bool operator>( const Document &doc ) const {
        return frequency < doc.frequency;
    }
    qint16 docNumber;
    qint16 frequency;
};

QDataStream &operator>>( QDataStream &s, Document &l );
QDataStream &operator<<( QDataStream &s, const Document &l );

class Index : public QObject
{
    Q_OBJECT
public:
    struct Entry {
            Entry( int d ) { documents.append( Document( d, 1 ) ); }
            Entry( QVector<Document> l ) : documents( l ) {}
            QVector<Document> documents;
    };
    struct PosEntry {
            PosEntry( int p ) { positions.append( p ); }
            QList<uint> positions;
    };

    Index( const QString &dp, const QString &hp );
    Index( const QStringList &dl, const QString &hp );
    void writeDict();
    void readDict();
    int makeIndex();
    QStringList query( const QStringList&, const QStringList&, const QStringList& );
    QString getDocumentTitle( const QString& );
    void setDictionaryFile( const QString& );
    void setDocListFile( const QString& );
    void setDocList( const QStringList & );

signals:
    void indexingProgress( int );

private slots:
    void setLastWinClosed();

private:
    void setupDocumentList();
    void parseDocument( const QString&, int );
    void insertInDict( const QString&, int );
    void writeDocumentList();
    void readDocumentList();
    QStringList getWildcardTerms( const QString& );
    QStringList split( const QString& );
    QVector<Document> setupDummyTerm( const QStringList& );
    bool searchForPattern( const QStringList&, const QStringList&, const QString& );
    void buildMiniDict( const QString& );
    QString getCharsetForDocument(QFile *);
    QStringList docList;
    QHash<QString, Entry*> dict;
    QHash<QString, PosEntry*> miniDict;
    uint wordNum;
    QString docPath;
    QString dictFile, docListFile;
    bool alreadyHaveDocList;
    bool lastWindowClosed;
    QHash<QString, QString> documentTitleCache;
};

#endif
