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

void Import::importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty)
{
    qDebug() << "Debug: _Import::importBook(QString file):" << "Start import book";
    qDebug() << "Debug: _Import::importBook(QString file):" << "pathName = " << pathName;
    QString line;
    QFile file(pathName);
    if ( file.open(QIODevice::ReadOnly) )
    {
        // file opened successfully
        QTextStream stream( &file );        // use a text stream
        // until end of file...
        line = stream.readLine();
        while (line.indexOf(ChapterSign) == -1)
            line = stream.readLine();


        qDebug() << "BookQry = " << BookQty;
        for (int i = 1; i <= BookQty; i++)
        {
            for (int j = 1; j <= ChapterQty; j++)
            {
                do{
                    line = stream.readLine();
                    QString str = importChapter(line);
                }
                while ((!line.isNull()) and ( line.indexOf(ChapterSign) == -1));
            }
        }
        file.close();
    }
    else
        qDebug() << "Debug: _Import::importBook" << "Error: not open file";

}
//----------------------------------------------------
QString Import::importChapter(QString line)
{
//    qDebug() << "Debug: _Import::importChapter:" << "Start import Chapter";

    line
            .remove("<sup>")
            .remove("</sup>");

//    qDebug() << "Debug: _Import::importChapter:" << "line " << line;
    return line;

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
        int BookQtyIn = 2000;
        int book = 0;
        //        int count;
        // file opened successfully
        QTextStream stream( &file );        // use a text stream
        // until end of file...
        do {
            // read and parse the command line
            line = stream.readLine();         // line of text excluding '\n'
            // do something with the line
            //            qDebug() << "Debug: _Import::importIni(QString filename)" << "line = " << line;
            //            count = 0;

            if (line != "")
            {
                if (miniparserini(line,"BibleName") != "") Config::configuration() -> setModuleBiblename(miniparserini(line,"BibleName"));
                if (miniparserini(line,"BibleShortName") != "") Config::configuration() -> setModuleBibleShortName(miniparserini(line,"BibleShortName"));
                if (miniparserini(line,"Copyright") != "") Config::configuration() -> setModuleCopyright(miniparserini(line,"Copyright"));
                if (miniparserini(line,"ChapterSign") != "") ChapterSign = miniparserini(line,"ChapterSign");
                if (miniparserini(line,"VerseSign") != "") VerseSign = miniparserini(line,"VerseSign");
                if (miniparserini(line,"BookQty") != "")  BookQty = miniparserini(line,"BookQty").toInt();
                if (miniparserini(line,"PathName") != "")
                {
                    //парсим инфу о книгке
                    QString line2 = stream.readLine();
                    QString line3 = stream.readLine();
                    QString line4 = stream.readLine();

//                    qDebug() << "line = " << line;
//                    qDebug() << "line2 = " << line2;
//                    qDebug() << "line3 = " << line3;
//                    qDebug() << "line4 = " << line4;

                    filename.remove(filename.length()-11, 11);
                    QString path = filename+miniparserini( line, "PathName");
                    QString full = miniparserini( line2, "FullName");
                    QString shortna = miniparserini( line3, "ShortName");
                    int chapter = QString(miniparserini( line4 , "ChapterQty" )).toInt();
                    book++;

//                    qDebug() << " pathame = " << path;
                    importBook(path, full, shortna, chapter);

                }
            }
            //             qDebug() << "Debug: _Import::importIni(QString filename)" << "line biblenae = " << line;

        } while ((!line.isNull()) || (BookQtyIn == book));
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
        str.remove(" ");
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
