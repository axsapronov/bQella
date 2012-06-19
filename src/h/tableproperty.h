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
#ifndef __TABLEPROPERTY_H__
#define __TABLEPROPERTY_H__

#include <QTextTable>

#include "ui_tableproperty.h"

//===================== class TableProperties ===========================

class TableProperties : public QDialog
{
     Q_OBJECT

public:
	TableProperties(QWidget *parent = 0);

signals:
	void createTable(int rows, int colums, QTextTableFormat tableFormat);
	void updateTable(int rows, int colums, QTextTableFormat tableFormat);
     
public slots:
	void accept();
	void reject();
	void setProperties(int rows, int colums, QTextTableFormat tableFormat, bool newTable);
	
private slots:
	void apply();
	void changeColorTable();
	void changeColorBorder();
	
private:
	void getTableFormat();
	
	Ui::TableProperty ui;
	bool modeNewTable;
	QTextTableFormat tFormat;
	QColor colorTable, colorBorder;
 
}; // class TableProperties

#endif // __TABLEPROPERTY_H__
