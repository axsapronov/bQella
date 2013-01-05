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


#include <QtGui>

#include "tableproperty.h"

//===================== class TableProperties ===========================

TableProperties::TableProperties(QWidget *parent)
     : QDialog(parent)
{	
	ui.setupUi(this);
	colorTable = QColor(Qt::transparent);
	colorBorder = QColor(Qt::black);

	connect(ui.PBApply, SIGNAL(clicked()), this, SLOT(apply()));	
	connect(ui.PBTableColor, SIGNAL(clicked()), this, SLOT(changeColorTable())); 
	connect(ui.PBBorderColor, SIGNAL(clicked()), this, SLOT(changeColorBorder()));	
	ui.PBTableColor -> setStyleSheet("background-color: " + colorTable.name() +";");
	ui.PBBorderColor -> setStyleSheet("background-color: " + colorBorder.name() +";");
	
	// !+! changes in height are weird. Uncomment when fixed
	ui.LHeight -> setVisible(false);
	ui.SBHeight -> setVisible(false);
	ui.CBHeight -> setVisible(false);
}

//------------------------------------------------------------------------------
void TableProperties::setProperties(int rows, int colums, QTextTableFormat tableFormat, bool newTable)
{
	ui.SBRows -> setValue(rows);
	ui.SBColumns -> setValue(colums);
	ui.PBApply -> setVisible(!newTable);
	modeNewTable = newTable;
	if (!newTable){
		tFormat = tableFormat;
	
		// global table settings
		colorTable = tFormat.background().color();
		ui.PBTableColor -> setStyleSheet("background-color: " + colorTable.name() +";");
		
		if (tFormat.alignment() == Qt::AlignLeft)		ui.CBAllign -> setCurrentIndex(0);
		if (tFormat.alignment() == Qt::AlignHCenter)	ui.CBAllign -> setCurrentIndex(1);
		if (tFormat.alignment() == Qt::AlignRight)		ui.CBAllign -> setCurrentIndex(2);
		
		QTextLength tl = tFormat.width();
		if (tl.type() == QTextLength::FixedLength) 		ui.CBWidth -> setCurrentIndex(0);
		if (tl.type() == QTextLength::PercentageLength)	ui.CBWidth -> setCurrentIndex(1);
		ui.SBWidth -> setValue(tl.rawValue());
		/*QTextLength tl = tFormat.height();
		if (tl.type() == QTextLength::FixedLength) 		ui.CBHeight -> setCurrentIndex(0);
		if (tl.type() == QTextLength::PercentageLength)	ui.CBHeight -> setCurrentIndex(1);
		ui.SBHeight -> setValue(tl.rawValue());*/
	
		// border and margins settings
		ui.SBBorder -> setValue(tFormat.border());
		colorBorder = tFormat.borderBrush().color();
		ui.PBBorderColor -> setStyleSheet("background-color: " + colorBorder.name() +";");
		ui.CBBorderStyle -> setCurrentIndex(tFormat.borderStyle());
		ui.SBTableMargins -> setValue(tFormat.cellSpacing());
		ui.SBCellMargins -> setValue(tFormat.cellPadding());
	}
}

//------------------------------------------------------------------------------
void TableProperties::getTableFormat()
{
	// global table settings
	if (ui.CBAllign -> currentIndex() == 0) tFormat.setAlignment(Qt::AlignLeft);
	if (ui.CBAllign -> currentIndex() == 1) tFormat.setAlignment(Qt::AlignHCenter);
	if (ui.CBAllign -> currentIndex() == 2) tFormat.setAlignment(Qt::AlignRight);
	
	if (ui.CBWidth -> currentIndex() == 0) tFormat.setWidth( QTextLength(QTextLength::FixedLength,	 ui.SBWidth -> value()) );
	if (ui.CBWidth -> currentIndex() == 1) tFormat.setWidth( QTextLength(QTextLength::PercentageLength,ui.SBWidth -> value()) );
	/* 
	size = ui.SBHeight -> value();
	if (ui.CBHeight -> currentIndex() == 0)	tFormat.setHeight( QTextLength(QTextLength::FixedLength, size) );
	if (ui.CBHeight -> currentIndex() == 1)	tFormat.setHeight( QTextLength(QTextLength::PercentageLength, size) );
	*/	
	tFormat.setBackground(colorTable);

	// border and margins settings
	tFormat.setBorder(ui.SBBorder -> value());
	tFormat.setBorderBrush(QBrush(colorBorder));
	tFormat.setBorderStyle( QTextFrameFormat::BorderStyle(ui.CBBorderStyle -> currentIndex()) );
	tFormat.setCellSpacing(ui.SBTableMargins -> value());
	tFormat.setCellPadding(ui.SBCellMargins -> value());
}

//------------------------------------------------------------------------------
void TableProperties::reject()
{
	QWidget::hide();  //close dialog
}

//------------------------------------------------------------------------------
void TableProperties::apply()
{
	getTableFormat();
	emit updateTable(ui.SBRows -> value(), ui.SBColumns -> value(), tFormat);
}

//------------------------------------------------------------------------------
void TableProperties::accept()
{	
	getTableFormat();
	if (modeNewTable){ 
		emit createTable(ui.SBRows -> value(), ui.SBColumns -> value(), tFormat);			
	}else{
		emit updateTable(ui.SBRows -> value(), ui.SBColumns -> value(), tFormat);
	}		
	QWidget::hide();  //close dialog
}

//------------------------------------------------------------------------------
void TableProperties::changeColorTable()
{
    QColor col = QColorDialog::getColor(colorTable, this);
    if (!col.isValid())
        return;   
	colorTable = col;
	ui.PBTableColor -> setStyleSheet("background-color: " + colorTable.name() +";");
}

//------------------------------------------------------------------------------
void TableProperties::changeColorBorder()
{
    QColor col = QColorDialog::getColor(colorBorder, this);
    if (!col.isValid())
        return;   
	colorBorder = col;
	ui.PBBorderColor -> setStyleSheet("background-color: " + colorBorder.name() +";");
}

