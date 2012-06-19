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

#include "autoitems.h"
#include "config.h"
#include "pcommon.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

// !+! move this enum to config.h since the same is in HelpDialog
enum { LinkRole   = Qt::UserRole + 1000, 	// link (file name) to source
	   IconFNRole = Qt::UserRole + 1001 };	// icon file name

//====================== class AutoItems ============================
AutoItems::AutoItems(QWidget *parent)
     : QDialog(parent)
{	 
     strAlphabet = tr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Upper case letters of the national alphabet (number of letters does not matter)");
     ui.setupUi(this);
     
     connect(ui.BTest, SIGNAL(clicked()), this, SLOT(CreateItems()) );
     connect(ui.BApply, SIGNAL(clicked()), this, SLOT(Apply()) );
     connect(ui.BCancel, SIGNAL(clicked()), this, SLOT(closeDialog()) );
     connect(ui.comboBoxCounterType, SIGNAL(currentIndexChanged(int)), this, SLOT(applyCounter(int)) );
     connect(ui.LECounterStart, SIGNAL(textChanged(QString)), this, SLOT(changeStartToolTip(QString)) );
     connect(ui.LECounterEnd, SIGNAL(textChanged(QString)), this, SLOT(changeEndToolTip(QString)) );
     connect(ui.CBLowerCase, SIGNAL(stateChanged(int)), this, SLOT(changeCase(int)) );
     connect(ui.CBDateFormat, SIGNAL(stateChanged(int)), this, SLOT(changeFormat(int)) );
}

//-------------------------------------------------
void AutoItems::closeDialog()
{
	ui.TWExample->clear();
	ui.BApply->setEnabled(false);
	QWidget::hide();  //close dialog	
}

//-------------------------------------------------
void AutoItems::Apply()
{
	QTreeWidgetItem *item;
	int i;
	int n = ui.TWExample->topLevelItemCount();

	for (i=0; i < n; i++){
		item = 	ui.TWExample->topLevelItem(i)->clone();
		qDebug() <<  "clone item title = " << item->text(0) << ", link = " << item->data(0, LinkRole);
		if (ui.CBCreateFiles->isChecked() )
			createFile(item);
		emit cloneItem(item, ui.CBAsSubItems->isChecked() );
	}
	emit cloneDone();
	ui.TWExample->clear();	
	ui.BApply->setEnabled(false);	
	QWidget::hide();  //close dialog	
}

//-------------------------------------------------
void AutoItems::createFile(QTreeWidgetItem *i) // create empty file for items' link 
{	
	QString fn = i->data(0, LinkRole).toString();
	QString str_status = "Creating file '" +fn+ "' for autoitem: ";
	QFile file(fn);
	
	if (file.exists()){	
		str_status = str_status + "already exists";
	}else{	//create file if it does not exist
		if (file.open(QIODevice::ReadWrite)){	//try to create file
			QFileInfo fi(fn);
			QString title = fi.baseName();
			QTextStream ts(&file);
			ts << "<html>" << endl;
			ts << "   <head>" << endl;
			ts << "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
			ts << "      <style type=\"text/css\">" << endl;
			ts << "      <title>" << title << "</title>" << endl;			
			ts << "   </head>" << endl;
			ts << "   <body></body>" << endl;
			ts << "</html>" << endl;
			file.close(); 
			str_status = str_status + "ok";
		}else{	
			str_status = str_status + "failed";				
		}
	} 
	qDebug() << str_status; //?+? add this info to log file
	//process subitems
	for (int index = 0; index < i->childCount(); ++index){		
        createFile(i->child(index));
	}	
}

//-------------------------------------------------
void AutoItems::CreateItems()
{
	ui.TWExample->clear();
	/*?+? may be make a root item in Sample View to indicate that this list is a subitems list
	if (ui.CBAsSubItems->isChecked()){
		QTreeWidgetItem *root = QTreeWidgetItem(ui.TWExample);
    	root->setText(0, tr("Root Item", "Example TreeWidget in AutoItems dialog"));
    }*/
	if (ui.tabWidget->currentWidget()->objectName() == QLatin1String("tabDate"))
		CreateItemsDate();
	else if (ui.tabWidget->currentWidget()->objectName() == QLatin1String("tabCounter"))
		CreateItemsCounter();
		
	if (ui.TWExample->topLevelItemCount()>0)
		ui.BApply->setEnabled(true);
	else
		ui.BApply->setEnabled(false);
}

//-------------------------------------------------
void AutoItems::putItem(QTreeWidgetItem *i, QString str, QString link) 
{
	QTreeWidgetItem *item = new QTreeWidgetItem(i);
    item->setText(0, str);	
    QString lnk = link;
    if(lnk.isEmpty()) lnk = CreateFileName(str);
    item->setData(0, LinkRole, lnk);
}

//-------------------------------------------------
void AutoItems::putItem(QTreeWidget *tw, QString str, QString link) 
{
	QTreeWidgetItem *item = new QTreeWidgetItem(tw);
    item->setText(0, str);	
    QString lnk = link;
    if(lnk.isEmpty()) lnk = CreateFileName(str);
    item->setData(0, LinkRole, lnk);
}

//-------------------------------------------------
QString AutoItems::CreateFileName(QString str)
{
	int counter=1;
	QString fn = CreateValidWebFileName(str);
	QString resFN = absolutifyFileName(fn + ".html", Config::configuration()->CurPrjDir());
	while (QFile::exists(resFN)){ // add counter if file with such a name exists
		resFN = absolutifyFileName(fn +"("+ QString::number(counter) +").html", Config::configuration()->CurPrjDir());
		++counter;	
	}
	
	return resFN;
}

//-------------------------------------------------
void AutoItems::activateTab(int n)
{
	ui.tabWidget->setCurrentIndex(n);
}

//----------------------- procedures for tab "Date" -----------------------
void AutoItems::CreateItemsDate()
{
	int y,m,m1,m2,d,d1,d2;
	QString str, link;
	QDate D, DFrom, DTo;
	
	if (ui.RBYears->isChecked()){
		DFrom.setDate(ui.SBYearFrom->value(),1,1); 
		DTo.setDate(ui.SBYearTo->value(),12,31); 
	}else if(ui.RBPeriod->isChecked()){
		DFrom = ui.DEPeriodFrom->date();
		DTo   = ui.DEPeriodTo->date();  		
	}
	if (ui.CBSplitPeriod->isChecked()){
		for (y = DFrom.year(); y <= DTo.year(); ++y){
			QTreeWidgetItem *iYear = new QTreeWidgetItem(ui.TWExample);
    		iYear->setText(0, QString::number(y));
    		link = CreateFileName(QString::number(y));
    		iYear->setData(0, LinkRole, link);
    		//set months
    		if (y == DFrom.year()) m1 = DFrom.month();
    			else m1 = 1;
    		if (y == DTo.year()) m2 = DTo.month();
    			else m2 = 12;
    		for (m=m1; m<=m2; m++){
    			QTreeWidgetItem *iMonth = new QTreeWidgetItem(iYear);
    			if (ui.CBShowMonthName->isChecked())
    				iMonth->setText(0, pMonthName(m));
    			else
    				iMonth->setText(0, QString::number(m));
    			link = CreateFileName(QString::number(y) +"."+ QString::number(m));
    			iMonth->setData(0, LinkRole, link);
    			D.setDate(y, m, 1);
    			//set days 
    			if ((y == DFrom.year()) && (m == DFrom.month())) d1 = DFrom.day();
    				else d1 = 1;
    			if ((y == DTo.year()) && (m == DTo.month())) d2 = DTo.day();
    				else d2 = D.daysInMonth(); 			
    			for (d=d1; d<=d2; d++){
    				QTreeWidgetItem *iDay = new QTreeWidgetItem(iMonth);
    				if ( ui.CBDateFormat->isChecked() ){
    					D.setDate(D.year(), D.month(), d);
    					str = D.toString(ui.LEDateFormat->text()); 
    				}else{ 
    					str = QString::number(d);
   					}
    				iDay->setText(0, str);
    				link = CreateFileName(QString::number(y) +"."+ QString::number(m) +"."+ QString::number(d));
    				iDay->setData(0, LinkRole, link);
    				qDebug() << "str = " << str << " link = " << link;
   				}
  			}
   		}
  	}else{
  		D = DFrom;
  		qDebug() << "DFrom = " << DFrom.toString("yyyy.MM.dd") << ", DTo = " << DTo.toString("yyyy.MM.dd");
  		while (D <= DTo){
  			if (ui.CBDateFormat->isChecked()) 
  				str = D.toString(ui.LEDateFormat->text());
  			else{ 
  				if (ui.CBShowMonthName->isChecked())
  					str = D.toString("yyyy MMMM d"); 	
  				else
  					str = D.toString("yyyy.MM.dd");
 			}
 			link = CreateFileName( D.toString("yyyy.MM.dd") );
  			putItem(ui.TWExample, str, link);
  			D = D.addDays(1);
 		}
 	}
}

//-------------------------------------------------
void AutoItems::setYears(int yFrom, int yTo)
{
	ui.SBYearFrom->setValue(yFrom);
	ui.SBYearTo->setValue(yTo);
}

//-------------------------------------------------
void AutoItems::setPeriods(QDate dFrom, QDate dTo)
{
	ui.DEPeriodFrom->setDate(dFrom);
	ui.DEPeriodTo->setDate(dTo);	
}

//----------------------- procedures for tab "Counter" -----------------------
void AutoItems::CreateItemsCounter()
{		
	QString str;
	int i; 
	int l = ui.spinBoxCounterLength->value();
	int counter = ui.LECounterStart->toolTip().toInt();
	int n = ui.LECounterEnd->toolTip().toInt() - ui.LECounterStart->toolTip().toInt(); 
	int m = 1; //for counter
	if (n <0){ m=-1; n=-n; }

   	for (i=0; i<=n; ++i){
   		str = IntToCounter(counter);
   		while (str.length() < l){
   			str = ui.LECounterFillChar->text() + str;
  		}
   		str = ui.LECounterPrefix->text() + str + ui.LECounterSuffix->text();
   		//if (ui.CBAsSubItems->isChecked()){ //?+? add as subitems to Example View
   			//putItem(root, str);
   		//}else
   			putItem(ui.TWExample, str, "");
   		counter += m;   		
  	}
}

//-------------------------------------------------
void AutoItems::applyCounter(int n)
{
	QString strFrom = "1";
	QString strTo  = "10";
	int intFrom = ui.LECounterStart->toolTip().toInt();
	int intTo   = ui.LECounterEnd->toolTip().toInt();

	switch(n) { 
		case 0: // Decimal numbers
			strFrom = QString::number(intFrom);
			strTo   = QString::number(intTo);
			break;
		case 1: // Roman numbers
			strFrom = IntToRoman(intFrom);
			strTo   = IntToRoman(intTo);
			break;
		case 2: // Latin letters
			strFrom = IntToLetter(intFrom);
			strTo   = IntToLetter(intTo);
			break;
		case 3: // Letters from national alphabet (f.e. Russian)
			strFrom = IntToLetter(intFrom, strAlphabet);
			strTo   = IntToLetter(intTo, strAlphabet);
			break;
    	default:
    	    break;
    }
    ui.LECounterStart->setText(strFrom);
	ui.LECounterEnd->setText(strTo);
	changeCase( ui.CBLowerCase->checkState() );		
}

//-------------------------------------------------
QString AutoItems::getToolTip(QString str)
{
	QString strTip;
	int n = 0;
	
	switch(ui.comboBoxCounterType->currentIndex()) {
		case 0:	n = str.toInt();					break;
		case 1:	n = RomanToInt(str);				break;
		case 2:	n = LetterToInt(str);				break;
		case 3:	n = LetterToInt(str, strAlphabet);	break;
    	default:	
    		break;		
	}
	strTip = QString::number(n);
	if(ui.CBLowerCase->isChecked())
		strTip = strTip.toLower();
	return strTip;
}

//-------------------------------------------------
QString AutoItems::IntToCounter(int n)
{
	QString strCounter;
	
	switch(ui.comboBoxCounterType->currentIndex()) {
		case 0:	strCounter = QString::number(n);			break;
		case 1:	strCounter = IntToRoman(n);					break;
		case 2:	strCounter = IntToLetter(n);				break;
		case 3:	strCounter = IntToLetter(n, strAlphabet);	break;
    	default:	
    		break;		
	}
	if(ui.CBLowerCase->isChecked())
		strCounter = strCounter.toLower();
	
	return strCounter;
}

//-------------------------------------------------
void AutoItems::changeStartToolTip(QString str)
{
	ui.LECounterStart->setToolTip( getToolTip(str) );
}

//-------------------------------------------------
void AutoItems::changeEndToolTip(QString str)
{
	ui.LECounterEnd->setToolTip( getToolTip(str) );
}

//-------------------------------------------------
void AutoItems::changeCase(int state)
{
	switch(state){
		case Qt::Checked:
			ui.LECounterStart->setText( ui.LECounterStart->text().toLower() );
			ui.LECounterEnd->setText( ui.LECounterEnd->text().toLower() );
			break;
		case Qt::Unchecked:	
			ui.LECounterStart->setText( ui.LECounterStart->text().toUpper() );
			ui.LECounterEnd->setText( ui.LECounterEnd->text().toUpper() );
			break;
    	default:	
    		break;		
	}		
}

//-------------------------------------------------
void AutoItems::changeFormat(int n)
{
    n = n;
	ui.LEDateFormat->setEnabled( ui.CBDateFormat->isChecked() );		
}

//-------------------------------------------------
void AutoItems::setMaxCounter()
{
	int max=0;
	
	switch(ui.comboBoxCounterType->currentIndex()) {
		case 0:	max = 10; 			break;
		case 1:	max = 3999;			break;
		case 2:	max = 26;			break;
		case 3:	max = strAlphabet.size();	break;
    	default:	
    		break;		
	}
	setMaxCounter(max);
}

//-------------------------------------------------
void AutoItems::setMaxCounter(int n)
{
	ui.LECounterEnd->setToolTip(QString::number(n));	
	applyCounter(ui.comboBoxCounterType->currentIndex());	
}

//-------------------------------------------------
void AutoItems::setCounterType(int n)
{
	ui.comboBoxCounterType->setCurrentIndex(n);		
}
