#include "importbookdialog.h"
#include "ui_importbookdialog.h"
#include "pcommon.h"
#include "filecommon.h"


#include "importdialog.h"
#include "previewbook.h"
#include "config.h"

#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QTextDocument>

ImportBookDialog::ImportBookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportBookDialog)
{
    ui->setupUi(this);

    importm = new Import();
    prevbook = new PreviewBook(this);
    connect(ui->pBBrowse, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui->pBProjectProperties, SIGNAL(clicked()), this, SLOT(showPropertiesDialog()));
    connect(ui->pBPreview, SIGNAL(clicked()), this, SLOT(showPreview()));
    connect(prevbook, SIGNAL(createBookPreview()), this, SLOT(createBookPreview()));
    connect(ui->pBEstimate, SIGNAL(clicked()), this, SLOT(estimate()));

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(deletePreviewFolder()));
    setData();
    ui->LETagChapter->setText("<h4>");
    ui->LETagVerse->setText("<p>");
//    debug();
}
///------------------------------------------------
ImportBookDialog::~ImportBookDialog()
{
    delete ui;
}
///------------------------------------------------
void ImportBookDialog::setData()
{
    QStringListModel *modelShortName;
    modelShortName = new QStringListModel(getFillShortName(), this);
    ui->CBShortName->setModel(modelShortName);

}
///------------------------------------------------
void ImportBookDialog::debug()
{
    //    ui->LEHtmlFilter->setText(htmlfilter);
    //    ui->CBShortName->acceptDrops()"lol");
    //    ui->SBCount->setValue(4);
    QString str = "/home/files/Documents/Bible/unrar/NT_Russian_Kassian/41_mark.htm";
    ui->LEFilePath->setText(str);

    ui->LETagChapter->setText("<h4>");
    //    ui->LETagChapter->setText("<A NAME");
    ui->LETagVerse->setText("<p>");
}

///------------------------------------------------
void ImportBookDialog::browse()
{
    QString beginpath = Config::configuration ()->AppDir ();
    QString t_text = ui->LEFilePath->text();
    if (!t_text.isEmpty())
        beginpath = getFolderFile(&t_text);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select book"),
                                                    beginpath,
                                                    tr("Html Files (*.html *.htm);;All Files (*)"));
    if (!fileName.isNull())
    {
        ui->LEFilePath->setText(fileName);
        ui->LAEncoding->setText(getEncodingFromFile(fileName));
        if (!ui->LETagVerse->text().isEmpty()
                && !ui->LETagChapter->text().isEmpty())
        estimate();
        //        qDebug() << getEncodingFromFile(fileName);
    }
}
///------------------------------------------------
void ImportBookDialog::accept()
{
    QString s = "";  //holds list of errors
    bool er = false;
    //    validProperties = false;
    if (ui->LEFilePath -> text().isEmpty())
    {
        s.append(tr("- Please enter a file.\n"));
        er = true;
    }
    if ( ui->CBShortName->currentText().isEmpty())
    {
        s.append(tr("- Please enter a correct short name.\n"));
        er = true;
    }
    if (ui->LEFullName -> text().isEmpty())
    {
        s.append(tr("- Please enter a full name.\n"));
        er = true;
    }
    if (ui->LETagChapter->text().isEmpty())
    {
        s.append(tr("- Please enter a tag chapter.\n"));
        er = true;
    }
    if (ui->SBCount->value() == 0)
    {
        s.append(tr("- Please enter a count chapters.\n"));
        er = true;
    }
    if (ui->LETagVerse->text().isEmpty())
    {
        s.append(tr("- Please enter a tag verse.\n"));
        er = true;
    }

    if (er)
    {
        QMessageBox::critical(this, tr("Import book error"), s);
    }
    else
    {
        saveData();
        QStringList replaceduplex = getReplaceList();;
        bookEncoding = ui->LAEncoding->text();
        importm->setHtmlFilter (ui->LEHtmlFilter->text ());
        importm->setTextReplace (replaceduplex);
        importm->importBook(projectfile,
                            bookPathFile,
                            bookFullName,
                            bookShortName,
                            bookCount,
                            bookTagChapter,
                            bookEncoding);
        importm->addContentToEndProjectFile(projectfile);

        deletePreviewFolder();
        emit SuccessfulImportBook();
        QWidget::hide();  //close dialog
    }
}
///------------------------------------------------
void ImportBookDialog::saveData()
{
    bookShortName = ui->CBShortName->currentText();
    bookFullName = ui->LEFullName->text();
    //    bookEncoding = ui->CBEncoding->currentText();
    bookEncoding = ui->LAEncoding->text();

    bookTagChapter = ui->LETagChapter->text();
    bookTagVerse = ui->LETagVerse->text();
    bookHtmlFilter = ui->LEHtmlFilter->text();
    bookPathFile = ui->LEFilePath->text();
    bookCount = ui->SBCount->value();
    projectfile = Config::configuration()->CurProject();
}
///------------------------------------------------
QStringList ImportBookDialog::getReplaceList()
{
    QStringList replaceduplex;
    if (!ui->LETextAdnvanceReplace->text().isEmpty())
    {
        QString str = ui->LETextAdnvanceReplace->text();

        /// replace spaces to text
        str = replaceSpaceInStrToText(str);

        /// add ";" to end if
        /// hahaha  how?  not work symbol ";"
        /// int(";") = 135406813
        if (str[str.length()-1] != QChar(135406813))
            str.append(";");

        int pos;
        QString cut;
        /// cut str to blocks
        while (str.indexOf(";") >=0)
        {
            pos = str.indexOf(";");
            cut = QString(str).remove(pos, str.length()-pos);
            str.remove(cut + ";");
            replaceduplex
                    << cut;
        }
    }
    else
    {
        replaceduplex
                << QString("%1:%2")
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1A->text()))
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1B->text()))
                << QString("%1:%2")
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1A_2->text()))
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1B_2->text()))
                << QString("%1:%2")
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1A_3->text()))
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1B_3->text()))
                << QString("%1:%2")
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1A_4->text()))
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1B_4->text()))
                << QString("%1:%2")
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1A_5->text()))
                   .arg(replaceSpaceInStrToText(ui->LETextReplace1B_5->text()));
    }
    return replaceduplex;
}
///------------------------------------------------
void ImportBookDialog::showPropertiesDialog()
{
    //    showProperties();
    emit ProjectPropsShow();
}
///------------------------------------------------
void ImportBookDialog::showPreview()
{
    //    prevbook->setEncoding(ui->CBEncoding->currentText());
    estimate();
    prevbook->setEncoding(ui->LAEncoding->text());
    prevbook->setData(ui->LEFilePath->text());
    createBookPreview();
    prevbook->createBookPreviewFunc();
    prevbook->show();
}
///------------------------------------------------
void ImportBookDialog::createBookPreview()
{
    saveData();
    importm->setPathOutput("_Preview_/_Preview_");
    importm->setEncodingForPreview(bookEncoding);
    importm->importBook(projectfile,
                        bookPathFile,
                        bookFullName,
                        bookShortName,
                        bookCount,
                        bookTagChapter,
                        bookEncoding);
}
///------------------------------------------------
void ImportBookDialog::estimate()
{
    QString TagOfFile = ui->LETagChapter->text();
    QString TextOfFile = getTextFromFile(ui->LEFilePath->text(),ui->LAEncoding->text());
    int countFiles = countTheNumberOfFiles(&TextOfFile, TagOfFile);
    ui->SBCount->setValue(countFiles);
}

void ImportBookDialog::deletePreviewFolder()
{
    QString path = Config::configuration()->PrjDir() + "_Preview_";
    QDir dir(path);
    dir.rmpath(path);

}
