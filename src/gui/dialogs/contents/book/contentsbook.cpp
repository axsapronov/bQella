#include "contentsbook.h"
#include "ui_contentsbook.h"

#include "config.h"
#include "pcommon.h"
#include "filecommon.h"

#include <QFile>
#include <QMessageBox>

ContentsBook::ContentsBook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContentsBook)
{
    ui->setupUi(this);
    connect(ui->pBCreateContents, SIGNAL(clicked()), this, SLOT(createContents()));
}
//------------------------------------------
ContentsBook::~ContentsBook()
{
    delete ui;
}
//------------------------------------------
void ContentsBook::createContents()
{
}
//-------------------------------------------
void ContentsBook::createContents(QString filebook)
{
    if (QFile::exists(filebook))
    {
        if (checkFileContainsText(filebook, "Content = "))
        {
            replaceTextOfFile(filebook, "Content = no", "Content = yes");
        }
        else
        {
            if (checkFileContainsText(filebook, "</body>"))
            {
                replaceTextOfFile(filebook, "</body>", "Content = yes\n</body>");
            }
            else
            {
                addToEndFile(filebook, "\nContent = yes");
            }
        }
    }
}
//-------------------------------------------
void ContentsBook::setProperty(QString filebook, QString namebook,
                               QString chapternumber, QStringList list)
{
//    ui->sBookFileEdit->setText(filebook);
    ui->sBookNameEdit->setText(namebook);
    ui->sChaptersEdit->setText(chapternumber);

//    if (QFile::exists(filecontents))
//    {
//        ui->sFileContents->setText(filecontents);
//        ui->pBCreateContents->setVisible(false);
//    }
//    else
//    {
//        ui->pBCreateContents->setVisible(true);
//        ui->sFileContents->setText("It can create the content?");
//    }

    if (!editable)
    {
        //        QTreeWidgetItem *item;
        for (int i = 0; i < list.size(); i++)
        {
            //            ui->treeW->addTopLevelItem();
        }

        //        ui->treeW->setModel(model);
    }
}
//--------------------------------------------
void ContentsBook::accept()
{
    QWidget::hide();
}
//--------------------------------------------
void ContentsBook::reject()
{
    QWidget::hide();
}
//--------------------------------------------
void ContentsBook::deleteContent(QString fName)
{
    if (QFile::exists(fName))
    {
        int ret = QMessageBox::warning(this, GL_PROG_NAME, tr("Delete content?"),
                                       QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (ret == QMessageBox::Yes)
        {
            replaceTextOfFile(fName, "Content = yes", "Content = no");
        }
    }
}
//--------------------------------------------
void ContentsBook::setEdit(bool edit)
{
    editable = edit;
}
