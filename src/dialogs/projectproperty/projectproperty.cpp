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


#include "projectproperty.h"
#include "config.h"
#include "mainwindow.h"
#include "pcommon.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>


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

    //    ui.checkBox

    ui.cbOldTestament -> setChecked(pr.oldTestament);
    ui.cbNewTestament -> setChecked(pr.newTestament);

    ui.cbApocrypha -> setChecked(pr.apocrypha);
    ui.cbChapterZero -> setChecked(pr.chapterZero);
    ui.cbEnglishPsalms -> setChecked(pr.englishPsalms);
    qDebug() << "old = " << pr.oldTestament << "cb = " << ui.cbStrongNumber->isChecked();
    ui.cbStrongNumber -> setChecked(pr.strongNumber);
    qDebug() << "old = " << pr.oldTestament << "cb = " << ui.cbStrongNumber->isChecked();
    ui.cbUseChapterHead -> setChecked(pr.useChapterHead);
    ui.cbUseRightAlignment -> setChecked(pr.useRightAlignment);
    ui.cbNoForcedLineBreaks -> setChecked(pr.noForcedLineBreaks);

    ui.lECategories -> setText(pr.categories);
    ui.comBEncoding -> setCurrentIndex( ui.comBEncoding->findText(pr.defaultEncoding));
//    ui.lEDefaultEncoding -> setText(pr.defaultEncoding);
    ui.lEDesiredFontName -> setText(pr.desiredFontName);
    ui.lEDesiredFontPath -> setText(pr.desiredFontPath);
    ui.lEStrongDir -> setText(pr.strongsDirectory);
    ui.lESoundDir -> setText(pr.soundDirectory);
    ui.lEInstallFonts -> setText(pr.installFonts);
    ui.lEDesiredUIFont -> setText(pr.desiredUIFont);
    ui.comBLanguage->setCurrentIndex(ui.comBLanguage->findText(pr.language));
    //    ui.comBLanguage ->currentText() .s
    //    установить язык

    //    // HTMLFilter должен автоматом создаваться
    //    QString language;

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
        QDir dir(prjFN);
        //    qDebug() << "path = " << path <<  "last = " << path.last();

        dir.mkdir(ui.lineEditBibleName -> text());

        //check for valid project file name
        //            qDebug() << "Debug: _ProjectProperties::accept()" << "Eprj = " << ui.EprjFileName -> text() << "biblname = " << ui.lineEditBibleName -> text();
        //            qDebug() << "Debug: _ProjectProperties::accept()" << "firePrj = " << prjFN+ui.lineEditBibleName -> text()+"/"+ui.lineEditBibleName -> text()+GL_Project_File;
        QFile filePrj(prjFN+ui.lineEditBibleName -> text()+"/"+ui.lineEditBibleName -> text()+GL_Project_File);
        if (!filePrj.exists())
        {		//create file if it's not exist
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
                QString str_body = "<body>"+tr("<p> добавить инструкцию для создания модулей</p>");

                Config::configuration() -> setModuleBiblename(ui.lineEditBibleName -> text());
                Config::configuration() -> setModuleBibleShortName(ui.lineEditBibleShortName -> text());
                Config::configuration() -> setModuleCopyright(ui.lineEditCopyright -> text());

                Config::configuration() -> setOldTestament(ui.cbOldTestament -> checkState());
                Config::configuration() -> setNewTestament(ui.cbNewTestament -> checkState());
                Config::configuration() -> setApocrypha(ui.cbApocrypha -> checkState());
                Config::configuration() -> setChapterZero(ui.cbChapterZero -> checkState());
                Config::configuration() -> setEnglishPsalms(ui.cbEnglishPsalms -> checkState());
                Config::configuration() -> setStrongNumber(ui.cbStrongNumber -> checkState());
                Config::configuration() -> setUseChapterHead(ui.cbUseChapterHead -> checkState());
                Config::configuration() -> setUseRightAlignment(ui.cbUseRightAlignment ->checkState());
                Config::configuration() -> setNoForcedLineBreaks(ui.cbNoForcedLineBreaks -> checkState());

                Config::configuration() -> setCategories(ui.lECategories -> text());
                Config::configuration() -> setDefaultEncoding(ui.comBEncoding->currentText());
                Config::configuration() -> setDesiredFontName(ui.lEDesiredFontName -> text());
                Config::configuration() -> setDesiredFontPath(ui.lEDesiredFontPath -> text());
                Config::configuration() -> setStrongsDirectory(ui.lEStrongDir -> text());
                Config::configuration() -> setSoundDirectory(ui.lESoundDir -> text());
                Config::configuration() -> setInstallFonts(ui.lEInstallFonts -> text());
                Config::configuration() -> setDesiredUIFont(ui.lEDesiredUIFont -> text());
                Config::configuration() -> setLanguage(ui.comBLanguage->currentText());

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


//            qDebug() << "before old = " << prop.oldTestament;
            prop.oldTestament = ui.cbOldTestament -> isChecked();
//            qDebug() << "after old = " << prop.oldTestament;
            prop.newTestament = ui.cbNewTestament -> isChecked();
            prop.apocrypha = ui.cbApocrypha -> isChecked();
            prop.chapterZero = ui.cbChapterZero -> isChecked();
            prop.englishPsalms = ui.cbEnglishPsalms -> isChecked();
            prop.strongsDirectory = ui.cbStrongNumber -> isChecked();
            prop.useChapterHead = ui.cbUseChapterHead -> isChecked();
            prop.useRightAlignment = ui.cbUseRightAlignment -> isChecked();
            prop.noForcedLineBreaks = ui.cbNoForcedLineBreaks -> isChecked();

            prop.categories = ui.lECategories -> text();
            prop.defaultEncoding = ui.comBEncoding->currentText();
            prop.language = ui.comBLanguage->currentText();
            prop.desiredFontName = ui.lEDesiredFontName -> text();
            prop.desiredFontPath = ui.lEDesiredFontPath -> text();
            prop.strongsDirectory = ui.lEStrongDir -> text();
            prop.soundDirectory = ui.lESoundDir -> text();
            prop.installFonts = ui.lEInstallFonts -> text();
            prop.desiredUIFont = ui.lEDesiredUIFont -> text();

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
