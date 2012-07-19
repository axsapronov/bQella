#include "importbookdialog.h"
#include "ui_importbookdialog.h"
#include "pcommon.h"

#include "importdialog.h"
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

    connect(ui->pBBrowse, SIGNAL(clicked()), this, SLOT(browse()));

    importm = new Import();
    setData();
}

ImportBookDialog::~ImportBookDialog()
{
    delete ui;
}

void ImportBookDialog::setData()
{

    QStringListModel *modelShortName;
    modelShortName = new QStringListModel(getFillShortName(), this);
    ui->CBShortName->setModel(modelShortName);

    QStringListModel *modelEncoding;
    modelEncoding = new QStringListModel(getFillEncoding(), this);
    ui->CBEncoding->setModel(modelEncoding);

    QString htmlfilter = "<br> <pre> </pre>"
            " <span </span> <font </font> <sup> </sup> <sub> </sub> <center> </center> <strong> </strong>"
            " <em> </em> <table </table>"
            " <tr <tr> </tr> <td <td> </td> <th> <th </th> <hr <hr>";
    ui->LEHtmlFilter->setText(htmlfilter);
    ui->LETagChapter->setText("<h4>");
    ui->LETagVerse->setText("<p>");

    //test
//    ui->LEFullName->setText("test12");
//    ui->SBCount->setValue(3);
}

void ImportBookDialog::browse()
{
    QString beginpath = "/home/warmonger";
//    QString beginpath = "/home/files/Documents/Bible/unrar/NT_Greek_WH-E_UTF8";
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select book"),
                                                    beginpath,
                                                    tr("Html Files (*.html *.htm);;All Files (*)"));
    if (!fileName.isNull())
    {
        ui->LEFilePath->setText(fileName);
    }
}

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
    if (ui->LEHtmlFilter -> text().isEmpty())
    {
            s.append(tr("- Please enter a html filter.\n"));
            er = true;
    }
    if (ui->LETagChapter->text().isEmpty())
    {
            s.append(tr("- Please enter a tag chapter.\n"));
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
        bookShortName = ui->CBShortName->currentText();
        bookFullName = ui->LEFullName->text();
        bookEncoding = ui->CBEncoding->currentText();
        bookTagChapter = ui->LETagChapter->text();
        bookTagVerse = ui->LETagVerse->text();
        bookHtmlFilter = ui->LEHtmlFilter->text();
        bookPathFile = ui->LEFilePath->text();
        bookCount = ui->SBCount->value();

        QString projectfile = Config::configuration()->CurProject();
//        qDebug() << " project file = " << projectfile;
        importm->importBook(projectfile, bookPathFile, bookFullName, bookShortName, bookCount, bookTagChapter);
        importm->addContentToEndProjectFile(projectfile);

        emit SuccessfulImportBook();
        QWidget::hide();  //close dialog

    }
}

