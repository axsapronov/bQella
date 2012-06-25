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
 *    Author: Alexander Sapronov/WarmongeR
 *    E-Mail: warmonger72@gmail.com
 *    Years: 2011
 */
#include <QFileDialog>
#include "linkproperty.h"
#include "config.h"

//====================== class ItemProperties ============================
LinkProperties::LinkProperties(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);
     
     connect(ui.BRemove, SIGNAL(clicked()), this, SLOT(RemoveLink()) );
     connect(ui.BChooseFile, SIGNAL(clicked()), this, SLOT(chooseFile()) );  
}

void LinkProperties::RemoveLink()
{
	emit removeLink();
	QWidget::hide();  //close dialog	
}

void LinkProperties::chooseFile()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Choose file to link to"),
                               Config::configuration() -> CurPrjDir(), tr("HTML (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty()){
    	ui.ELinkLocation -> setText(fn);
   	}
}

void LinkProperties::setProperties(QString lText, QString lLocation)
{
	linkText = lText;
	linkLocation = lLocation;
	
	ui.ELinkText -> setText(lText);
	ui.ELinkLocation -> setText(lLocation);
}

void LinkProperties::reject()
{
	validProperties = false;
	QWidget::hide();  //close dialog	
}

void LinkProperties::accept()
{
	linkText = ui.ELinkText -> text();
	linkLocation = ui.ELinkLocation -> text();
	validProperties = true;
	if ( ui.ELinkLocation -> text().isEmpty() )
		emit removeLink();
	else
		emit updateLink(linkText, linkLocation);	
	QWidget::hide();  //close dialog		
}
