#include "previewbook.h"
#include "ui_previewbook.h"

#include "pcommon.h"
#include "filecommon.h"
#include "config.h"
#include "importdialog.h"

#include <QStringListModel>

PreviewBook::PreviewBook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewBook)
{
    ui->setupUi(this);
    createConnects();
}
///------------------------------------------------
PreviewBook::~PreviewBook()
{
    delete ui;
}
///------------------------------------------------
void PreviewBook::setData(QString filepath)
{
    ui->LAViewFile->setText(filepath);
    ui->LAViewFile_2->setText(filepath);

    setPathToBook(filepath);
//    qDebug() << "getEncoding()" << getEncoding();
    QString text = getTextFromFile(filepath, getEncoding());

    ui->tBHtmlBook->setPlainText(text);
    ui->tBViewBook->setHtml(text);


    QString path = Config::configuration()->PrjDir()+"_Preview_/";
    QDir dir(path);
    dir.mkdir(path);


    /// if dir is exist then remove files in dir
    /// if not create folder
    QString folderpath = Config::configuration()->PrjDir() +
            "_Preview_" +
            "/" +
            "_Preview_/";
    createFolder(folderpath);
}
///-------------------------------------------------------------------
//void PreviewBook::reject()
//{
//    //    qDebug() << "reject";
//        removePreviewFiles();
//    QWidget::hide();  //close dialog
//}
///-------------------------------------------------------------------
void PreviewBook::accept()
{
    removePreviewFiles();

    disconnect(ui->comBChapters_1, SIGNAL(currentIndexChanged(int)), this, SLOT(showChapter(int)));
    disconnect(ui->comBChapters_2, SIGNAL(currentIndexChanged(int)), this, SLOT(showChapter(int)));
    ui->comBChapters_1->clear();
    ui->comBChapters_2->clear();
    ui->tBHtmlBook->clear();
    ui->tBHtmlChapter->clear();
    ui->tBViewBook->clear();
    ui->tbViewChapter->clear();

    emit deletePreviewFolder();

    QWidget::hide();  //close dialog
}
///-------------------------------------------------------------------
void PreviewBook::removePreviewFiles()
{
    QString outputPath = Config::configuration()->PrjDir() + "_Preview_/_Preview_";
//    qDebug() << "outputpath = " << outputPath;
    QDir dir(outputPath);
    if (dir.exists())
    {
        /// get path and outpath
//        QString dira = QString(outputPath).remove(QString("/") + QString("_Preview_"));
        QString dira = Config::configuration()->PrjDir() + "_Preview_/";
        QDir dir2(dira);
        /// get file list and remove files
        QStringList lstFiles = dir.entryList(QDir::Files);
        foreach (QString entry, lstFiles)
        {
            QString entryAbsPath = dir.absolutePath() + "/" + entry;
            QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
            QFile::remove(entryAbsPath);
        }
        dir2.rmdir("_Preview_");
    }
}
///-------------------------------------------------------------------
void PreviewBook::createBookPreviewFunc()
{
    QStringList chapters = getChapterComboText();

    /// add list to comboBox
    ui->comBChapters_1->addItems(chapters);
    ui->comBChapters_2->addItems(chapters);
    createConnects();
    if (!chapters.isEmpty())
        showChapter(0);
}
///------------------------------------------------------------------
void PreviewBook::showChapter(int count)
{
    QString textchapter = getTextFromFile(getChapterList().at(count), getEncoding());
//    QString textchapter = getTextFromFile(getChapterList().at(count), "UTF-8");
    ui->comBChapters_1->setCurrentIndex(count);
    ui->comBChapters_2->setCurrentIndex(count);
    ui->tBHtmlChapter->setPlainText(textchapter);
    ui->tbViewChapter->setHtml(textchapter);
}
///------------------------------------------------------------------
void PreviewBook::createFolder(QString folderPath)
{
//    qDebug() << folderPath;
    QDir dir(folderPath);
    if (!dir.exists())
    {
        /// create folder
        dir.mkdir(folderPath);
    }
    else
    {
        /// get file list and remove files
        QStringList lstFiles = dir.entryList(QDir::Files);
        foreach (QString entry, lstFiles)
        {
            QString entryAbsPath = dir.absolutePath() + "/" + entry;
            QFile::setPermissions(entryAbsPath, QFile::ReadOwner | QFile::WriteOwner);
            QFile::remove(entryAbsPath);
        }
    }
}
///---------------------------------------------------------------
void PreviewBook::createConnects()
{
    connect(ui->comBChapters_1, SIGNAL(currentIndexChanged(int)), this, SLOT(showChapter(int)));
    connect(ui->comBChapters_2, SIGNAL(currentIndexChanged(int)), this, SLOT(showChapter(int)));
}
