#include "import.h"
#include "pcommon.h"
#include "helpdialog.h"
#include "mainwindow.h"
#include "config.h"

#include <QDialog>
#include <QFileDialog>
#include <QTextStream>

Import::Import(QObject *parent) :
    QObject(parent)
{

}
//----------------------------------------------------
void Import::importModule(QString file)
{
    qDebug() << "Debug: _Import::importModule(QString file)" << "Start import Module";
    importIni(file);
    createInstructionFile();
    addContentToEndProjectFile();
    //    qDebug() << "TEST = " << HelpDialog::ui.listContents -> topLevelItem(0) -> data(0,LinkRole).toString().remove("file:");
}
//----------------------------------------------------
void Import::importIni(QString filename)
{
    qDebug() << "Debug: _Import::importIni(Q)" << "Start import ini";
    qDebug() << "Debug: _Import::importIni()" << "file = " << filename;

    //    qDebug() << "Debug: _Import::importIni()" << "curprj" << Config::configuration()->PrjDir();
    //    qDebug() << "Debug: _Import::importIni()" << "curprj2" << filename.remove(filename.length()-11, 11);
    //    Config::configuration()->setCurPrjDir(filename.remove(filename.length()-11, 11));
    QFile file(filename);

    filename.remove(filename.length()-11, 11);
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
            if (line != "" and line.indexOf("//") <0)
            {
                if (miniparserini(line,"BibleName") != "")
                {
                    Config::configuration() -> setModuleBiblename(miniparserini(line,"BibleName"));
                    createImportFolder(Config::configuration()->PrjDir() + Config::configuration()->ModuleBiblename());
                }
                if (miniparserini(line,"BibleShortName") != "") Config::configuration() -> setModuleBibleShortName(miniparserini(line,"BibleShortName"));
                if (miniparserini(line,"Copyright") != "") Config::configuration() -> setModuleCopyright(miniparserini(line,"Copyright"));
                if (miniparserini(line,"ChapterSign") != "") ChapterSign = miniparserini(line,"ChapterSign");
                if (miniparserini(line,"VerseSign") != "") VerseSign = miniparserini(line,"VerseSign");
                if (miniparserini(line,"BookQty") != "")
                {
                    BookQty = miniparserini(line,"BookQty").toInt();

                    importProjectFile();
                }
                if (miniparserini(line,"PathName") != "")
                {
                    //парсим инфу о книгке
                    QString line2 = stream.readLine();
                    QString line3 = stream.readLine();
                    QString line4 = stream.readLine();
                    QString path = filename + miniparserini( line, "PathName");
                    QString full = miniparserini( line2, "FullName");
                    QString shortna = miniparserini( line3, "ShortName");
                    int chapter = QString(miniparserini( line4 , "ChapterQty" )).toInt();
                    book++;
                    importBook(path, full, shortna, chapter);

                }
            }
        } while ((!line.isNull()) || (BookQtyIn == book));
        // Close the file
        file.close();
    }
}
//----------------------------------------------------
void Import::importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty)
{
//    qDebug() << "Debug: _Import::importBook(QString file):" << "Start import book";
//    qDebug() << "Debug: _Import::importBook(QString file):" << "pathName = " << pathName;
    QString last = pathName.split("/").last().split(".").last(); // получаем разрешение файла (htm)
    QString title = pathName.split("/").last().split(".").first();
    QString path = "./book_" + pathName.split("/").last();

    // create book file
    createBookFile(pathName, FullName, ShortName, ChapterQty);


    // add info to project file
    QString text2 = QString("<section title=\"" + Qt::escape(title) + "\" ref=\"" + Qt::escape(path) + "\" icon=\"\">");
    addContentToProjectFile(text2, false);

//    qDebug() << " _filename_ = " << pathName;

    // parse
    bool flag;
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
        //        qDebug() << "BookQry = " << BookQty;
        for (int j = 1; j <= ChapterQty; j++)
        {
            QString titlechap = QString("%1").arg(j);
            QString pathchap = pathName;
            pathchap =  "./book_" + pathName.split("/").last().remove("."+last) + QString("_chapter_%1.%2").arg(j).arg(last);
            QString textchap = QString("<section title=\"" + Qt::escape(titlechap) + "\" ref=\"" + Qt::escape(pathchap) + "\" icon=\"\">");

            //                qDebug() << "pathchap = " << pathchap;

            addContentToProjectFile(textchap , true);
            QString text ="";
            do{
                line = stream.readLine();
                if (line.indexOf(ChapterSign) >=0)
                {
                    line = "";
                    flag = false;
                }
                text.append("\n"+importChapter(line));
            }
            while ((!line.isNull()) and ( flag));

            flag = true;
            //                qDebug() << "\n\n pathnamec = " << pathNameC << "pathname = " << pathName;
            createChaterFile(pathName, text, j);
            addContentToProjectFile("</section>", true);

        }
        file.close();
    }
    else
        qDebug() << "Debug: _Import::importBook" << "Error: not open file";

    QString text = "</section>";
    addContentToProjectFile(text, false);
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
void Import::importProjectFile()
{
    createProjectFile();

    ModuleProperties pr;
    pr.prjFN = getPrjFN();
    pr.prjStartPage = getStartPage();
    pr.moduleBiblename = Config::configuration()->ModuleBiblename();
    pr.moduleBibleShortName = Config::configuration()->ModuleBibleShortName();
    pr.moduleBVersion = 1.0;
    pr.prjTitle = Config::configuration()->ModuleBiblename();

    QString ind1="   ";
    QString fn = unurlifyFileName(pr.prjFN);
    qDebug() << " fn = " << fn;
    Config::configuration() -> toAppLog(1, tr("Create a new project: %1", "For log").arg(pr.prjTitle));
    Config::configuration() -> toAppLog(3, tr("- project file: %1", "For log").arg(fn));
    QFile f(fn);
    if (!f.open(QFile::WriteOnly)){
        qDebug() << "Failed to create project: " << fn;
        //        statusBar() -> showMessage(tr("Failed to create project: %1").arg(fn), 7000);
        Config::configuration() -> toAppLog(1, tr("- failed", "For log"));
        return;
    }

    Config::configuration() -> toAppLog(3, tr("- project start page: %1", "For log").arg(pr.prjStartPage));
    QFileInfo fi(fn);
    QString name = fi.baseName();
    QString path = fi.absolutePath();
    Config::configuration() -> setDbName(path +"/"+ name +"-sources.db");
    name.remove(QChar(' '));
    QString spFN = relatifyFileName(pr.prjStartPage, path);
    //fi.setFile(spFN);
    QString spT = tr("   ___Instruction");//fi.baseName(); // name of first doc in project in listcontent
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << "<pemproject version=\"1.0\">" << endl << endl;

    QString version;
    version.setNum(pr.moduleBVersion);
    //    qDebug() << "Debug: _MainWindow::createProject" << "version(str) = " << version << "version(double) = " << pr.moduleBVersion;

    ts << "<profile>" << endl;
    ts << ind1 << "<property name=\"title\">" << Qt::escape(pr.prjTitle) << "</property>" << endl;
    ts << ind1 << "<property name=\"name\">" << Qt::escape(name) << "</property>" << endl;
    ts << ind1 << "<property name=\"startpage\">" << Qt::escape(spFN) << "</property>" << endl;
    ts << ind1 << "<property name=\"biblename\">" << Qt::escape(pr.moduleBiblename) << "</property>" << endl;
    ts << ind1 << "<property name=\"bibleshortname\">" << Qt::escape(pr.moduleBibleShortName) << "</property>" << endl;
    ts << ind1 << "<property name=\"copyright\">" << Qt::escape(pr.moduleCopyright) << "</property>" << endl;
    ts << ind1 << "<property name=\"version\">" << pr.moduleBVersion << "</property>" << endl;
    //    ts << ind1 << "<property name=\"type\">" << pr.moduleType << "</property>" << endl;
    ts << "</profile>" << endl << endl;

    ts << "<contents>" << endl;
    ts << ind1 << "<section title=\"" << Qt::escape(spT) << "\" ref=\""<< Qt::escape(spFN) << "\" icon=\"\">" << endl;
    ts << ind1 << "</section>" << endl;
    //    ts << "</contents>" << endl << endl;

    //    ts << "</pemproject>" << endl;
    f.close();

    Config::configuration() -> toAppLog(3, tr("- project sources DB: %1", "For log").arg(Config::configuration() -> DbName()));
    Config::configuration() -> toAppLog(1, tr("- done", "For log"));
}
//----------------------------------------------------
void Import::createImportFolder(QString path)
{
    QDir dir(path);
//        qDebug() << "path = " << path.toLower();
    if (!QDir(QString(path)).exists())
    {
        dir.mkdir(QString(path));
    }
    else
    {
        qDebug() << "Debug: _Import::createImportFolder" << QString(tr("Такая папка импорта уже существует:")) << path;
    }
    Config::configuration()->setCurPrjDir(path);
//        qDebug() << " curprg  = " << Config::configuration()->CurPrjDir();
}
//----------------------------------------------------
void Import::createBookFile(QString pathName, QString FullName, QString ShortName, int ChapterQty)
{
    QString pathNameE = pathName.split("/").last(); // получаем pathname (filename.htm)
    pathNameE.remove("book_");
    QString fileimportname = Config::configuration()->CurPrjDir() + "/book_"+ pathNameE;
    QString text = ""+tr("PathName = %1"
                         "\nFullName = %2"
                         "\nShortName = %3"
                         "\nChapterQty = %4")
            .arg(pathNameE)
            .arg(FullName)
            .arg(ShortName)
            .arg(ChapterQty);
    createEmptyHtml(fileimportname, "1" , text);
}
//----------------------------------------------------
void Import::createChaterFile(QString file, QString text, int i)
{
    QString pathNameE = file.split("/").last(); // получаем pathname (filename.htm)
    QString last = file.split("/").last().split(".").last();
    pathNameE.remove("book_");
    QString fileimportname = Config::configuration()->CurPrjDir() + "/book_"+ pathNameE.remove("."+last) +QString("_chapter_%1.").arg(i) + last;

    //    qDebug() << "Debug: _Import::createChaterFile()" << fileimportname;
    createEmptyHtml(fileimportname, QString("%1").arg(i) , text);
}
//----------------------------------------------------
void Import::createProjectFile()
{
    QString filename = Config::configuration()->CurPrjDir()+"/" + Config::configuration()->ModuleBiblename()+ ".pem";
    qDebug() << "filename = " << filename;
    // проверить можно ли обращаться к helpdialog по helpdilog::

    QFile file1(filename);

    if(!file1.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error write";
    }
    else
    {
        file1.write(QString("").toUtf8());
    }
}
//----------------------------------------------------
void Import::createInstructionFile()
{
    QString text = tr("Добавить инструкцию");
    createEmptyHtml(QString(Config::configuration()->CurPrjDir()+"/   ___Instruction"), QString("   ___Instruction"), text);
}
//----------------------------------------------------
void Import::addContentToProjectFile(QString text, bool tr)
{
    QString ind1="   ";
    QString fn = unurlifyFileName(getPrjFN());
    QFile f(fn);
    f.open(QFile::Append);
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    if (tr)
    {
        ts << ind1 << ind1 << text << endl;
    }
    else
    {
        ts << ind1 << text << endl;
    }
    f.close();
}
//----------------------------------------------------
void Import::addContentToEndProjectFile()
{
    QString fn = unurlifyFileName(getPrjFN());
    QFile f(fn);
    f.open(QFile::Append);
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << "</contents>" << endl << endl;
    ts << "</pemproject>" << endl;
    f.close();
}
//----------------------------------------------------
QString Import::getPrjFN()
{
    return QString(Config::configuration()->CurPrjDir()+"/" + Config::configuration()->ModuleBiblename()+ ".pem");
}
//----------------------------------------------------
QString Import::getStartPage()
{
    return QString(Config::configuration()->CurPrjDir()+"/   ___Instruction");
}
//----------------------------------------------------
QString Import::miniparserini(QString str, QString po)
{
    po.append(" = ");
    if (str.indexOf(po) >= 0)
    {
        str.remove(po);
        if (po != "ShortName = ")
        {
            str.remove(" \0");
        }
        return str;
    }
    return "";
}

