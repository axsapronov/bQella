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
#ifndef __AUTOITEMS_H__
#define __AUTOITEMS_H__

#include "ui_autoitems.h"

//==================== class AutoItems ====================
class AutoItems : public QDialog
{
     Q_OBJECT

public:
	AutoItems(QWidget *parent = 0);
	
	void activateTab(int n);
	void setMaxCounter();
	void setMaxCounter(int n);
	void setCounterType(int n); //set which counter to use: numbers, roman numbers or letters
	void setYears(int yFrom, int yTo);
	void setPeriods(QDate dFrom, QDate dTo);

signals:
	void cloneItem(QTreeWidgetItem *item, bool subItem);
	void cloneDone();
     
public slots:
	void applyCounter(int n);
	void CreateItems();
	
private slots:
	void Apply(); 		//apply changes and close dialog
	void closeDialog(); //close dialog without changes
	//procedures for tab "Counter"
	void changeStartToolTip(QString str);
	void changeEndToolTip(QString str);
	void changeCase(int state);
	void changeFormat(int n); //apply special date format

private:
	Ui::AutoItems ui;
	QString strAlphabet;	// Alphabet of the language user use
	
	void putItem(QTreeWidgetItem *i, QString str, QString link);
	void putItem(QTreeWidget *tw, QString str, QString link); 
	void createFile(QTreeWidgetItem *i);
	void CreateItemsDate();
	void CreateItemsCounter();	
	QString CreateFileName(QString str);
	QString getToolTip(QString str);
	QString IntToCounter(int n);

}; // class AutoItems

#endif // __AUTOITEMS_H__
