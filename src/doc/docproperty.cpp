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
#include "docproperty.h"

//===================== class DocProperties =====================
DocProperties::DocProperties(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);
}

//-------------------------------------------------
void DocProperties::setTitle(QString title)
{
	ui.ETitle -> setText(title);
}

//-------------------------------------------------
void DocProperties::setFileName(QString fn)
{
	ui.LEFileName -> setText(fn);
}

//-------------------------------------------------
void DocProperties::reject()
{
	QWidget::hide();  //close dialog	
}

//-------------------------------------------------
void DocProperties::accept()
{
	emit updateTitle( ui.ETitle -> text() );	
	QWidget::hide();  //close dialog		
}
