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
}

void ProjectProperties::setProperties(bool newPrj, ModuleProperties pr)
{
    prjTitle= pr.prjTitle;
    prjFN = pr.prjFN;
    prjStartPage = pr.prjStartPage;
    modeNewProject = newPrj;


    QString version;
    version.setNum(pr.moduleBVersion);
    //        qDebug() << "Debug: _ProjectProperties::setProperties" << "version(str) = " << version << "version(double) = " << pr.moduleBVersion;

    ui.lineEditBibleName -> setText(pr.moduleBiblename); // добавил
    ui.lineEditBibleShortName -> setText(pr.moduleBibleShortName);
    ui.lineEditCopyright -> setText(pr.moduleCopyright);
    ui.doubleSpinBoxVersion -> setValue(pr.moduleBVersion);
    // для комбокса установить значение moduletype

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
    if (ui.lineEditBibleName -> text().isEmpty()){
        s = tr("- Please enter a bible name.\n");
        er = true;
    }
    if (ui.lineEditBibleShortName -> text().isEmpty()) {
        s += tr("- Please enter a short bible name.\n");
        er = true;
    }

    //check for correct start page
    /*if  (ui.EStartPage -> text().isEmpty()){ //|| (!itemprop -> valid()) )
  s += tr("- Please choose a valid start page.");
  er = true;
 }
        */
    if (er){
        QMessageBox::critical(this, tr("Project property error"), s);
    }else{

        // если папки export нету, то создаем, если есть, то удаляем содержимое
        QDir dir(prjFN);


        //    qDebug() << "path = " << path <<  "last = " << path.last();

        dir.mkdir(ui.lineEditBibleName -> text());

        //check for valid project file name
        //            qDebug() << "Debug: _ProjectProperties::accept()" << "Eprj = " << ui.EprjFileName -> text() << "biblname = " << ui.lineEditBibleName -> text();
        //            qDebug() << "Debug: _ProjectProperties::accept()" << "firePrj = " << prjFN+ui.lineEditBibleName -> text()+"/"+ui.lineEditBibleName -> text()+GL_Project_File;
        QFile filePrj(prjFN+ui.lineEditBibleName -> text()+"/"+ui.lineEditBibleName -> text()+GL_Project_File);
        if (!filePrj.exists()){		//create file if it's not exist
            if (filePrj.open(QIODevice::ReadWrite)){	//try to create file
                QTextStream ts(&filePrj);
                ts << "<pemproject>\n</pemproject>";
                filePrj.close();
            }else{
                QMessageBox::critical(this, tr("Project property error"), tr("Can not create file:\n%1").arg(ui.lineEditBibleName -> text()));
                er = true;
            }
        }
        //check for valid start page file name
//        QFile fileSP(prjFN+ui.lineEditBibleName -> text()+"/"+"Bibleqt"+GL_Project_Conf_File);
                QFile fileSP(prjFN+ui.lineEditBibleName -> text()+"/"+"   ___Instruction");
        if (!fileSP.exists()){		//create file if it does not exist
            if (fileSP.open(QIODevice::ReadWrite)){		//try to create file
                QTextStream ts(&fileSP);
                ts.setCodec("UTF-8");

                QString str_header = "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" +
                        tr("   ___Instruction") + "</title></head>\n";
//                QString str_body = "<body>"+tr("\nBibleName = %1"
//                                               "\nBibleShortName = %2"
//                                               "\nCopyright = %3"
//                                               "\nDefaultEncoding = utf-8"
//                                               "\nChapterSign = <h4>"
//                                               "\nVerseSign = <p>")
//                        .arg(ui.lineEditBibleName -> text())
//                        .arg(ui.lineEditBibleShortName -> text())
//                        .arg(ui.lineEditCopyright -> text());
                QString str_body = "<body>"+tr("<p> добавить инструкцию для создания модулей</p>");

                Config::configuration() -> setModuleBiblename(ui.lineEditBibleName -> text());
                Config::configuration() -> setModuleBibleShortName(ui.lineEditBibleShortName -> text());
                Config::configuration() -> setModuleCopyright(ui.lineEditCopyright -> text());
                Config::configuration() -> setModuleCopyright(ui.lineEditCopyright -> text());
                QString str_ender = "\n</body>\n</html>\n";


                ts << str_header << str_body << str_ender;
                //ts << str_conf;
                fileSP.close();


            }else{
                QMessageBox::critical(this, tr("Project property error"), tr("Can not create file:\n%1").arg(ui.lineEditBibleName -> text()));
                er = true;
            }
        }


        if (!er){	//project, start page and sources files do exist we can proceed with setting project properties
            ModuleProperties prop;
            prop.prjTitle= ui.lineEditBibleName -> text();//ui.ETitle -> text();
            prop.prjStartPage = urlifyFileName(prjFN+ui.lineEditBibleName -> text()+"/"+"   ___Instruction");
            prop.prjFN = urlifyFileName(prjFN+ui.lineEditBibleName -> text()+"/"+ui.lineEditBibleName -> text()+GL_Project_File);
            prop.moduleBiblename = ui.lineEditBibleName -> text();
            prop.moduleBibleShortName = ui.lineEditBibleShortName -> text();
            prop.moduleCopyright = ui.lineEditCopyright -> text();
            prop.moduleBVersion = ui.doubleSpinBoxVersion -> value();
//            prop.moduleType = ui.comboBoxTypeModule -> currentText();

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
