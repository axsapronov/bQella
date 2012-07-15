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
        void insertContentsItem(QString ful, QString shotme, int count, QString path);
        void updateContentsItem(QString full, QString shoe, int count, QString path);
     
public slots:
	void accept();
        void reject();
        void setProperties(QString full, QString shortname, int counts, QString path);

        QString getFullName() { return itemFullName; }
        QString getShortName() { return itemShortName; }
        QString getFilename() { return itemFile; }
	
private:
	Ui::ItemProperty ui;
	
	bool insertMode; 	
        QString itemFullName, itemShortName, itemFile;

	bool validProperties;

}; // class ItemProperties

#endif // __ITEMPROPERTY_H__
