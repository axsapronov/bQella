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
 *    Years: 2012
 *    WebSite: https://github.com/WarmongeR1/bQella
 */

#include "tagdialog.h"
#include "ui_tagdialog.h"
#include "config.h"

#include <QFileDialog>

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
