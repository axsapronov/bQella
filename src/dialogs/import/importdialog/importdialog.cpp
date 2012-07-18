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


#include "importdialog.h"
#include "pcommon.h"
#include "helpdialog.h"
#include "mainwindow.h"
#include "config.h"



#include <QFileDialog>
#include <QStringList>
#include <QStringListModel>



Import::Import(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    QStringListModel *typeModel;
    QStringList items = getFillEncoding();
    typeModel = new QStringListModel(items, this);
    ui.cBEncoding -> setModel(typeModel);


//    ui.lEImportFile->setText("");
    ui.lEImportFile->setText("/home/files/Documents/Bible/unrar/NT_Greek_WH-E_UTF8/BIBLEQT.INI");

    connect(ui.pBImportFile, SIGNAL(clicked()), this, SLOT(selectImportFile()));
}
//----------------------------------------------------
void Import::selectImportFile()
{
    QString beginpath = "/home/warmonger";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select bibleqt.ini"),
                                                    beginpath,
                                                    tr("Bibleqt.ini (*.ini)"));
    if (!fileName.isNull())
    {
        ui.lEImportFile->setText(fileName);
    }
}
//----------------------------------------------------
void Import::on_buttonBox_accepted()
{
    if (!ui.lEImportFile->text().isEmpty())
    {
        encoding = ui.cBEncoding->currentText();
        encoding.replace("CP", "Windows");
        QTextCodec * codec = QTextCodec::codecForName("UTF-8");

        if (encoding == "UTF-8") codec = QTextCodec::codecForName("UTF-8");
        if (encoding == "UTF-16") codec = QTextCodec::codecForName("UTF-16");
        if (encoding == "UTF-32") codec = QTextCodec::codecForName("UTF-32");
        if (encoding == "Windows-1251") codec = QTextCodec::codecForName("Windows-1251");
        if (encoding == "Windows-1252") codec = QTextCodec::codecForName("Windows-1252");
        if (encoding == "Windows-1253") codec = QTextCodec::codecForName("Windows-1253");
        if (encoding == "Windows-1254") codec = QTextCodec::codecForName("Windows-1254");
        if (encoding == "Windows-1255") codec = QTextCodec::codecForName("Windows-1255");
        if (encoding == "Windows-1256") codec = QTextCodec::codecForName("Windows-1256");
        if (encoding == "Windows-1257") codec = QTextCodec::codecForName("Windows-1257");
        if (encoding == "Windows-1258") codec = QTextCodec::codecForName("Windows-1258");

        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForTr(codec);

        importModule(ui.lEImportFile->text());

        emit SuccessfulImport();

    }

    QWidget::hide();  //close dialog
}
//----------------------------------------------------
void Import::importModule(QString file)
{
    qDebug() << "Debug: _Import::importModule()" << "Start import Module";

    Config::configuration()->setLanguage("rus");
    importIni(file);
    createInstructionFile();
    addContentToEndProjectFile();
    //    qDebug() << "TEST = " << HelpDialog::ui.listContents -> topLevelItem(0) -> data(0,LinkRole).toString().remove("file:");
}
//----------------------------------------------------
void Import::importIni(QString filename)
{
    qDebug() << "Debug: _Import::importIni()" << "Start import ini";
    //    qDebug() << "Debug: _Import::importIni()" << "file = " << filename;

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

                if (miniparserini(line,"Bible") != "") Config::configuration() -> setModuleType(QStringtoBool(miniparserini(line,"Bible")));
                if (miniparserini(line,"OldTestament") != "") Config::configuration() -> setOldTestament(QStringtoBool(miniparserini(line,"OldTestament")));
                if (miniparserini(line,"NewTestament") != "") Config::configuration() -> setNewTestament(QStringtoBool(miniparserini(line,"NewTestament")));
                if (miniparserini(line,"Apocrypha") != "") Config::configuration() -> setApocrypha(QStringtoBool(miniparserini(line,"Apocrypha")));
                if (miniparserini(line,"ChapterZero") != "") Config::configuration() -> setChapterZero(QStringtoBool(miniparserini(line,"ChapterZero")));
                if (miniparserini(line,"EnglishPsalms") != "") Config::configuration() -> setEnglishPsalms(QStringtoBool(miniparserini(line,"EnglishPsalms")));
                if (miniparserini(line,"StrongNumber") != "") Config::configuration() -> setStrongNumber(QStringtoBool(miniparserini(line,"StrongNumber")));
                if (miniparserini(line,"NoForcedLineBreaks") != "") Config::configuration() -> setNoForcedLineBreaks(QStringtoBool(miniparserini(line,"NoForcedLineBreaks")));
                if (miniparserini(line,"UseChapterHead") != "") Config::configuration() -> setUseChapterHead(QStringtoBool(miniparserini(line,"UseChapterHead")));
                if (miniparserini(line,"UseRightAlignment") != "") Config::configuration() -> setUseRightAlignment(QStringtoBool(miniparserini(line,"UseRightAlignment")));

                if (miniparserini(line,"StrongsDirectory") != "") Config::configuration() -> setStrongsDirectory(miniparserini(line,"StrongsDirectory"));
                if (miniparserini(line,"SoundDirectory") != "") Config::configuration() -> setSoundDirectory(miniparserini(line,"SoundDirectory"));
                if (miniparserini(line,"Language") != "") Config::configuration() -> setLanguage(miniparserini(line,"Language"));
                if (miniparserini(line,"InstallFonts") != "") Config::configuration() -> setInstallFonts(miniparserini(line,"InstallFonts"));
                if (miniparserini(line,"DesiredFontName") != "") Config::configuration() -> setDesiredFontName(miniparserini(line,"DesiredFontName"));
                if (miniparserini(line,"Categories") != "") Config::configuration() -> setCategories(miniparserini(line,"Categories"));
                if (miniparserini(line,"DesiredFontPath") != "") Config::configuration() -> setDesiredFontPath(miniparserini(line,"DesiredFontPath"));
                if (miniparserini(line,"DefaultEncoding") != "")  Config::configuration() -> setDefaultEncoding(miniparserini(line,"DefaultEncoding"));
                if (miniparserini(line,"DesiredUIFont") != "") Config::configuration() -> setDesiredUIFont(miniparserini(line,"DesiredUIFont"));
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

                    QString full = miniparserini( line2, "FullName");
                    QString shortna = miniparserini( line3, "ShortName");
                    QString path = filename + miniparserini( line, "PathName");
//                    QString path = filename + checkProcentRol(shortna, 50)+".htm";
//                    qDebug()/* << " bookshorname = " << shortna */<< " check = " << checkProcentRol(shortna, 50) << "path = " << path << "filename = " << filename;
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
    QString title = FullName; /*pathName.split("/").last().split(".").first();*/
//    QString path = "./book_" + pathName.split("/").last();
    QString path = "./book_" + checkProcentRol(ShortName, 50, pathName.split("/").last())+".htm";

//    qDebug() << " path = " << path << " pathname = " << pathName;
    // create book file
    createBookFile(pathName, FullName, ShortName, ChapterQty);

    QString chunksnameforchapter = QString(path).remove("./book_").remove(".htm");
    QString filenameforchapter = QString(pathName).replace(getFileNameAbs(pathName), chunksnameforchapter);
//    qDebug() << " chunk 1 = " << chunksnameforchapter << " chunkk = " << filenameforchapter;

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
            //            qDebug() << " _ titlechap = " << titlechap;
            QString chapterfile = incstr(titlechap, GL_LengtItemString, "_");
            titlechap = incstr(titlechap, GL_LengtItemString, " ");

            //            qDebug() << " __ titlechap = " << titlechap;
            QString pathchap = pathName;
            //            pathchap.replace( QString("_chapter_%1").arg(j), QString("_chapter_"+chapterfile) );

            //            qDebug() << "\n----- pathchap = " << pathchap;
            pathchap =  "./book_" + chunksnameforchapter + QString("_chapter_" + chapterfile + "." + last);
            QString textchap = QString("<section title=\"" + Qt::escape(titlechap) + "\" ref=\"" + Qt::escape(pathchap) + "\" icon=\"\">");
            //            qDebug() << "--textchap = " << textchap << " pathchap = " << pathchap;
            addContentToProjectFile(textchap , true);
            QString text ="";
            //            qDebug() << "Debug: _Import::importBook()" << "chaptersign = " << ChapterSign;
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
            createChaterFile(filenameforchapter, text, j);
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
void Import::importBook(QString projectfile, QString pathName, QString FullName, QString ShortName, int ChapterQty,QString myChapterSign)
{
    //    qDebug() << "Debug: _Import::importBook(QString file):" << "Start import book";
    //    qDebug() << "Debug: _Import::importBook(QString file):" << "pathName = " << pathName;
    QString last = pathName.split("/").last().split(".").last(); // получаем разрешение файла (htm)
    QString title = pathName.split("/").last().split(".").first();
//    QString path = "./book_" + pathName.split("/").last();

    QString path = "./book_" + checkProcentRol(ShortName, 50, pathName.split("/").last())+".htm";
    // create book file
    createBookFile(pathName, FullName, ShortName, ChapterQty);

    QString chunksnameforchapter = QString(path).remove("./book_").remove(".htm");
    QString filenameforchapter = QString(pathName).replace(getFileNameAbs(pathName), chunksnameforchapter);
//    qDebug() << " chunk 1 = " << chunksnameforchapter << " chunkk = " << filenameforchapter;


    // add info to project file
    QString text2 = QString("<section title=\"" + Qt::escape(title) + "\" ref=\"" + Qt::escape(path) + "\" icon=\"\">");
    addContentToProjectFile(projectfile ,text2, false);


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
        while ((line.indexOf(myChapterSign) == -1) and (!stream.atEnd()))
            line = stream.readLine();
//        qDebug() << " test = ";
        for (int j = 1; j <= ChapterQty; j++)
        {
            QString titlechap = QString("%1").arg(j);
            QString chapterfile = incstr(titlechap, GL_LengtItemString, "_");
            titlechap = incstr(titlechap, GL_LengtItemString, " ");
            QString pathchap = pathName;
            pathchap =  "./book_" + chunksnameforchapter + QString("_chapter_" + chapterfile + "." + last);
            QString textchap = QString("<section title=\"" + Qt::escape(titlechap) + "\" ref=\"" + Qt::escape(pathchap) + "\" icon=\"\">");
            addContentToProjectFile(projectfile, textchap , true);
            QString text ="";
            do
            {
//                qDebug() << " chaptersign = " << myChapterSign;
                line = stream.readLine();
//                qDebug() << " line = " << line;
                if (line.indexOf(myChapterSign) >=0)
                {
                    line = "";
                    flag = false;
                }
                text.append("\n"+importChapter(line));
            }
            while ((!line.isNull()) and ( flag));

//            qDebug() << " text = " << text << " path = " << pathName;
            flag = true;
            createChaterFile(filenameforchapter, text, j);
            addContentToProjectFile(projectfile, "</section>", true);

        }
        file.close();
    }
    else
        qDebug() << "Debug: _Import::importBook" << "Error: not open file";

    QString text = "</section>";
    addContentToProjectFile(projectfile, text, false);
}
//----------------------------------------------------
QString Import::importChapter(QString line)
{
    //    qDebug() << "Debug: _Import::importChapter:" << "Start import Chapter";
    line
            .remove("<sup>")
            .remove("</sup>");
//        qDebug() << "Debug: _Import::importChapter:" << "line " << line;
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

    pr.oldTestament = Config::configuration()->OldTestament();
    pr.newTestament = Config::configuration()->NewTestament();
    pr.apocrypha = Config::configuration()->Apocrypha();
    pr.chapterZero = Config::configuration()->ChapterZero();
    pr.englishPsalms = Config::configuration()->EnglishPsalms();
    pr.strongNumber = Config::configuration()->StrongNumber();
    pr.noForcedLineBreaks = Config::configuration()->NoForcedLineBreaks();
    pr.useChapterHead = Config::configuration()->UseChapterHead();
    pr.useRightAlignment = Config::configuration()->UseRightAlignment();

    pr.strongsDirectory = Config::configuration()->StrongsDirectory();
    pr.soundDirectory = Config::configuration()->SoundDirectory();
    pr.language = Config::configuration()->Language();
    pr.installFonts = Config::configuration()->InstallFonts();
    pr.desiredFontName = Config::configuration()->DesiredFontName();
    pr.categories = Config::configuration()->Categories();
    pr.desiredFontPath = Config::configuration()->DesiredFontName();
    pr.defaultEncoding = Config::configuration()->DefaultEncoding();
    pr.desiredUIFont = Config::configuration()->DesiredUIFont();


    QString ind1="   ";
    QString fn = unurlifyFileName(pr.prjFN);
//    qDebug() << " fn = " << fn;
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

    ts << ind1 << "<property name=\"strongsdirectory\">" << Qt::escape(ist(pr.strongsDirectory)) << "</property>" << endl;
    ts << ind1 << "<property name=\"sounddirectory\">" << Qt::escape(ist(pr.soundDirectory)) << "</property>" << endl;
    ts << ind1 << "<property name=\"language\">" << Qt::escape(pr.language) << "</property>" << endl;
    ts << ind1 << "<property name=\"installfonts\">" << Qt::escape(ist(pr.installFonts)) << "</property>" << endl;
    ts << ind1 << "<property name=\"desiredfontname\">" << Qt::escape(ist(pr.desiredFontName)) << "</property>" << endl;
    ts << ind1 << "<property name=\"categories\">" << Qt::escape(ist(pr.categories)) << "</property>" << endl;
    ts << ind1 << "<property name=\"desiredfontpath\">" << Qt::escape(ist(pr.desiredFontPath)) << "</property>" << endl;
    ts << ind1 << "<property name=\"defaultencoding\">" << Qt::escape(ist(pr.defaultEncoding)) << "</property>" << endl;
    ts << ind1 << "<property name=\"desireduifont\">" << Qt::escape(ist(pr.desiredUIFont)) << "</property>" << endl;

    ts << ind1 << "<property name=\"moduletype\">" << Qt::escape(BooltoQString(pr.moduleType)) << "</property>" << endl;
    ts << ind1 << "<property name=\"oldtestament\">" << Qt::escape(BooltoQString(pr.oldTestament)) << "</property>" << endl;
    ts << ind1 << "<property name=\"newtestament\">" << Qt::escape(BooltoQString(pr.newTestament)) << "</property>" << endl;
    ts << ind1 << "<property name=\"apocrypha\">" << Qt::escape(BooltoQString(pr.apocrypha)) << "</property>" << endl;
    ts << ind1 << "<property name=\"chapterzero\">" << Qt::escape(BooltoQString(pr.chapterZero)) << "</property>" << endl;
    ts << ind1 << "<property name=\"englishpsalms\">" << Qt::escape(BooltoQString(pr.englishPsalms)) << "</property>" << endl;
    ts << ind1 << "<property name=\"strongnumber\">" << Qt::escape(BooltoQString(pr.strongNumber)) << "</property>" << endl;
    ts << ind1 << "<property name=\"noforcedlinebreaks\">" << Qt::escape(BooltoQString(pr.noForcedLineBreaks)) << "</property>" << endl;
    ts << ind1 << "<property name=\"userightalignment\">" << Qt::escape(BooltoQString(pr.useRightAlignment)) << "</property>" << endl;
    ts << ind1 << "<property name=\"usechapterhead\">" << Qt::escape(BooltoQString(pr.useChapterHead)) << "</property>" << endl;


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
//    path.toUtf8();
    QDir dir(path);
    //        qDebug() << "path = " << path.toLower();
    if (!QDir(QString(path)).exists())
    {
        dir.mkdir(QString(path));
    }
    else
    {
        qDebug() << "Debug: _Import::createImportFolder" << QString(tr("This folder is imported already exists:")) << path;
    }
    Config::configuration()->setCurPrjDir(path);
    //        qDebug() << " curprg  = " << Config::configuration()->CurPrjDir();
}
//----------------------------------------------------
void Import::createBookFile(QString pathName, QString FullName, QString ShortName, int ChapterQty)
{
    QString pathNameE = pathName.split("/").last(); // получаем pathname (filename.htm)
    pathNameE.remove("book_");
//    QString fileimportname = Config::configuration()->CurPrjDir() + "/book_"+ pathNameE;
    QString fileimportname = Config::configuration()->CurPrjDir() +  "/book_" + checkProcentRol(ShortName, 50, pathName.split("/").last())+".htm";
//    qDebug() << " \n ------- pathname = " << pathNameE << " filenamef = " << fileimportname;
    if (pathNameE.indexOf("book_") < 0)
        pathNameE = "book_" + pathNameE;
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
    /*
    qDebug() << "\n---test = " << test;
    test.replace( QString("_chapter_%1").arg(j), chapterfile );
    qDebug() << "--------test = " << test;*/

    QString chapterfilecount = incstr(QString("%1").arg(i), GL_LengtItemString, "_");
    QString pathNameE = file.split("/").last(); // получаем pathname (filename.htm)
    QString last = file.split("/").last().split(".").last();
    pathNameE.remove("book_");
    QString fileimportname = Config::configuration()->CurPrjDir() + "/book_"+ pathNameE.remove("."+last) +QString("_chapter_%1.").arg(chapterfilecount) + last;

    //    qDebug() << "Debug: _Import::createChaterFile()" << fileimportname;
    if (QFile::exists(fileimportname))
    {
        QFile::remove(fileimportname);
    }
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
    QString text = tr("Add a manual");
    createEmptyHtml(QString(Config::configuration()->CurPrjDir()+"/   ___Instruction"), QString("   ___Instruction"), text);
}
//----------------------------------------------------
void Import::addContentToProjectFile(QString text, bool tr)
{
    QString ind1="   ";
    QString fn = unurlifyFileName(getPrjFN());
    QFile f(fn);
    f.close();

    f.open(QFile::Append);
    if (f.isOpen())
    {
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
    }
    else
    {
        qDebug() << "Debug: _Import::addContentToProjectFile" << " not open project file for add info" << " projectfile = " << getPrjFN();
    }
    f.close();
}
//----------------------------------------------------
void Import::addContentToProjectFile(QString filename, QString text, bool tr)
{
    if (!tr)
    {
        QStringList removestring;
        removestring << "</contents>"
                     << "</pemproject>";

        removeStringInFile(filename, removestring);    }
//    qDebug() << " text tut = " << text << " file = " << filename;
    QString ind1="   ";
    QString fn = unurlifyFileName(filename);
    QFile f(fn);
    f.close();
    f.open(QFile::Append);
    if (f.isOpen())
    {
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
    }
    else
    {
        qDebug() << "Debug: _Import::addContentToProjectFile" << " not open project file for add info";
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
void Import::addContentToEndProjectFile(QString filename)
{
    QString fn = unurlifyFileName(filename);
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
    return QString(Config::configuration()->CurPrjDir()+"/" + Config::configuration()->ModuleBiblename()+ ".pem"); // Нахера если Config::configuration->CurProject тоже самое? =)
}
//----------------------------------------------------
QString Import::getStartPage()
{
    return QString(Config::configuration()->CurPrjDir()+"/   ___Instruction");
}
//----------------------------------------------------


