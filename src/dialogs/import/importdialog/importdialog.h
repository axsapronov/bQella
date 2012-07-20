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

#ifndef IMPORT_H
#define IMPORT_H

#include "projectproperty.h"
#include "ui_importdialog.h"

class QDialog;
class QOBject;


/**
@class Import
class required to import modules.
It also contains three features that are needed for the class importbookdialog (Import book separately)
*/
class Import : public QDialog
{
    Q_OBJECT
public:
    /// The default constructor
    explicit Import(QWidget *parent = 0);

    /**
    @function
    Edit project file (projectname.pem). Add to end two strings:"
    </contents>
    </pemproject>"
    @param  filename  project filename
    */
    void addContentToEndProjectFile(QString filename); // для импорта отдельно книги

    /**
    @function
    The import feature of the book separately.
    In contrast to the usual import of the book takes another project file
    @param  projectfile  path to project file
    @param  pathName  path to book file
    @param  FullName  full name for book (bibleqt param)
    @param  ShortName  short name for book (bibleqt param)
    @param  ChapterQty  count chapters in book (bibleqt param)
    @param  myChapterSign  tag for chapters (bibleqt param)
    */
    void importBook(QString projectfile, QString pathName, QString FullName, QString ShortName, int ChapterQty,QString myChapterSign); // для импорта книги

    /**
    @function
    Edit project file (projectname.pem). Add text to project file(option for load project (book, chapters))
    @param  filename project filename
    @param  text text
    @param  tr chapter or book
    */
    void addContentToProjectFile(QString filename, QString text, bool tr); // filename чтобы работал с импортом отдельно книги

    /**
    @function
    Reeturn project filename
    */
    QString getPrjFN();

    /**
    @function
    Return start page for project
    */
    QString getStartPage();

    /**
      @function
      Set htmlfilter ( tags will be removed upon import)
      @param removetags  tags will be removed upon import
      */
    void setHtmlFilter(QString removetags) { htmlfilter = removetags; }

signals:
    void SuccessfulImport();

public slots:
    /**
      @function
      Start import module
      @param bibleqtinifile  path to bibleqt.ini file
      */
    void importModule(QString bibleqtinifile);

private slots:

    /**
      @function
      Browse bibleqt.ini file
      */
    void selectImportFile();

private:
    Ui::ImportDialog ui;
    int BookQty;

    QString VerseSign;
    QString ChapterSign;
    QString encoding;
    QString htmlfilter;

    void accept();

    /**
    @function
    Import ini file (input bibleqt.ini)
    @param file  ini filename
    */
    void importIni(QString file);

    /**
    @function
    Import project file (create project file, and write options, first tags)
    */
    void importProjectFile();

    /**
    @function
    The import book.
    @param  pathName  path to book file
    @param  FullName  full name for book (bibleqt param)
    @param  ShortName  short name for book (bibleqt param)
    @param  ChapterQty  count chapters in book (bibleqt param)
    @param  myChapterSign  tag for chapters (bibleqt param)
    */
    void importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty);

    /**
    @function
    Import chapter, parse text (remove tags)
    @param  file  path to file
    @return text
    */
    QString importChapter(QString file);

    /**
      @function
      Create import folder of path
      @param path  project folder
      */
    void createImportFolder(QString path);

    /**
      @function
      Creates a file with the data book
      @param pathName  path to book file
      @param fullName  param bibleqt
      @param ShortName  param bibleqt
      @param ChapterQty  count chapters (param bibleqt)
    */
    void createBookFile(QString pathName, QString FullName, QString ShortName, int ChapterQty);

    /**
      @function
      Creates a file with the data chapter
      @param text  text
      @param file  path to chapter file  (book_namebook_chapter_count)
      @param count  number of chapter
    */
    void createChaterFile(QString file,QString text, int count);

    /**
      @function
      Create project file with the first data (bibleqt.ini param)
      */
    void createProjectFile();
    /**
      @function
      Create file    ___Instruction in project path (first file)
      */
    void createInstructionFile();

    /**
    Edit project file (projectname.pem). Add text to project file(option for load project (book, chapters))
    @param  text text
    @param  tr chapter or book
      */
    void addContentToProjectFile(QString text, bool tr);

    /**
      @function
    Edit project file (projectname.pem). Add to end two strings:"
    </contents>
    </pemproject>"
    */
    void addContentToEndProjectFile();

};

#endif // IMPORT_H
