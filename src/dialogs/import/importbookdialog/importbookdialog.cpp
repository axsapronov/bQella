#include "importbookdialog.h"
#include "ui_importbookdialog.h"
#include "pcommon.h"


#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>

ImportBookDialog::ImportBookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportBookDialog)
{
    ui->setupUi(this);

    connect(ui->pBBrowse, SIGNAL(clicked()), this, SLOT(browse()));

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
}

void ImportBookDialog::browse()
{
    QString beginpath = "/home/warmonger";
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

//        // добавить возможность изменить full name, short name

////            itemTitle = ui.ETitle -> text(); 	//!+! get title from document

//        itemFullName = ui.lineEditFullName->text();
//        itemShortName = ui.comboBoxShortName->currentText();
//        itemFile = urlifyFileName(ui.laPathToBook->text());
//        int count = ui.laCountOfChapter->text().toInt();

//        validProperties = true;
//        insertMode = false;
//        if (insertMode)
//        {
//            emit insertContentsItem(itemFullName, itemShortName, count, itemFile);

//        }else{
//            emit updateContentsItem(itemFullName, itemShortName, count, itemFile);
//        }
        QWidget::hide();  //close dialog

    }
}
