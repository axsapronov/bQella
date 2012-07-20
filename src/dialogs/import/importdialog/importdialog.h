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


    void addContentToEndProjectFile(QString filename); // для импорта отдельно книги

    /// The import feature of the book separately. In contrast to the usual import of the book takes another project file
    /// @param  projectfile – path to project file
    /// @param  pathName – path to book file
    /// @param  FullName – full name for book (bibleqt param)
    /// @param  ShortName – short name for book (bibleqt param)
    /// @param  ChapterQty – count chapters in book (bibleqt param)
    /// @param  myChapterSign – tag for chapters (bibleqt param)
    /// @return void
    void importBook(QString projectfile, QString pathName, QString FullName, QString ShortName, int ChapterQty,QString myChapterSign); // для импорта книги

    void addContentToProjectFile(QString filename, QString text, bool tr); // filename чтобы работал с импортом отдельно книги

    QString getPrjFN();
    QString getStartPage();
signals:
    void SuccessfulImport();

public slots:
    void importModule(QString file);

private slots:
    void selectImportFile();
private:
    Ui::ImportDialog ui;
    int BookQty;
    QString VerseSign;
    QString ChapterSign;

    QString encoding;
    QString htmlfilter;

    void accept();
    void importIni(QString file);
    void importProjectFile();
    void importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty);
    QString importChapter(QString file);

    void createImportFolder(QString path);
    void createBookFile(QString pathName, QString FullName, QString ShortName, int ChapterQty);
    void createChaterFile(QString file,QString text, int i);
    void createProjectFile();
    void createInstructionFile();
    void addContentToProjectFile(QString text, bool tr);
    void addContentToEndProjectFile();

};

#endif // IMPORT_H
