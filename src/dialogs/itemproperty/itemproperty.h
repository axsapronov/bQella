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
#ifndef __ITEMPROPERTY_H__
#define __ITEMPROPERTY_H__

#include "ui_itemproperty.h"

//==================== class ItemProperties ====================
class ItemProperties : public QDialog
{
     Q_OBJECT

public:
	ItemProperties(QWidget *parent = 0);
	bool valid() { return validProperties; }

signals:
	void insertContentsItem(QString title, QString fileName, QString iconFN);
	void updateContentsItem(QString title, QString fileName, QString iconFN);
     
public slots:
	void chooseIcon();
	void chooseFile();
	void accept();
	void reject();
	void setProperties(QString title, QString fileName, QString iconFN, bool mode);
	QString title()		{ return itemTitle; }	
	QString filename()	{ return itemFile; }
	QString iconFN()	{ return itemIconFN; }
	
private:
	Ui::ItemProperty ui;
	
	bool insertMode; 	
	QString itemTitle, itemFile, itemIconFN;
	bool validProperties;

}; // class ItemProperties

#endif // __ITEMPROPERTY_H__
