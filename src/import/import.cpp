#include "import.h"


#include "helpdialog.h"
#include "mainwindow.h"
#include "config.h"

#include <QDialog>

#include <QFileDialog>
#include <QTextStream>



Import::Import(QObject *parent) :
    QObject(parent)
{

    //    mystr = "test1";
    //    qDebug() << "Debug: _Import()" << "mystr =" << mystr;
}

//----------------------------------------------------

void Import::importBook(QString file)
{
    qDebug() << "Debug: _Import::importBook(QString file)" << "Start import book";

}
//----------------------------------------------------
void Import::importChapter(QString file)
{
    qDebug() << "Debug: _Import::importChapter(QString file)" << "Start import Chapter";

}
//----------------------------------------------------
void Import::importModule(QString file)
{
    qDebug() << "Debug: _Import::importModule(QString file)" << "Start import Module";

    importIni(file);
}
//----------------------------------------------------
void Import::importIni(QString filename)
{
    qDebug() << "Debug: _Import::importIni(QString filename)" << "Start import ini";
    qDebug() << "Debug: _Import::importIni(QString file)" << "file = " << filename;

    QFile file(filename);
    QString line;
    if ( file.open(QIODevice::ReadOnly) )
    {
        int count;
        // file opened successfully
        QTextStream stream( &file );        // use a text stream
        // until end of file...
        do {
            // read and parse the command line
            line = stream.readLine();         // line of text excluding '\n'
            // do something with the line
//            qDebug() << "Debug: _Import::importIni(QString filename)" << "line = " << line;

            if (line != "")
            {
                if (miniparserini(line,"BibleName") != "")
                {
                    Config::configuration() -> setModuleBiblename(miniparserini(line,"BibleName"));
                    qDebug() << "test config " << Config::configuration() -> ModuleBiblename();
                    count++;
                };
                if (miniparserini(line,"BibleShortName") != "")
                {
                    count++;
                };
                if (miniparserini(line,"Copyright") != "")
                {
                    count++;
                };
                if (miniparserini(line,"ChapterSign") != "")
                {
                    count++;
                };
                if (miniparserini(line,"VerseSign") != "")
                {
                    count++;
                };
                if (miniparserini(line,"BookQty") != "")
                {
                    count++;
                };
                if (count = 0)
                {
                    //парсим книгу
                }
            }
//             qDebug() << "Debug: _Import::importIni(QString filename)" << "line biblenae = " << line;

        } while (!line.isNull());
        // Close the file
        file.close();
    }
}

QString Import::miniparserini(QString str, QString po)
{
    po.append(" = ");
    if (str.indexOf(po) >= 0)
    {
        str.remove(po);
        return str;
    }
    return "";

}

//----------------------------------------------------
//void Import::setTestValue(QString test)
//{
//    mystr = test;
//}

//QString Import::getTestValue()
//{
//    return mystr;
//}

//void Import::test()
//{
//    qDebug() << "Debug: _Import::test()" << "mystr = " << mystr;
//}
