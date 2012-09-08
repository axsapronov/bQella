#include "previewmodule.h"
#include "ui_previewmodule.h"

#include "previewbook.h"
#include "filecommon.h"
#include "pcommon.h"
#include <QDir>
#include <QDebug>

PreviewModule::PreviewModule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewModule)
{
    ui->setupUi(this);
    connect(ui->pBPreviewBook, SIGNAL(clicked()), this, SLOT(showPreviewBook()));
//    showPreviewBook();
    prevbook = new PreviewBook();
}
///-----------------------------------------------------------------------------------------------
PreviewModule::~PreviewModule()
{
    delete ui;
}
///------------------------------------------------------------------------------------------------
void PreviewModule::setData(QString filepath)
{
    ui->LAFile->setText(filepath);
    setFileBibleqtIni(filepath);

    ///
    createListBook();
}
///------------------------------------------------------------------------------------------------
void PreviewModule::showPreviewBook()
{
    QStringList listfiles = getListFilesFromBibleqtIni(getFileBibleqtIni());
    QStringList listvaluetext = getListValueTextFromBibleqtIni(getFileBibleqtIni(),
                                                               getEncoding());
    QString book = listfiles.at(ui->ComBBooks->currentIndex());
    prevbook->setEncoding(getEncoding());
    prevbook->setData(book);
    emit createBookPreviewModule(listvaluetext.at(ui->ComBBooks->currentIndex()));
    prevbook->createBookPreviewFunc();
    prevbook->show();
}
///------------------------------------------------------------------------------------------------
void PreviewModule::createPreview()
{
    createFolder(prjDir + "/");
    createBookPreview();
}
///------------------------------------------------------------------------------------------------
void PreviewModule::createFolder(QString path)
{
    QDir dir(path);
//    qDebug() << " path = " << path;
    dir.mkdir("_Preview_");
}
///------------------------------------------------------------------------------------------------
void PreviewModule::createBookPreview()
{

}
///------------------------------------------------------------------------------------------------
void PreviewModule::accept()
{
//    qDebug() << "accept";
    removePreviewFiles();
    QWidget::hide();  //close dialog
}
///------------------------------------------------------------------------------------------------
void PreviewModule::reject()
{
//    qDebug() << "reject";
    QWidget::hide();  //close dialog
}
///------------------------------------------------------------------------------------------------
void PreviewModule::removePreviewFiles()
{
    QString outputPath = prjDir  + "_Preview_/";
//    qDebug() << "outputPath = " << outputPath;
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
///------------------------------------------------------------------------------------------------
void PreviewModule::createListBook()
{
    QStringList listvaluetext = getListValueTextFromBibleqtIni(getFileBibleqtIni(),
                                                               getEncoding());
    ui->ComBBooks->clear();
    ui->ComBBooks->addItems(listvaluetext);
//    ui->ComBBooks->model()->sort(0);
}
