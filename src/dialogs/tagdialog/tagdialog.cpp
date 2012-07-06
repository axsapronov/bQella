#include "tagdialog.h"
#include "ui_tagdialog.h"

#include <QFileDialog>


#include "config.h"


TagDialog::TagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagDialog)
{
    ui->setupUi(this);
}
//---------------------------------------
TagDialog::~TagDialog()
{
    delete ui;
}
//---------------------------------------
void TagDialog::setProperties(QString lText)
{
    tag = lText;
    ui->lETag ->setText(lText);
}
//---------------------------------------
void TagDialog::reject()
{
    validProperties = false;
    QWidget::hide();  //close dialog
}
//---------------------------------------
void TagDialog::accept()
{
    tag = ui->lETag->text();
    validProperties = true;
    emit addTag(tag);
    QWidget::hide();  //close dialog
}
//---------------------------------------
