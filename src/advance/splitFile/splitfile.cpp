#include "splitfile.h"
#include "ui_splitfile.h"
#include "config.h"
#include "pcommon.h"
#include "filecommon.h"

#include <QFileDialog>
#include <QFile>
#include <QStringListModel>

SplitFile::SplitFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplitFile)
{
    ui->setupUi(this);

    createConnect();

    setData();
//    QString str = "/home/files/Documents/Bible/unrar/my/1Co.htm";
//    str = "/home/files/Documents/Bible/unrar/NT_Russian_Kassian/57_philippians.htm";
//    ui->LEFilePath->setText(str);

    QString tag = "<h4>";
//     QString tag = "<A NAME";
    ui->LETagSplit->setText(tag);

//    QStringListModel *modelEncoding;
//    modelEncoding = new QStringListModel(getFillEncoding(), this);
//    ui->comBEncoding->setModel(modelEncoding);

    //    showFileHtml(str);
//    ui->cBAutoOn->setChecked(false);
    ui->cBAutoOn->setChecked(true);
    AutoSplitOn();
    showFileText();
        AutoEstimate();
    AutoRun();
}
//--------------------------------------------
SplitFile::~SplitFile()
{
    delete ui;
}
//--------------------------------------------
void SplitFile::createConnect()
{
    /// manual
    connect(ui->pBBrowse, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui->pBShow, SIGNAL(clicked()), this, SLOT(showFileText()));

//    connect(ui->comBEncoding, SIGNAL(currentIndexChanged(int)), this, SLOT(showFileText()));

    /// auto
    connect(ui->cBAutoOn, SIGNAL(stateChanged(int)), this, SLOT(AutoSplitOn()));
    connect(ui->pBAutoRun, SIGNAL(clicked()), this, SLOT(AutoRun()));
    connect(ui->pBAutoBrowse, SIGNAL(clicked()), this, SLOT(AutoBrowse()));
    connect(ui->pBAutoEstimate, SIGNAL(clicked()), this, SLOT(AutoEstimate()));
    connect(ui->pBSave, SIGNAL(clicked()), this, SLOT(saveTextEdit()));

    /// auto refresh if edit text
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(refreshTextHtml()));
}
//--------------------------------------------
void SplitFile::browse()
{
    QString beginpath = Config::configuration ()->AppDir ();
    //    QString beginpath = "/home/files/Documents/Bible/unrar/NT_Greek_WH-E_UTF8";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select file"),
                                                    beginpath,
                                                    tr("Html Files (*.html *.htm);;All Files (*)"));
    if (!fileName.isNull())
    {
        ui->LEFilePath->setText(fileName);
        ui->LAEncoding->setText(getEncodingFromFile(fileName));
        showFileText();
        refreshAutoInfo();
    }
}
//--------------------------------------------
void SplitFile::setData()
{

}
//--------------------------------------------
void SplitFile::showFileText()
{
    setEncoding(ui->LAEncoding->text());
    TextOfFile = getTextFromFile(ui->LEFilePath->text(), getEncoding());
    showFileHtml(TextOfFile);
    showFileEdit(TextOfFile);
}

////--------------------------------------------
//void SplitFile::showFileHtml(QString filepath)
//{
//    if(QFile::exists(filepath))
//    {
//        //        QString filetext = getTextFromFile(filepath);
//        ui->textBrowser->setHtml(TextOfFile);
//    }
//}
//--------------------------------------------
void SplitFile::showFileHtml(QString text)
{
    ui->textBrowser->setHtml(text);
}
//--------------------------------------------
//void SplitFile::showFileEdit(QString filepath)
//{
//    if(QFile::exists(filepath))
//    {
//        //        QString filetext = getTextFromFile(filepath);
//        ui->textEdit->setPlainText(TextOfFile);
//    }
//}
void SplitFile::showFileEdit(QString text)
{
    ui->textEdit->setPlainText(text);
}
//-----------------------------------------------
void SplitFile::AutoSplitOn()
{
    bool flag = ui->cBAutoOn->isChecked();
    QString filepath = ui->LEFilePath->text();
    ui->LAInputFile->setText(filepath);

    QString outputpath = QString(filepath)
            .remove(QString(filepath).split("/").last())
            + "Output/";
    ui->LEDirOutput->setText(outputpath);

    /// visible
    ui->LAInputFile->setVisible(flag);
    ui->LAValueFiles->setVisible(flag);
    ui->LEDirOutput->setVisible(flag);
    ui->LETagSplit->setVisible(flag);

    ui->label_5->setVisible(flag);
    ui->label_6->setVisible(flag);
    ui->label_7->setVisible(flag);
    ui->label_8->setVisible(flag);

    ui->pBAutoBrowse->setVisible(flag);
    ui->pBAutoEstimate->setVisible(flag);
    ui->pBAutoRun->setVisible(flag);
    if (flag)
        AutoEstimate();

}
//-----------------------------------------------
void SplitFile::AutoEstimate()
{
    TagOfFile = ui->LETagSplit->text();
    TextOfFile = ui->textEdit->toPlainText();
    countFiles = countTheNumberOfFiles(&TextOfFile, TagOfFile);
    ui->LAValueFiles->setText(QString::number(countFiles));
}
//-----------------------------------------------
void SplitFile::AutoBrowse()
{
    QString firstpath = Config::configuration()->AppDir();
    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Browse path for ouput"),
                                                          firstpath,
                                                          options);
    if (!directory.isEmpty())
        ui->LEDirOutput->setText(directory);
}
//-----------------------------------------------
void SplitFile::AutoRun()
{
    AutoEstimate();

    /// if dir is exist then remove files in dir
    /// if not create folder
    QString outputPath = ui->LEDirOutput->text();
    QDir dir(outputPath);
    if (!dir.exists())
    {
        /// create folder
        dir.mkpath(outputPath);
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

    /// split file
    QString filename, text;
    QString textinput = TextOfFile;
    int pos, posNext;

    ///first delete
    pos = textinput.indexOf(TagOfFile);
    text = QString(textinput).remove(pos, textinput.length() - pos);

    if (
            text != " " &&
            text != "\n" &&
            text != TagOfFile
       )
        textinput.remove(text);

    for (int i = 1; i < countFiles+1; i++)
    {
        /// create files
        filename = outputPath + "output_file_" + QString::number(i) + ".htm";

        /// detect pos of tag
        /// and split block
        pos = textinput.indexOf(TagOfFile)  ;
        if (pos == -1)
        {
            pos = 0;
            text = textinput;
        }
        else
        {
            posNext = textinput.indexOf(TagOfFile, pos+TagOfFile.length());
            text = textinput.mid(pos, posNext-TagOfFile.length());
        }
        textinput.remove(text);
        createEmptyHtml(filename, QString::number(i), text);
    }
}
//-----------------------------------------------
void SplitFile::refreshTextHtml()
{
    /// get text from textedit
    /// and set to textbrowser
    QString text = ui->textEdit->toPlainText();
    showFileHtml(text);
}

//-----------------------------------------------
void SplitFile::saveTextEdit()
{
    /// get text, filepath
    /// remove file
    /// and create file with text
    /// (save)
    QString filepath = ui->LEFilePath->text();
    QString text = ui->textEdit->toPlainText();

    QFile file(filepath);
    file.close();
    file.remove();
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << text;
        file.close();
    }
}
//-----------------------------------------------
void SplitFile::refreshAutoInfo()
{
    QString filepath = ui->LEFilePath->text();
    ui->LAInputFile->setText(filepath);

    QString outputpath = QString(filepath)
            .remove(QString(filepath).split("/").last())
            + "Output/";
    ui->LEDirOutput->setText(outputpath);
    AutoEstimate();
}
