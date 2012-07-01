#include "export.h"
#include "helpdialog.h"
#include "config.h"
#include "pcommon.h"



Export::Export(QObject *parent) :
    QObject(parent)
{

    file = tr(""); // Что за херь?
}

void Export::exportCreateDir(QString current_dir)
{

    qDebug() << "Debug: _Export::exportCreateDir()" << "current_dir = " << current_dir;
    // если папки export нету, то создаем, если есть, то удаляем содержимое
    QDir dir(current_dir);
    //    qDebug() << "path = " << path <<  "last = " << path.last();
    if (!QDir(QString("%1export_%2/").arg(current_dir, getNameFolder(current_dir))).exists())
    {
        dir.mkdir(QString("export_%1").arg(getNameFolder(current_dir)));
    }
    else
    {
        dir.setPath((QString("%1export_%2").arg(current_dir, getNameFolder(current_dir))));
        QStringList lstFiles = dir.entryList(QDir::Files); //Получаем список файлов

        //Удаляем файлы
        foreach (QString entry, lstFiles)
        {
            QString entryAbsPath = dir.absolutePath() + "/" + entry;
            //qDebug() << entryAbsPath;
            QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
            QFile::remove(entryAbsPath);
        }
    }
}

void Export::exportBibleqtIni(QString string, int count)
{
    //экспортируем ini файл
    QFile file(string);
    //    qDebug() << string;
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Error write";
    }
    else
    {

        // сделать QStringList?
        QString stru =""+tr("BibleName = %1"
                            "\nBibleShortName = %2"
                            "\nCopyright = %3"
                            "\nDefaultEncoding = utf-8"
                            "\nChapterSign = <h4>"
                            "\nVerseSign = <p>"
                            "\nBookQty = %4"
                            "\nBible = %5"
                            "\nOldTestament = %6"
                            "\nNewTestament = %7"
                            "\nApocrypha = %8"
                            "\nChapterZero = %9"
                            "\nEnglishPsalms = %10"
                            "\nStrongNumber = %11"
                            "\nNoForcedLineBreaks = %12"
                            "\nUseChapterHead = %13"
                            "\nUseRightAlignment = %14"
                            "\nStrongsDirectory = %15"
                            "\nSoundDirectory = %16"
                            "\nLanguage = %17"
                            "\nInstallFonts = %18"
                            "\nDesiredFontName = %19"
                            "\nCategories  = %20"
                            "\nDesiredFontPath = %21"
                            "\nDefaultEncoding = %22"
                            "\nDesiredUIFont = %23"
                            )
                .arg(Config::configuration() -> profile() -> props["biblename"])  /*1*/
                .arg(Config::configuration() -> profile() -> props["bibleshortname"]) /*2*/
                .arg(Config::configuration() -> profile() -> props["copyright"]) /*3*/
                .arg(count) /*4*/
                .arg(Config::configuration() -> profile() -> props["moduletype"]) /*5*/
                .arg(Config::configuration() -> profile() -> props["oldtestament"]) /*6*/
                .arg(Config::configuration() -> profile() -> props["newtestament"])/*7*/
                .arg(Config::configuration() -> profile() -> props["apocrypha"]) /*8*/
                .arg(Config::configuration() -> profile() -> props["chapterzero"])/*9*/
                .arg(Config::configuration() -> profile() -> props["englishpsalms"]) /*10*/
                .arg(Config::configuration() -> profile() -> props["strongnumber"]) /*11*/
                .arg(Config::configuration() -> profile() -> props["noforcedlinebreaks"]) /*12*/
                .arg(Config::configuration() -> profile() -> props["usechapterhead"]) /*13*/
                .arg(Config::configuration() -> profile() -> props["userightalignment"]) /*14*/
                .arg(Config::configuration() -> profile() -> props["strongsdirectory"]) /*15*/
                .arg(Config::configuration() -> profile() -> props["sounddirectory"]) /*16*/
                .arg(Config::configuration() -> profile() -> props["language"]) /*17*/
                .arg(Config::configuration() -> profile() -> props["installfonts"]) /*18*/
                .arg(Config::configuration() -> profile() -> props["desiredfontname"]) /*19*/
                .arg(Config::configuration() -> profile() -> props["categories"]) /*20*/
                .arg(Config::configuration() -> profile() -> props["desiredfontpath"]) /*21*/
                .arg(Config::configuration() -> profile() -> props["defaultencoding"]) /*22*/
                .arg(Config::configuration() -> profile() -> props["desireduifont"]); /*23*/

        //         qDebug() << "test = " << Config::configuration() -> profile() -> props["biblename"];
        stru.replace("none", "");

        file.write(QString("%1").arg(stru).toUtf8());
    }
    file.close();
}

void Export::exportBibleqtIniInfo(QString file, QString filename, int count)
{
//    QString filename = helpDock -> ui.listContents -> topLevelItem(i) -> data(0,LinkRole).toString().remove("file:");
    QString string = exportChapter(filename, count ,false);

    QFile filebibleqt(file);
    if(!filebibleqt.open(QIODevice::Append))
    {
        qDebug() << "Error write";
    }
    else
    {
        filebibleqt.write(QString("%1").arg(string).toUtf8());
    }
    filebibleqt.close();
    //  qDebug() << string;

}

QString Export::exportChapter (QString filename, int i, bool chapt)
{
    // подготавливаем главы
    // мега индия
    QFile file(filename);
    QString str;
    if(file.exists())
    {
        if(!file.open(QIODevice::ReadWrite  | QIODevice::Text))
        {
            qDebug() << "Error write";
        }
        else
        {
            QTextStream stream(&file);
            stream.setCodec(QTextCodec::codecForName("UTF-8"));
            str = stream.readAll();

            QRegExp title("<title>[1]</title>");
            QRegExp rx("(<[^>]*>)");
            QRegExp rxp("(<p.*?>)");
            QRegExp rxi("( [a-zA-Z:]+=)|(\"[^\"]*\"|'[^']*')");

            if (chapt)
            {
                qDebug() << "************ Export: chapter-file";
                QString title = QString("<title>%1</title>").arg(i);
                QString chapter = tr("\n?h4?Глава %1?/h4?").arg(i);
                str.replace(title,chapter);
            }
            else
            {
                str.remove(title);
                qDebug() << "************ Export: book-file";
            }

            str.remove("p, li { white-space: pre-wrap; }")
                    .remove(title)
                    .replace(rxp, "?p?")
                    .remove("</p>")
                    .remove(rxi)
                    .replace("<p>", "?p?")
                    .remove(rx)
                    .replace("\n\n?p?PathName","PathName")
                    .replace("FullName", "\nFullName")
                    .replace("ShortName", "\nShortName")
                    .replace("ChapterQty", "\nChapterQty")
                    .replace("?h4?", "<h4>")
                    .replace("?/h4?\n\n", "</h4>")
                    .replace("?p?", "<p>");
        }
    }
    else
        qDebug() << "Error exist";
    file.close();
    return str;
}


//void Export::setTopLevelItemCount(int i)
//{
//    topLevelItemCount = i;
//}

//void Export::setFileBibleqtName(QString str)
//{
//    fileBibleqtName = str;
//}



void Export::test()
{

//    topLevelItemCount = 3;
//    qDebug() << "Debug: _Export::test()" << file;
//    qDebug() << "Debug: _Export::test()" << "fileBibleqtName = " << fileBibleqtName;
//    qDebug() << "Debug: _Export::test()" << "topLevelItemCount = " << topLevelItemCount;
//    qDebug() << "Debug: _Export::test()" << " " ;
//    qDebug() << "Debug: _Export::test()" << " "  ;
//    qDebug() << "Debug: _Export::test()" << " "  ;
//    qDebug() << "Debug: _Export::test()" << " "  ;
//    qDebug() << "Debug: _Export::test()" << " "  ;
//    qDebug() << "Debug: _Export::test()" << " "  ;
//    qDebug() << "Debug: _Export::test()" << " "  ;

}

QString Export::getNameFolder(QString cur_dir)
{
    QStringList path = cur_dir.split("/");
    path.removeLast();
    return path.last();
}


