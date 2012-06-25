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
#ifndef __LINKPROPERTY_H__
#define __LINKPROPERTY_H__

#include "ui_linkproperty.h"

//===================== class LinkProperties =====================
class LinkProperties : public QDialog
{
     Q_OBJECT

public:
	LinkProperties(QWidget *parent = 0);
	bool valid() { return validProperties; }
	QString LinkText() { return linkText; }
	QString LinkLocation() { return linkLocation; }

signals:
	void removeLink();//(QString title, QString fileName, QString iconFN);
	void updateLink(QString lText, QString lLocation);
     
public slots:
	void chooseFile();
	void RemoveLink();
	void accept();
	void reject();
	void setProperties(QString lText, QString lLocation);
	
private:
	Ui::LinkProperty ui;
	
	QString linkText, linkLocation;
	bool validProperties;

}; // class LinkProperties

#endif // __LINKPROPERTY_H__
