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
#ifndef __PROJECTSOURCES_H__
#define __PROJECTSOURCES_H__

#include <QtSql>

#include "ui_projectsources.h"
//#include "srclistparser.h"

//===================== class ProjectSources =====================
class ProjectSources : public QDialog
{
     Q_OBJECT

public:
	ProjectSources(QWidget *parent = 0);
	QString getSignString() 		{ return signString; }
	//void setDefaultSignIndex(int n)	{ defaultSignIndex = n; }
	//int getDefaultSignIndex()		{ return defaultSignIndex; }
	
	// for SQLite database
	QSqlError initDb();
	QSqlError newDb();
	void closeDb();
	QString dbAddSource(QSqlQuery &q, const QString &title, const QString &authorId, const QString &title_short, const QString &title_original, const QString &title_original_short, const QString &comment);
	int dbAddLink(QSqlQuery &q, const QString &id_source, const QString &link, const QString &comment);
	void dbAddAuthor(QSqlQuery &q, const QString &name, const QString &name_short, const QString &comment);
	QStringList signList() { return SignList; }
	
signals:
	//void updateLinkList(int srcID);
	//void updateSigns(int defaultSign);
	void updateSigns();
     
public slots:
	void setSignString(int id);  //Set default sign; id = link id
	QString getSignString(int id);  //id = link id
	void loadSources();
	QSqlError newDb(QString dbFileName);
	
private slots:
	void saveSources();
	//void ApplyDbChanges();	// For manual DB commit changes. For debug
	void closeDialog();
	void srcRecordNew();
	void srcRecordDel();
	void lnkRecordNew(QString sourceID);
	void lnkRecordNew();
	void lnkRecordDel();
	void lnkShowAllRecords();
	void authorRecordNew();
	void authorRecordDel();
	void setSignString();
	void refreshLnkModel();

	//tab Format
	void testFormat();
	void setSignStringFromExample();
	void setSignFormat();
	void setExampleSign(int id);

private:
	void itemFromLinkList(int index);
	void setSignList();
	void refreshAuthorModel();
	void refreshSrcModel();	
	QString curAuthorId();
	QString curSrcId();
	QString curLnkId();
	//void saveTable(QSqlTableModel *model);	// For manual DB commit changes. For debug
	
	Ui::DialogSources ui;
	QString signString;
	QStringList SignList;
	//int defaultSignIndex;
	QSqlDatabase db;
	QSqlRelationalTableModel *srcModel;	 
	QSqlTableModel *authorModel, *lnkModel;

}; // class ProjectSources

#endif // __PROJECTSOURCES_H__
