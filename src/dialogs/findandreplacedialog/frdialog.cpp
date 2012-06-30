#include "frdialog.h"
#include "ui_frdialog.h"

#include "config.h"

#include <QComboBox>
#include <QTableWidget>
#include <QDesktopServices>
#include <QPushButton>
#include <QStringList>
#include <QUrl>
#include <QtGui>
#include <QString>
#include <QStringListModel>
#include <QRegExp>

// ПЕРЕДЕЛАТЬ (библиотеки)

FRDialog::FRDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FRDialog)
{
    ui->setupUi(this);

    ui->tableFiles->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
    ui->tableFiles->verticalHeader()->hide();
    ui->tableFiles->setShowGrid(false);

    connect(ui->pBFind, SIGNAL(clicked()), this, SLOT(find()));
    connect(ui->pBReplace, SIGNAL(clicked()), this, SLOT(replace()));

//    connect(ui->cBBook, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateCBChapter(QString)));
    connect(ui->cBBook, SIGNAL(activated(QString)), this, SLOT(updateinfo()));

    updateCBChapter(Config::configuration()->PrjDir());
    updateCBBook();
}
//---------------------------------------
FRDialog::~FRDialog()
{
    delete ui;
};
//---------------------------------------
void FRDialog::replace()
{
    // файлы найдены
    // получаем список файлов
    // открываем файлы и делаем QString.replace
}
//---------------------------------------
void FRDialog::find()
{
    ui->tableFiles->setRowCount(0);

//    QString fileName = fileComboBox->currentText();
//    QString text = textComboBox->currentText();
//    QString path = directoryComboBox->currentText();

    QString fileName = "";
    QString text = ui->lEFind->text();

    QString path = Config::configuration()->CurPrjDir();
    // path должен составляться из комбобокса


    currentDir = Config::configuration()->CurPrjDir();/*QDir(path);*/
    QStringList files;
    if (fileName.isEmpty())
        fileName = "*";
    files = currentDir.entryList(QStringList(fileName),
                                 QDir::Files | QDir::NoSymLinks);

    if (!text.isEmpty())
        files = findFiles(files, text);
    showFiles(files);
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
    return foundFiles;
}
//---------------------------------------
void FRDialog::updateinfo()
{
    QString str;
    str = Config::configuration()->PrjDir() + ui->cBBook->currentText();
    qDebug() << "str = " << str;
    updateCBChapter(str);
}
//---------------------------------------
void FRDialog::updateCBChapter(QString path)
{
    qDebug() << "path = " << path;
    QStringList items;
    items << tr("Все главы");

    QDir dir(path);
    QStringList list = dir.entryList();
    list.removeFirst();
    list.removeFirst();
    list.removeFirst();

    // убрать из списка .pem файлы
    // и book файлы
//    qDebug() << "Debug: _FRDialog::updateCBChapter()" << "list = " << list << "prj = " << Config::configuration()->PrjDir();
    items.append(list);

    QStringListModel * typeModel2 = new QStringListModel(items, this);
    ui -> cBChapter -> setModel(typeModel2);
}
//---------------------------------------
void FRDialog::updateCBBook()
{
    QStringList items;
    items << tr("Все книги");

    QDir dir(Config::configuration()->PrjDir());
    QStringList list = dir.entryList();
    list.removeFirst();
    list.removeFirst();
//    qDebug() << "Debug: _FRDialog::updateCBBook()" << "list = " << list << "prj = " << Config::configuration()->PrjDir();
    // в цикле менять главы при выборе книг
    items.append(list);

    QStringListModel * typeModel2 = new QStringListModel(items, this);
    ui -> cBBook -> setModel(typeModel2);

}
//---------------------------------------
void FRDialog::showFiles(const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = ui->tableFiles->rowCount();
        ui->tableFiles->insertRow(row);
        ui->tableFiles->setItem(row, 0, fileNameItem);
        ui->tableFiles->setItem(row, 1, sizeItem);
    }
    ui->filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size()) +
                             (" (Double click on a file to open it)"));
}



