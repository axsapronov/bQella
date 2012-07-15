/*!
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *    Author: Sapronov Alexander/WarmongeR
 *    E-Mail: sapronov.alexander92@.gmail.com
 *    Years: 2011, 2012
 *    WebSite: https://github.com/WarmongeR1/bQella
 */

#include "bookadddialog.h"
#include "ui_bookadddialog.h"
#include "pcommon.h"

#include <QTextCodec>
#include <QStringListModel>
#include <QDebug>

BookAddDialog::BookAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookAddDialog)
{
    bookFullName = "";
    bookShortName = "";
    bookChapterQty = 0;
    bookCheckAutoChapterCreate = false;

    QTextCodec *codec = QTextCodec::codecForName("UTF8");  // назначаем кодировку для всех надписей
    QTextCodec::setCodecForTr(codec); //устанавливаем кодек

    ui -> setupUi(this);

    QStringList items = getFillShortName();
    typeModel = new QStringListModel(items, this);
    ui -> comboBoxShortName -> setModel(typeModel);


    connect(ui -> buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(clickslot(QAbstractButton *)));
    connect(ui -> buttonBox, SIGNAL(accepted()), this, SIGNAL(signalbookaddChanged()));
}

BookAddDialog::~BookAddDialog()
{
    delete ui;
}


void BookAddDialog::reject()
{
    QWidget::hide();  //close dialog
}

void BookAddDialog::clickslot(QAbstractButton *AButton)
{
    if (ui -> buttonBox -> standardButton(AButton) == QDialogButtonBox::Cancel)
    {
        reject();
    }

    if (ui -> buttonBox -> standardButton(AButton) == QDialogButtonBox::Ok)
    {
        bookFullNameWithSpace = ui->lineEditFullName->text();
        QString str = ui->lineEditFullName->text();
//                str.replace(" ","_")
//                str.remove(str.length(),1);
//        qDebug() << " str = " << str << " strreplace " << str.replace(" ", "_") << " strremove = " << str.remove(str.length()-1,1);
        ui->lineEditFullName->setText(str);
        send();
        reject();
    }
    ui->lineEditFullName->setText("");
    ui->spinBoxChapterQty->setValue(0);
    ui->checkBoxAutoCreateChapter->setChecked(false);

    QStringList items = getFillShortName();
    typeModel = new QStringListModel(items, this);
    ui -> comboBoxShortName -> setModel(typeModel);
}


void BookAddDialog::send()
{

    bookChapterQty = ui -> spinBoxChapterQty -> value();
    bookFullName = QString(ui -> lineEditFullName -> text()).replace(" ", "_").remove(bookFullName.length(),1);
    bookShortName = ui -> comboBoxShortName -> currentText();
    bookCheckAutoChapterCreate = ui -> checkBoxAutoCreateChapter -> isChecked();

    if (ui->comboBoxShortName->findText(bookShortName) == -1)
    {
        ui->comboBoxShortName->addItem(bookShortName);
        ui->comboBoxShortName->setCurrentIndex(ui->comboBoxShortName->findText(bookShortName));
    }
}
