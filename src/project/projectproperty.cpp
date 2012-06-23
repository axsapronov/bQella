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
#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include "projectproperty.h"
#include "config.h"
#include "mainwindow.h"
#include "pcommon.h"

//===================== class ProjectProperties ===========================

ProjectProperties::ProjectProperties(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);
     
     connect(ui.BprjFileName, SIGNAL(clicked()), this, SLOT(chooseProjectFineName()));
     //connect(ui.BStartPage, SIGNAL(clicked()), this, SLOT(chooseStartPage()));
}

void ProjectProperties::chooseProjectFineName()
{//warmonger
	QString fn = QFileDialog::getSaveFileName(this, tr("Choose project name"),
                               Config::configuration()->PrjDir(), tr("Project editor module (*.pem);;All Files (*)"));
    if (!fn.isEmpty()){
    	QFileInfo fi(fn);
    	if (fi.suffix().isEmpty()) 
                fn += GL_Project_File;
		ui.EprjFileName->setText(fn);
	}
}


void ProjectProperties::chooseStartPage()
{
/*	QString *s= new QString();
	QString path = ui.EprjFileName->text();
	if (path.isEmpty()) path = Config::configuration()->CurPrjDir();
    QString fn = QFileDialog::getSaveFileName(this, tr("Choose file for start page"),
                               path, tr("HTML (*.htm *.html);;All Files (*)"), s, QFileDialog::DontConfirmOverwrite );
    if (!fn.isEmpty()){
    	QFileInfo fi(fn);
    	if (fi.suffix().isEmpty()) 
    		fn += ".html";
    	ui.EStartPage->setText(fn);
   	}
    */

//    QString *s= new QString();
        QString path = ui.EprjFileName->text();
        if (path.isEmpty()) path = Config::configuration()->CurPrjDir();


    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;

    QString fn  = QFileDialog::getExistingDirectory(this,
                                tr("Choose path for project"),
                                ui.EprjFileName->text(),
                                options);


    if (!fn.isEmpty()){
        /*
        QFileInfo fi(fn);

        if (fi.suffix().isEmpty())
                fn += ".html";*/
        //ui.EStartPage->setText(fn);
        }
}

void ProjectProperties::setProperties(QString title, QString fileName, QString startPage, bool newPrj, QString moduleBiblename, QString moduleBibleShortName, QString moduleCopyright)
{
	prjTitle= title;
	prjFN = fileName;
	prjStartPage = startPage; 
	modeNewProject = newPrj;

        //qDebug() << "/n" << fileName << " " << startPage << " " << title << "/n";
	ui.ETitle->setText(title);

        ui.lineEditBibleName->setText(moduleBiblename); // добавил
        ui.lineEditBibleShortName->setText(moduleBibleShortName);
        ui.lineEditCopyright->setText(moduleCopyright);
//        ui.doubleSpinBoxVersion->setValue()

        //ui.EprjFileName->setText(fileName);
        //ui.EStartPage->setText(startPage);
}

void ProjectProperties::reject()
{
	validProperties = false;
	QWidget::hide();  //close dialog
}

void ProjectProperties::accept()
{
	QString s = "";  //holds list of errors
	bool er = false;
	validProperties = false;
	if (ui.ETitle->text().isEmpty()){
		s = tr("- Please enter a title.\n");
		er = true;
	}
	if (ui.EprjFileName->text().isEmpty()) {
		s += tr("- Please enter a project file name.\n");
		er = true;
	}
	//check for correct start page
        /*if  (ui.EStartPage->text().isEmpty()){ //|| (!itemprop->valid()) )
		s += tr("- Please choose a valid start page.");
		er = true;
	}
        */
	if (er){
		QMessageBox::critical(this, tr("Project property error"), s);	
	}else{

            //check for valid project file name
            QFile filePrj(prjFN+ui.EprjFileName->text()+GL_Project_File);
            if (!filePrj.exists()){		//create file if it's not exist
                    if (filePrj.open(QIODevice::ReadWrite)){	//try to create file
                            QTextStream ts(&filePrj);
                            ts << "<pemproject>\n</pemproject>";
                            filePrj.close();
                    }else{
                            QMessageBox::critical(this, tr("Project property error"), tr("Can not create file:\n%1").arg(ui.EprjFileName->text()));
                            er = true;
                    }
            }
            //check for valid start page file name


            //QFile fileSP(prjFN+ui.EStartPage->text()+GL_Project_Conf_File);
            QFile fileSP(prjFN+"Bibleqt"+GL_Project_Conf_File);
            if (!fileSP.exists()){		//create file if it does not exist
                    if (fileSP.open(QIODevice::ReadWrite)){		//try to create file
                            QTextStream ts(&fileSP);
                            ts.setCodec("UTF-8");

                            QString str_header = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" +
                                                                    tr("Bibleqt.ini") + "</title></head>\n";
                            QString str_body = "<body>"+tr("\nBibleName = %1"
                                                           "\nBibleShortName = %2"
                                                           "\nCopyright = %3"
                                                           "\nDefaultEncoding = utf-8"
                                                           "\nChapterSign = <h4>"
                                                           "\nVerseSign = <p>")
                                    .arg(ui.lineEditBibleName->text())
                                    .arg(ui.lineEditBibleShortName->text())
                                    .arg(ui.lineEditCopyright->text());



                            Config::configuration()->setModuleBiblename(ui.lineEditBibleName->text());
                            Config::configuration()->setModuleBibleShortName(ui.lineEditBibleShortName->text());
                            Config::configuration()->setModuleCopyright(ui.lineEditCopyright->text());
                            QString str_ender = "\n</body>\n</html>\n";


                            ts << str_header << str_body << str_ender;
                            //ts << str_conf;
                            fileSP.close();


			}else{
				QMessageBox::critical(this, tr("Project property error"), tr("Can not create file:\n%1").arg(ui.EprjFileName->text()));
				er = true;
			}
		}  


		if (!er){	//project, start page and sources files do exist we can proceed with setting project properties
                        ModuleProperties prop;
                        prop.prjTitle= ui.ETitle->text();
                        prop.prjStartPage = urlifyFileName(prjFN+"Bibleqt"+GL_Project_Conf_File);
                        prop.prjFN = urlifyFileName(prjFN+ui.EprjFileName->text()+GL_Project_File);
                        prop.moduleBiblename = ui.lineEditBibleName->text();
                        prop.moduleBibleShortName = ui.lineEditBibleShortName->text();
                        prop.moduleCopyright = ui.lineEditCopyright->text();

                        validProperties = true;
			if (modeNewProject){ 
                                emit createProject(prop);
			}else{
                                emit updateProjectProperties(prop);
			}
			QWidget::hide();  //close dialog
		}
	}
}
