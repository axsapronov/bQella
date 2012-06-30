#include "frdialog.h"
#include "ui_frdialog.h"

FRDialog::FRDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FRDialog)
{
    ui->setupUi(this);
}

FRDialog::~FRDialog()
{
    delete ui;
}
