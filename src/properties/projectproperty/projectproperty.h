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
    QString strongsDirectory;
    QString soundDirectory;
    QString htmlFilter;
    QString language;
    QString installFonts;
    QString desiredFontName;
    QString categories;
    QString desiredFontPath;
    QString defaultEncoding;
    QString desiredUIFont;
    double moduleBVersion;
    bool moduleType;
    bool oldTestament;
    bool newTestament;
    bool apocrypha;
    bool chapterZero;
    bool englishPsalms;
    bool strongNumber;
    bool noForcedLineBreaks;
    bool useRightAlignment;
    bool useChapterHead;
} ModuleProperties;

/**
@class ProjectProperties
class


*/
class ProjectProperties : public QDialog
{
     Q_OBJECT

public:
        ProjectProperties(QWidget *parent = 0);

        /**
        @function
        */
        void showUpdate();
signals:
        void createProject(ModuleProperties propert);
        void updateProjectProperties(ModuleProperties propert);
public slots:

        /**
        @function
        @param
        @param
        */
        void setProperties(bool newPrj, ModuleProperties pr);

        /**
        @function
        @param
        @param
        */
        void setModeNewProject(bool t)  { modeNewProject = t;}
private slots:
        /**
          @function
          Hide/show LineEdit strong number (input dir of strong)
          If cbStrongNumber active - show
          */
        void hideshow();
private:
        Ui::ProjectProperty ui;

        /**
        @function
        */
        void accept();

        /**
        @function
        */
        void reject();

        /**
        @function
        */
        void setToolTipLabels();

        /**
        @function
        */
        void setData();

        QString prjTitle;
        QString prjFN;
        QString prjStartPage;
        bool modeNewProject;
        bool validProperties;
}; // class ProjectProperties

#endif // __PROJECTPROPERTY_H__
