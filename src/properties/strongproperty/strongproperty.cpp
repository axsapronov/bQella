#include "strongproperty.h"
#include "ui_strongproperty.h"
#include "config.h"
#include "pcommon.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

StrongProperties::StrongProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StrongProperties)
{
    ui->setupUi(this);
    connect(ui->BRemove, SIGNAL(clicked()), this, SLOT(RemoveStrong()));
    connect(ui->pBInfo, SIGNAL(clicked()), this, SLOT(setStrongInfo()));
}

StrongProperties::~StrongProperties()
{
    delete ui;
}

void StrongProperties::RemoveStrong()
{
    emit removeStrong();
    QWidget::hide();  //close dialog
}
void StrongProperties::setProperties(QString lText, QString numberstr)
{
    strongText = lText;
    int number = QString(numberstr).toInt();
    strongNumber = number;

    //    qDebug() << "int = " << number
    //             << " str = " << numberstr;
    ui->EStrongText -> setText(lText);
    ui->sBStrongNumber->setValue(number);
}
// how not work?
//void StrongProperties::reject()
//{
//    validProperties = false;
//    QWidget::hide();  //close dialog
//}

//void StrongProperties::accept()
//{
//    strongText = ui->EStrongText -> text();
//    strongLocation = ui->EStrongLocation -> text();
//    validProperties = true;
//    if ( ui->EStrongLocation -> text().isEmpty() )
//        emit removeStrong();
//    else
//        emit updateStrong(strongText, strongLocation);
//    QWidget::hide();  //close dialog
//}

void StrongProperties::on_buttonBox_accepted()
{

    bool er = false;
    int strongValueH = 8674;
    int strongValueG = 5624;
    int strongnumberactual = ui->sBStrongNumber->value();
    if (strongnumberactual > strongValueH
            && ui->rBHebrew->isChecked())
    {
        QMessageBox::critical(this, tr("There is no such numbers Strong"),
                              tr("There is no such numbers Strong:\n%1").arg(strongnumberactual));
        er = true;
    }

    if (strongnumberactual > strongValueG
            && ui->rBGreek->isChecked())
    {
        QMessageBox::critical(this, tr("There is no such numbers Strong"),
                              tr("There is no such numbers Strong:\n%1").arg(strongnumberactual));
        er = true;
    }

    if (!er)
    {
        strongText = ui->EStrongText -> text();
        strongLocation = QString::number(ui->sBStrongNumber->value());
        validProperties = true;
        if ( ui->sBStrongNumber->value() == 0 )
            emit removeStrong();
        else
            emit updateStrong(strongText, strongLocation);
        QWidget::hide();  //close dialog
    }

}

void StrongProperties::on_buttonBox_rejected()
{
    validProperties = false;
    QWidget::hide();  //close dialog
}

void StrongProperties::setStrongInfo()
{

    int strongValueH = 8674;
    int strongValueG = 5624;
    int strongnumberactual = ui->sBStrongNumber->value();
    if (strongnumberactual > strongValueH
            && ui->rBHebrew->isChecked())
    {
        QMessageBox::critical(this, tr("There is no such numbers hebrew Strong"),
                              tr("There is no such numbers Strong:\n%1").arg(strongnumberactual));
    }

    if (strongnumberactual > strongValueG
            && ui->rBGreek->isChecked())
    {
        QMessageBox::critical(this, tr("There is no such numbers greek Strong"),
                              tr("There is no such numbers Strong:\n%1").arg(strongnumberactual));
    }
    QString horg = "";
    if (ui->rBGreek->isChecked()) horg = "g";
    if (ui->rBHebrew->isChecked()) horg = "h";
    QString str = tr("Number ") +
            QString::number(strongnumberactual) + ":<br>" +
            getInfoFromStrongFile(horg, QString::number(strongnumberactual));
    ui->sStrongText->setText(str);
}
