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

#include "export.h"
#include "helpdialog.h"
#include "config.h"
#include "pcommon.h"


Export::Export(QObject *parent) :
    QObject(parent)
{

    file = tr(""); // Что за херь?
}
//---------------------------------
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
//---------------------------------
void Export::exportBibleqtIni(QString string, QString count)
{
    QString encoding = Config::configuration ()->profile ()->props["defaultencoding"];
    QTextCodec * codec = getCodecOfEncoding (encoding);
    //экспортируем ini файл
    QFile file(string);
    //    qDebug() << string;
    if(!file.open(QIODevice::WriteOnly))
        {
            qDebug() << "Error write";
        }
    else
        {
            QString htmlfilter = "<br> <pre> </pre>"
                    " <span </span> <font </font> <sup> </sup> <sub> </sub> <center> </center> <strong> </strong>"
                    " <em> </em> <table </table>"
                    " <tr <tr> </tr> <td <td> </td> <th> <th </th> <hr <hr>";
            // сделать QStringList?
            QString stru ="";
            stru.append(tr("\\\\ Этот модуль создан в программе bQella."
                           "\n\\\\ Разработчик программы: Сапронов Александр - sapronov.alexander92[@]gmail.com"
                           "\n\\\\ Github программы: http://warmonger1.github.com/bQella/"
                           "\n\\\\ Блог разработчика: http://warmonger72.blogspot.com/"
                           "\n\\\\ Свежая версия программы доступна по ссылкам: "
                           "\n\\\\ win:   "
                           "\n\\\\ source: \n\n\n"));

            stru.append(tr("\nBibleName = %1"
                           "\nBibleShortName = %2"
                           "\nCopyright = %3"
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
                           "\nHTMLFilter = %24"
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
                        .arg(Config::configuration() -> profile() -> props["desireduifont"]) /*23*/
                        .arg(Config::configuration() -> profile() -> props["htmlfilter"]));
            //                .arg(htmlfilter));
            //         qDebug() << "test = " << Config::configuration() -> profile() -> props["biblename"];
            stru.replace("none", "");

            QTextStream ts(&file);
            ts.setCodec(codec);
            ts << stru;
        }
    file.close();
}
//---------------------------------
void Export::exportBibleqtIniInfo(QString file, QString filename, QString count)
{
    //    QString filename = helpDock -> ui.listContents -> topLevelItem(i) -> data(0,LinkRole).toString().remove("file:");
    //    qDebug() << "\n-------str = " << filename;
    QString encoding = Config::configuration ()->profile ()->props["defaultencoding"];
    QTextCodec * codec = getCodecOfEncoding (encoding);
    QString string = exportChapter(filename, count ,false);
    QFile filebibleqt(file);
    if(!filebibleqt.open(QIODevice::Append))
        {
            qDebug() << "Error write";
        }
    else
        {
            QTextStream ts(&filebibleqt);
            ts.setCodec(codec);
            ts << string;
        }
    filebibleqt.close();
}
//---------------------------------
QString Export::exportChapter (QString filename, QString i, bool chapt)
{
    //    qDebug() << "--- i " << i;
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

                    //            qDebug() << "\n --------str = " << str;
                    QStringList strlist;
                    strlist << str.split("\n");


                    str = getHtmlCoolCode(str, i, tr("Chapter"), chapt);
                    //            qDebug() << "\n\nDebug: _Export::exportChapter" << " strlist = " << strlist;

                    //            QRegExp title("<title>  [1]</title>");
                    //            QRegExp title2("<title>[1]</title>");
                    //            QRegExp rx("(<[^>]*>)");
                    //            str.replace("<P>","<p>");
                    //            QRegExp rxp("(<[Pp].*?>)");
                    //            QRegExp rxi("( [a-zA-Z:]+=)|(\"[^\"]*\"|'[^']*')");

                    ////            if (chapt)
                    ////            {
                    //                //                qDebug() << "************ Export: chapter-file";
                    //                QString titlec = QString("<title>%1</title>").arg(incstr(i,GL_LENGTITEMSTRING," "));
                    //                QString titlec2 = QString("<title>%1</title>").arg(i);
                    //                QString chapter = tr("\n?h4_.Chapter %1?/h4_.").arg(incstr(i,GL_LENGTITEMSTRING," "));

                    //                //                qDebug() << "___int  = " << i << "str = "<< str;
                    //                str.replace(titlec,chapter)
                    //                        .replace(titlec2,chapter);
                    ////            }
                    ////            else
                    ////            {
                    //                str.remove(title)
                    //                        .remove(title2);

                    //                //                qDebug() << "************ Export: book-file";
                    ////            }

                    //            // перед rxp надо достать <center>

                    ////            str = getCenterTag(str);
                    //            str.remove("p, li { white-space: pre-wrap; }")
                    //                    .remove(title)
                    //                    .replace(rxp, "?p_.")
                    //                    .remove("</p>")
                    //                    .remove(rxi);

                    //            QStringList tags;

                    //            tags << "p" << "br /" << "h4" << "/h4" << "pre" << "/pre" << "span"
                    //                 << "/span" << "font" << "/font" << "sup" << "/sup" << "sub" << "/sub" << "center"
                    //                 << "/center" << "strong" << "/strong" << "em" << "/em" << "table" << "/table"
                    //                 << "tr" << "tr" << "/tr" << "td" << "td" << "/td" << "th" << "th" << "/th" << "hr /" ;

                    //            str = editStringList(str, tags, true); // сохраняем нужные теги, заменой на ?tag_.
                    //            str.remove(rx)
                    //                    .remove("")
                    //                    .remove("\n");
                    //            str.replace("?p_.PathName","\nPathName")
                    //                    .replace("PathName", "\n\nPathName")
                    //                    .replace("FullName", "\nFullName")
                    //                    .replace("ShortName", "\nShortName")
                    //                    .replace("ChapterQty", "\nChapterQty");
                    //            str = editStringList(str, tags, false); // возвращаем нужные теги


                }
        }
    else
        qDebug() << "Error exist";
    file.close();
    return str;
}
//---------------------------------
QString Export::getNameFolder(QString cur_dir)
{
    QStringList path = cur_dir.split("/");
    path.removeLast();
    return path.last();
}
//---------------------------------

