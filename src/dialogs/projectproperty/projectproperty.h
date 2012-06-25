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
#ifndef __PROJECTPROPERTY_H__
#define __PROJECTPROPERTY_H__

#include "ui_projectproperty.h"

//===================== class ProjectProperties ===========================

typedef struct StructModuleProperties
{
    QString prjTitle;
    QString prjFN;
    QString prjStartPage;
    QString moduleBiblename;
    QString moduleCopyright;
    QString moduleBibleShortName;
    double moduleBVersion;
//    QString moduleType;

} ModuleProperties;


class ProjectProperties : public QDialog
{
     Q_OBJECT

public:
	ProjectProperties(QWidget *parent = 0);
	bool valid() { return validProperties; }

signals:
        void createProject(ModuleProperties propert);
        void updateProjectProperties(ModuleProperties propert);
	//void createDb(QString dbFileName);
     
public slots:
	void accept();
	void reject();
        void setProperties(bool newPrj, ModuleProperties pr);
//	QString Title()			{ return prjTitle; }	
	QString FileName()		{ return prjFN; }
//	QString StartPage()		{ return prjStartPage; } 
	
private:
	Ui::ProjectProperty ui; 
	
	bool modeNewProject; 	
        QString prjTitle, prjFN, prjStartPage, moduleBiblename, moduleCopyright, moduleBibleShortName;
//        QString moduleType;
        double moduleVersion;
	bool validProperties;
}; // class ProjectProperties

#endif // __PROJECTPROPERTY_H__
