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


FRDialog::FRDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FRDialog)
{
    ui->setupUi(this);

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

    // файлы найдены
    // получаем список файлов
    // открываем файлы и делаем QString.replace

    for (int row = 0; row < ui->tableFiles->rowCount(); row++)
    {
        // составляем путь до файла
//        qDebug() << "\ncurrent row = " << row << "item ij = " <<  ui->tableFiles->item(row,1)->text() << "item ji = " << ui->tableFiles->item(row,0)->text();
        QString filepath = Config::configuration()->CurPrjDir() + "/book_" + ui->tableFiles->item(row,0)->text() + "_chapter_" + QString(ui->tableFiles->item(row,1)->text()).remove(tr("Chapter "))+".htm";
//        qDebug() << "filepath = " << filepath;

        // заменяем в файлах
        replaceTextOfFile(filepath, ui->LEFind->text(), ui->LEReplace->text());

    }
}

//---------------------------------------
void FRDialog::find()
{
    ui->tableFiles->setRowCount(0);
    QString fileName = "";
    QString text = ui->LEFind->text();
    QString path;

    if (ui->cBBook->currentIndex() == 0) // and ui->cBChapter->currentText() == "Все главы")
        path = Config::configuration()->CurPrjDir();

//    qDebug() << "if1 = " << path;
    if (ui->cBBook->currentIndex() != 0)
    {
//        qDebug() << "1";
        if (ui->cBChapter->currentIndex() == 0)
        {
            path = Config::configuration()->CurPrjDir() + "/"  + ui->cBBook->currentText();
//            qDebug() << "2";
        }
        else
        {
//            qDebug() << "3";
            QString str = ui->cBChapter->currentText();
            QString str2 = ui->cBBook->currentText();
//            qDebug() << "___" << str2.split("_").first();
//            qDebug() << "---" << str.replace(tr("Chapter "), tr("book_%1_chapter_").arg(str2));
            path = Config::configuration()->CurPrjDir() + "/" + str.replace(tr("Chapter "), tr("book_%1_chapter_").arg(str2)) +".htm";
//            qDebug() << "path = " <<  path /*<< "string " << stringforfoldername*/;
        }
    }
//    qDebug() << "4";
//    qDebug() << "Debug: _FRDialog::find()" << "_path = " << path;
    currentDir = Config::configuration()->CurPrjDir();/*QDir(path);*/
    QStringList files;
    if (fileName.isEmpty())
        fileName = "*";
    files = currentDir.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);
    if (!text.isEmpty())
        files = findFiles(files, text);

    QStringList bookList = files;
    QStringList chapterList = files;

    updateItemforTable(bookList, chapterList);
//    updateItemforTableChapter(chapterList);
//    showFiles(files);
//    qDebug() << " test " << bookList << chapterList;
    showFiles(bookList, chapterList);
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
//    qDebug() << "Debug: _FRDialog::updateinfo()" << "currenttext = " << ui->cBBook->currentText() << "currentindex = " << ui->cBBook->currentIndex();
    if (ui->cBBook->currentIndex() == 0)
        str = Config::configuration()->PrjDir();
    else
        str = Config::configuration()->CurPrjDir();
//    qDebug() << "str = " << str;
    updateCBChapter(str);
}

//---------------------------------------
void FRDialog::updateCBBook()
{
    QStringList items;
    items << tr("All books");

    QDir dir(Config::configuration()->CurPrjDir());
    QStringList list = dir.entryList();
    removeItemListBook(list);
//    qDebug() << "Debug: _FRDialog::updateCBBook()" << "list = " << list << "prj = " << Config::configuration()->PrjDir();
    items.append(list);
    QStringListModel * typeModel2 = new QStringListModel(items, this);
    ui -> cBBook -> setModel(typeModel2);
}
//---------------------------------------
void FRDialog::updateCBChapter(QString path)
{
//    qDebug() << "Debug: _FRDialog::updateCBChapter()" << "path = " << path;
    QStringList items;
    items << tr("All chapters");
    QDir dir(path);
    QStringList list = dir.entryList();
    removeItemListChapter(list);
//    qDebug() << "Debug: _FRDialog::updateCBChapter()" << "list = " << list ;
    items.append(list);

    QStringListModel * typeModel2 = new QStringListModel(items, this);
    ui -> cBChapter -> setModel(typeModel2);
}
//---------------------------------------
void FRDialog::removeItemListChapter(QStringList &list)
{
#ifdef Q_OS_WIN
    list.removeOne(QString("export_%1").arg( QString(list.at(2)).remove(".pem")));
#else
    list.removeOne(QString("export_%1").arg( QString(list.at(3)).remove(".pem")));
#endif
//    qDebug() << "Debug: _FRDialog::removeItemList()" << "list = " << list;
    QStringList listb;
    QString bookname;
    int i = 4;
    while (i < list.size())
    {
        if ( list.at(i).indexOf(".pem") >= 0  or (  list.at(i).indexOf(ui->cBBook->currentText()) <=0 and
                   list.at(i).indexOf("chapter") <= -1)  )
        {
//            qDebug() << "Debug: _FRDialog::removeItemList()" << "if = " << list.at(i);
            bookname = list.at(i);
            list.removeAt(i);
            --i;
        }
        QString app = QString(list.at(i)).remove(bookname.split(".").first()).replace(tr("book_%1_chapter_").arg(ui->cBBook->currentText()), tr("Chapter ")).remove(".htm");
        listb << app;
//        qDebug() << "Debug: _FRDialog::removeItemList()" << "listb = " << listb << "app = " << app;
        ++i;
    }
    listb.removeOne(QString(Config::configuration()->CurProject().split("/").last()));
    listb.removeOne("book_"+ui->cBBook->currentText());
    listb.removeOne("");

    list = listb;
//    qDebug() << "Debug: _FRDialog::removeItemList()" << "list = " << listb;
}

//---------------------------------------
void FRDialog::removeItemListBook(QStringList &list)
{
    QStringList listb;
    int i = 4;
    while (i < list.size())
    {
        if (
                ( list.at(i).indexOf(".pem") >= 0 ) or
                ( list.at(i).indexOf("chapter") >= 0) or
                ( list.at(i).indexOf("export") >= 0 )
           )
        {
            list.removeAt(i);
            --i;
        }
        else
        {
            QString app = QString(list.at(i)).remove("book_").remove(".htm");
            listb << app;
        }
        ++i;
    }
    listb.removeOne(".pem");
    listb.removeOne("");
    listb.removeOne("help");
    listb.removeDuplicates();
    list = listb;
//    qDebug() << "Debug: _FRDialog::removeItemList()" << "list = " << listb;
}
//---------------------------------------
void FRDialog::updateItemforTable(QStringList &list, QStringList &list2)
{
    QStringList lib, lib2;
    QString app, app2;

    for (int i = 0; i < list.size(); i++)
    {
        QRegExp rx("_chapter_\\d{1,3}");
        app = QString(list.at(i)).remove(rx).remove("book_").remove(".htm");
        lib << app;
        QStringList str;
        QString st = QString(list.at(i));
        (str << st.split("_")).removeFirst();
        app2 = QString(list.at(i)).remove(tr("book_%1").arg(str.first())).replace("_chapter_", tr("Chapter ")).remove(".htm");
        lib2 << app2;
    }
//    qDebug() << " lib = " << lib;
//    qDebug() << " lib2 = " << lib2;
    list = lib;
    list2 = lib2;
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
void FRDialog::on_buttonBox_accepted()
{
    ui->LEFind->setText("");
    ui->LEReplace->setText("");
    ui->tableFiles->clearContents();
}
//---------------------------------------
void FRDialog::on_buttonBox_rejected()
{
    ui->LEFind->setText("");
    ui->LEReplace->setText("");
    ui->tableFiles->clearContents();
}
//---------------------------------------
