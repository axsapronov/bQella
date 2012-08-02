#include "strongproperty.h"
#include "ui_strongproperty.h"
#include "config.h"

#include <QFileDialog>
#include <QDebug>

StrongProperties::StrongProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StrongProperties)
{
    ui->setupUi(this);

    connect(ui->BRemove, SIGNAL(clicked()), this, SLOT(RemoveStrong()) );
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
    strongText = ui->EStrongText -> text();
    strongLocation = QString::number(ui->sBStrongNumber->value());

    qDebug() << "location = " << StrongLocation();
    validProperties = true;
    if ( ui->sBStrongNumber->value() == 0 )
        emit removeStrong();
    else
        emit updateStrong(strongText, strongLocation);
    QWidget::hide();  //close dialog

}

void StrongProperties::on_buttonBox_rejected()
{
    validProperties = false;
    QWidget::hide();  //close dialog
}
