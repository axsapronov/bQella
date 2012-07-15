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

#include "itemproperty.h"
#include "config.h"
#include "pcommon.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStringListModel>

//====================== class ItemProperties ============================
ItemProperties::ItemProperties(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);
     
//     connect(ui.BIcon, SIGNAL(clicked()), this, SLOT(chooseIcon()) );
//     connect(ui.BOpenFileDialog, SIGNAL(clicked()), this, SLOT(chooseFile()) );
}

void ItemProperties::accept()
{
	QString s = "";  //holds list of errors
	bool er = false;
	validProperties = false;
        if (ui.lineEditFullName -> text().isEmpty())
        {
                s = tr("- Please enter a full name.\n");
                er = true;
        }
        if ( ui.comboBoxShortName->currentText().isEmpty())
        {
                s += tr("- Please enter a correct short name.\n");
                er = true;
        }

        if (er)
        {
                QMessageBox::critical(this, tr("Item property error"), s);
        }
        else
        {

            // добавить возможность изменить full name, short name
//                createEmptyHtml(ui.EFile -> text(), ui.ETitle -> text());
//                QFile file(ui.EFile -> text());
//                if (file.exists()){	//file does exist we can proceed with setting item properties
//                        itemTitle= ui.ETitle -> text(); 	//!+! get title from document
//                        itemFile = urlifyFileName(ui.EFile -> text());
//                        validProperties = true;
//                        if (insertMode){
//                                emit insertContentsItem(itemTitle, itemFile);
//                        }else{
//                                emit updateContentsItem(itemTitle, itemFile);
//                        }
                        QWidget::hide();  //close dialog
//                }
        }
}

void ItemProperties::reject()
{
	validProperties = false;
	QWidget::hide();  //close dialog
}


void ItemProperties::setProperties(QString full, QString shortname, int counts, QString path)
{
    itemFullName = full;
    itemShortName = shortname;
    itemFile = path;

    QStringList items = getFillShortName();
    QStringListModel *typeModel = new QStringListModel(items, this);
    ui.comboBoxShortName -> setModel(typeModel);

    if (ui.comboBoxShortName->findText(shortname) == -1)
        ui.comboBoxShortName->addItem(shortname);
    ui.comboBoxShortName->setCurrentIndex(ui.comboBoxShortName->findText(shortname));

    ui.laCountOfChapter->setText(QString::number(counts));
    ui.laPathToBook->setText(path);
    ui.lineEditFullName->setText(full);
}
