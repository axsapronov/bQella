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

#include "frdialog.h"
#include "ui_frdialog.h"

#include "config.h"
#include "pcommon.h"

#include <QProgressDialog>
#include <QStringListModel>
#include <QMessageBox>

FRDialog::FRDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FRDialog)
{
    ui->setupUi(this);

    ui->tableFiles->setSelectionBehavior(QAbstractItemView::SelectRows);  //settings for table
    ui->tableFiles->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tableFiles->verticalHeader()->hide();
    ui->tableFiles->setShowGrid(false);

    ui->LEFind->setText("");
    ui->LEReplace->setText("");
    ui->tableFiles->clearContents();

    connect(ui->pBFind, SIGNAL(clicked()), this, SLOT(find()));
    connect(ui->pBReplace, SIGNAL(clicked()), this, SLOT(replace()));
    connect(ui->cBBook, SIGNAL(activated(QString)), this, SLOT(updateinfo()));

    //    stringforfoldername = "";
    //    ui->LEFind->setText("<"); // закоментить
    updateCBChapter(Config::configuration()->PrjDir());
//    qDebug() << "path = " << Config::configuration()->PrjDir()
//             << " curpj = " << Config::configuration()->CurPrjDir();
    updateCBBook();

}
//---------------------------------------
FRDialog::~FRDialog()
{
    delete ui;
}
//---------------------------------------
void FRDialog::replace()
{

    QString s = "";  //holds list of errors
    bool er = false;
    if (ui->LEReplace -> text().isEmpty())
    {
            s.append(tr("- Please enter a replace text.\n"));
            er = true;
    }

    if (er)
    {
            QMessageBox::critical(this, tr("Replace error"), s);
    }
    else
    {
        // файлы найдены
        // получаем список файлов
        // открываем файлы и делаем QString.replace
        for (int row = 0; row < ui->tableFiles->rowCount(); row++)
        {
            QString namebook = ui->tableFiles->item(row,0)->text();
//            namebook = bookfiles.at(bookfiles.indexOf(namebook));
//            qDebug() << " at = " << "" << " index = " << ui->cBBook->findText(namebook)
//                     << " bookfiles  = " << bookfiles
//                     << " namebook = " << namebook;
            namebook = bookfiles.at(ui->cBBook->findText(namebook));
            QString namechapter = incstr(QString(ui->tableFiles->item(row,1)->text())
                                         .remove(tr("Chapter ")) ,
                                         GL_LENGT_ITEM_STRING,
                                         "_");
            // составляем путь до файла
            QString filepath = Config::configuration()->CurPrjDir() + "/book_" +
                    namebook+ "_chapter_" + namechapter +".htm";
            // заменяем в файлах
//            qDebug() << " filepath = " << filepath;
            replaceTextOfFile(filepath, ui->LEFind->text(), ui->LEReplace->text());
        }
    }
}

//---------------------------------------
void FRDialog::find()
{

    QString s = "";  //holds list of errors
    bool er = false;
    if (ui->LEFind -> text().isEmpty())
    {
            s.append(tr("- Please enter a find text.\n"));
            er = true;
    }

    if (er)
    {
            QMessageBox::critical(this, tr("Find error"), s);
    }
    else
    {

        ui->tableFiles->setRowCount(0);
        QString fileName = "";
        QString text = ui->LEFind->text();
        QString correctName;

        if (ui->cBBook->currentIndex() == 0)
            correctName = "";

        if (ui->cBBook->currentIndex() != 0)
        {
            if (ui->cBChapter->currentIndex() == 0)
            {
                QString namebook = bookfiles.at(ui->cBBook->currentIndex());
                correctName = "book_" + namebook;
            }
            else
            {
                QString namebook = bookfiles.at(ui->cBBook->currentIndex());
                QString namechapter = ui->cBChapter->currentText();
                namechapter
                        .remove(tr("Chapter "));
                namechapter = incstr(namechapter,GL_LENGT_ITEM_STRING, "_");
                correctName = "book_"+namebook+"_chapter_"+namechapter;
            }
        }
        currentDir = Config::configuration()->CurPrjDir();/*QDir(correctName);*/

        QStringList files1;
        if (fileName.isEmpty())
            fileName = "*";
        files1 = currentDir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

        QStringList files;
        if (!correctName.isEmpty())
        {
                for (int i = 0; i < files1.size(); i++)
                {
                    if(files1.at(i).indexOf(correctName) != -1)
                    {
                        files << files1.at(i);
                    }
                }
        }
        else
        {
            files = files1;
        }

        if (!text.isEmpty())
            files = findFiles(files, text);

        QStringList bookList = files;
        QStringList chapterList = files;
        updateItemforTable(bookList, chapterList);

//        qDebug() << "booklist = " << bookList
//                 << " chapterlist = " << chapterList
//                 << " bookfiles = " << bookfiles;
        showFiles(bookList, chapterList);
    }

}
//---------------------------------------
QStringList FRDialog::findFiles(const QStringList &files, const QString &text)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Find Files"));
    QStringList foundFiles;
    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 of %2...")
                                    .arg(i).arg(files.size()));
        qApp->processEvents();

        if (progressDialog.wasCanceled())
            break;

        QFile file(currentDir.absoluteFilePath(files[i]));
        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                if (progressDialog.wasCanceled())
                    break;
                line = in.readLine();
                if (line.contains(text)) {
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
    foundFiles.removeOne("   ___Instruction");
    foundFiles.removeOne(Config::configuration()->CurProject().split("/").last());
    return foundFiles;
}
//---------------------------------------
void FRDialog::updateinfo()
{
    QString str;
    if (ui->cBBook->currentIndex() == 0)
        str = Config::configuration()->PrjDir();
    else
        str = Config::configuration()->CurPrjDir();
    updateCBChapter(str);
}

//---------------------------------------
void FRDialog::updateCBBook()
{
    QStringList items;
    items << tr("All books");
    bookfiles.clear();
    bookfiles << tr("All books");

    QDir dir(Config::configuration()->CurPrjDir());
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QStringList list = dir.entryList();
    list.removeDuplicates();
    removeItemListBook(list);
    items.append(list);
    QStringListModel * typeModel2 = new QStringListModel(items, this);
    ui -> cBBook -> setModel(typeModel2);
}
//---------------------------------------
void FRDialog::updateCBChapter(QString path)
{
    QStringList items;
    items << tr("All chapters");
    QDir dir(path);
    dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    QStringList list = dir.entryList();
    list.removeDuplicates();
    removeItemListChapter(list);
    items.append(list);
    QStringListModel * typeModel2 = new QStringListModel(items, this);
    ui -> cBChapter -> setModel(typeModel2);
}
//---------------------------------------
void FRDialog::removeItemListChapter(QStringList &list)
{
    QStringList listb;
    QString bookname;
    int i = 1;
    list.removeDuplicates();
//    qDebug() << "list = " << list;
    while (i < list.size())
    {
        QString namebook = getFileNameAbs(bookfiles.at(ui->cBBook->currentIndex()));
        QString name = QString(list.at(i));

        if ( list.at(i).indexOf(GL_PROJECT_FILE) >= 0  or
             (  list.at(i).indexOf(namebook) <=0 and
                list.at(i).indexOf("chapter") <= -1)  )
        {
            bookname = list.at(i);
            list.removeAt(i);
            --i;
        }

        if (name.indexOf("book_"+namebook+"_") != -1)
        {
//            qDebug() << "name = " << name
//                     << " namebook = " << namebook
//                     << " listat(i) = " << list.at(i);
            QString namechapter = QString(name).remove("book_"+namebook+"_chapter");
            namechapter
                    .remove("_")
                    .remove(".htm");
            namechapter = tr("Chapter ") + namechapter;
            listb << namechapter;
        }
        ++i;
    }
    listb.removeOne(QString(Config::configuration()->CurProject().split("/").last()));
    listb.removeDuplicates();
    listb.removeOne("");
    list = listb;
}

//---------------------------------------
void FRDialog::removeItemListBook(QStringList &list)
{
    QStringList listb;
    int i = 1;
    while (i < list.size())
    {
        if (
                ( list.at(i).indexOf(GL_PROJECT_FILE) >= 0 ) or
                ( list.at(i).indexOf("chapter") >= 0) or
                ( list.at(i).indexOf("export") >= 0 )
                )
        {
            list.removeAt(i);
            --i;
        }
        else
        {
            QString filename = Config::configuration()->CurPrjDir() + "/" +
                    list.at(i);
//            qDebug() << "filename = " << filename
//                     << " param = " << getParamBook(filename, "FullName");
            QString bookfilename = QString(list.at(i)).remove("book_").remove(".htm");
            QString app = getParamBook(filename, "FullName");
            listb << app;
            bookfiles << bookfilename;
        }
        ++i;
    }
    listb.removeOne(GL_PROJECT_FILE);
    listb.removeOne("");
    listb.removeOne("help");
    listb.removeDuplicates();
    list = listb;
}
//---------------------------------------
void FRDialog::updateItemforTable(QStringList &booklist, QStringList &chapterlist)
{
    QStringList booklib, chapterlib;

    for (int i = 0; i < booklist.size(); i++)
    {
        QString name = getFileNameAbs(QString(booklist.at(i)));
        QString namebook = QString(name)
                .remove("book_")
                .remove("chapter_");
        QString namechapter = namebook;
        QStringList list;
        list = namebook.split("_");
        namebook = list.first();

        namechapter = QString(namechapter)
                .remove("_");
        namechapter = removeFirst(namechapter, namebook);

        QString filenamebook = Config::configuration()->CurPrjDir()
                + "/" + "book_" + namebook + ".htm";

        namechapter = tr("Chapter ") + namechapter;

        namebook = getParamBook(filenamebook, "FullName");
        booklib << namebook;
        chapterlib << namechapter;
    }
    booklist = booklib;
    chapterlist = chapterlib;
//    qDebug() << "\nlist1 = " << booklist
//             << "\n\n list2 = " << chapterlist;
}
//---------------------------------------
void FRDialog::showFiles(const QStringList &bookList, const QStringList &chapterList)
{

    for (int i = 0; i < bookList.size(); ++i)
    {
        QTableWidgetItem *bookItem = new QTableWidgetItem(bookList[i]);
        QTableWidgetItem *chapterItem = new QTableWidgetItem(chapterList[i]);
        int row = ui->tableFiles->rowCount();
        ui->tableFiles->insertRow(row);
        ui->tableFiles->setItem(row, 0, bookItem);
        ui->tableFiles->setItem(row, 1, chapterItem);
    }
    ui->filesFoundLabel->setText(tr("%1 file(s) found").arg(bookList.size()) /*+
                                 (" (Double click on a file to open it)")*/);
}
//---------------------------------------
void FRDialog::accept ()
{
    ui->LEFind->setText("");
    ui->LEReplace->setText("");
    ui->tableFiles->clearContents();
    QWidget::hide();
}
//---------------------------------------
void FRDialog::reject ()
{
    ui->LEFind->setText("");
    ui->LEReplace->setText("");
    ui->tableFiles->clearContents();
    QWidget::hide();
}
//---------------------------------------
