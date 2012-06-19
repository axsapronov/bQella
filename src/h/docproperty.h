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
#ifndef __DOCPROPERTY_H__
#define __DOCPROPERTY_H__

#include "ui_docproperty.h"

//===================== class DocProperties =====================
class DocProperties : public QDialog
{
     Q_OBJECT

public:
	DocProperties(QWidget *parent = 0);
	void setTitle(QString title);
	void setFileName(QString fn);
	
signals:
	void updateTitle(QString title);
     
public slots:
	void accept();
	void reject();	
	
private:
	Ui::DocProperty ui;
	
}; // class LinkProperties

#endif // __DOCPROPERTY_H__
