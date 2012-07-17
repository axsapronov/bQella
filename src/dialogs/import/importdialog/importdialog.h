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

class Import : public QDialog
{
    Q_OBJECT
public:
    explicit Import(QWidget *parent = 0);



signals:
    void SuccessfulImport();

public slots:

    void importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty);
    void importModule(QString file);
    void addContentToProjectFile(QString text, bool tr);

    QString getPrjFN();
    QString getStartPage();

private slots:
    void selectImportFile();

    void importIni(QString file);
    void importProjectFile();
    QString importChapter(QString file);

    void createImportFolder(QString path);
    void createChaterFile(QString file,QString text, int i);
    void createBookFile(QString pathName, QString FullName, QString ShortName, int ChapterQty);
    void createProjectFile();
    void createInstructionFile();

    void addContentToEndProjectFile();
    void on_buttonBox_accepted();


private:
    Ui::ImportDialog ui;
    int BookQty;
    QString VerseSign;
    QString ChapterSign;

    QString encoding;

};

#endif // IMPORT_H
