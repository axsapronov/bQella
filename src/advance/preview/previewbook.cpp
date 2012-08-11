#include "previewbook.h"
#include "ui_previewbook.h"

#include "pcommon.h"
#include "config.h"

PreviewBook::PreviewBook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewBook)
{
    ui->setupUi(this);

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
    QString text = getTextFromFile(filepath);

    ui->tBHtmlBook->setPlainText(text);
    //    ui->tBHtmlBook->setPlainText(text);
    ui->tBViewBook->setHtml(text);


    /// if dir is exist then remove files in dir
    /// if not create folder
    QString path = Config::configuration()->CurPrjDir();
    QString outputPath = path + "/" + "_Preview_/";

    qDebug() << "outputPath = " << outputPath;
    QDir dir(outputPath);
    if (!dir.exists())
    {
        /// create folder
        QString last = QString(outputPath).remove(outputPath.length()-1,1);
        last =  QString(last).split("/").last();
        QString dira = QString(outputPath).remove("/" + last);
        QDir dir2(dira);
        dir2.mkdir(last);
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
///-------------------------------------------------------------------
void PreviewBook::reject()
{
    qDebug() << "reject";
//    removePreviewFiles();
    QWidget::hide();  //close dialog
}
///-------------------------------------------------------------------
void PreviewBook::accept()
{
    qDebug() << "accept";
    QWidget::hide();  //close dialog
}
///-------------------------------------------------------------------
void PreviewBook::removePreviewFiles()
{
    QString outputPath = Config::configuration()->CurPrjDir() + "/" + "_Preview_/";
    qDebug() << "outputPath = " << outputPath;
    QDir dir(outputPath);
    if (dir.exists())
    {
        /// get path and outpath
        QString dira = QString(outputPath).remove(QString("/") + QString("_Preview_"));
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
