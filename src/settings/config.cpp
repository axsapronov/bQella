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

#include "config.h"
#include "profile.h"
#include "docuparser.h"
#include "pcommon.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFont>
#include <QFontInfo>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QtXml>
#include <QList>
#include <QtDebug> 			//to use qWarning and qDebug messages
#include <QTextStream>		//to use [out << "text"] in ProjectSave
#include <QTextDocument>	//to use Qt::escape()
#include <QMap>				//for saveProject
#include <QMessageBox>
#include <QTranslator>

static Config *static_configuration = 0;

//-------------------------------------------------
Config::Config()
    : profil(0), hideSidebar(false), rebuildDocs(true)
{
    if( !static_configuration ) {
        static_configuration = this;
    } else {
        qWarning( "Multiple configurations not allowed!" );
    }
}

//-------------------------------------------------
Config *Config::configuration()
{
    Q_ASSERT( static_configuration );
    return static_configuration;
}

//-------------------------------------------------
void Config::loadProject(const QString &projectFileName)
{
    toAppLog(2, "Load project: " + projectFileName);
    QString prjFN = projectFileName;
    QFileInfo fi(prjFN);
//    qDebug() << "Debug: Config::loadProject() " << "prjfn = " << prjFN;

    if ( !(fi.exists() && fi.isFile()) ){
        qWarning( (QString("Project does not exist2: ") + prjFN).toUtf8().constData() );
        toAppLog(2, "- project does not exist: " + prjFN); 
        //QMessageBox::warning(this, "No project", "Project does not exist:\n"+ prjFN, QMessageBox::Ok, QMessageBox::Ok); //only for QWidget child, but class "Config" is not.
        prjFN = PrjDir() + "help/help.pem";
        fi.setFile(prjFN);
        qWarning( (QString("Open help project instead: ") + prjFN).toAscii().constData() );
        toAppLog(2, "- open help project instead: " + prjFN);
    } 
    
    setCurProject(prjFN);
    setCurPrjDir(fi.absolutePath());
    DocuParser *parser = DocuParser::createParser( prjFN );
    if (!parser) {
        qWarning( (QString("Failed to create parser for file: ") + prjFN).toAscii().constData() );
        toAppLog(2, "- failed to create parser for file: " + prjFN);
    }
    DocuParserRAP *profileParser = static_cast<DocuParserRAP*>(parser);
    QFile file(prjFN);
    parser -> parse(&file);
    profil = profileParser -> profile();
    if (!profil) {
        qWarning( (QString("Config::loadProject(), no profile in: ") + prjFN).toAscii().constData() );
        toAppLog(2, "- no profile in: " + prjFN);
    }
    profil -> setDocuParser(profileParser);
    setCurFile(profil -> props["startpage"]);
    setCurPrjSrc();
    prjLogFN = AppDir() + "log/" + profileName() + ".log";
//    qDebug() << "Debug: _Config::loadProject()" << "curProject = " << curProject;
//    qDebug() << "Debug: _Config::loadProject()" << "curFile = " << curFile;
//    qDebug() << "Debug: _Config::loadProject()" << "sources file = " << CurPrjSrc();
    toAppLog(2, "- loaded succesfully");
}

//-------------------------------------------------
void Config::loadSettings()
{
    QSettings settings(iniFile, QSettings::IniFormat);
    setAppLogLevel( settings.value(QString("LogLevel-Application") ).toInt() );
    setPrjLogLevel( settings.value(QString("LogLevel-Project") ).toInt() );
    toAppLog(2, "Load application settings");

    //miscellaneous settings
    lang = settings.value(QString("Language") ).toString();
    src = settings.value(QString("Source") ).toStringList();
    QStringList::iterator it = src.begin();
    for (; it != src.end(); ++it) {
    	*it = urlifyFileName( absolutifyFileName(*it, prjDir) );
    }    
    sideBar = settings.value(QString("SideBarPage") ).toInt();
    rebuildDocs = settings.value(QString("RebuildDocDB"), true ).toBool();
    profileFNs = settings.value(QString("Projects") ).toStringList();
    profileFNs = absolutifyFileList(profileFNs, prjDir);	//we will load this list to widget in HelpDialog::initialize()
    curProject = settings.value(QString("ActiveProject") ).toString();
    curProject = absolutifyFileName(curProject, prjDir);	 //absolutify project file path
    setContentsSortOrder(settings.value(QString("ContentsSortOrder") ).toString());

    //window and font settings
    winGeometry = settings.value(QString("WindowGeometry")).toByteArray();
    mainWinState = settings.value(QString("MainWindowState")).toByteArray();
    pointFntSize = settings.value(QString("FontSize"), qApp -> font().pointSizeF()).toDouble();
    m_fontSettings.windowFont = qVariantValue<QFont>(settings.value(QString("WindowFont"), qApp -> font()));
    m_fontSettings.browserFont = qVariantValue<QFont>(settings.value(QString("BrowserFont"), qApp -> font()));
    m_fontSettings.useWindowFont = settings.value(QString("UseWindowFont"), false).toBool();
    m_fontSettings.useBrowserFont = settings.value(QString("UseBrowserFont"), false).toBool();
    m_fontSettings.windowWritingSystem = static_cast<QFontDatabase::WritingSystem>(
                settings.value(QString("WindowWritingSystem"), QFontDatabase::Latin).toInt());
    m_fontSettings.browserWritingSystem = static_cast<QFontDatabase::WritingSystem>(
                settings.value(QString("BrowserWritingSystem"), QFontDatabase::Latin).toInt());
    m_fontSettings.browserFont.setPointSizeF(pointFntSize);

    //settings from Settings window
    setContentsAdditionalView(	settings.value(QString("ContentsAdditionalView")).toBool() );
    setShowSubItemsTitle(		settings.value(QString("ShowSubItemsTitle")).toBool() );
    setAutoCollapse(			settings.value(QString("AutoCollapse")).toBool() );
    backupDir = settings.value(QString("BackupPath") ).toString();
    backupDir = absolutifyFileName(backupDir, prjDir);		//absolutify path to backup dir
    externalEditor = settings.value(QString("ExternalEditor") ).toString();
    externalEditor = absolutifyFileName(externalEditor, prjDir); //absolutify file path to editor
    externalBrowser = settings.value(QString("ExternalBrowser") ).toString();
    externalBrowser = absolutifyFileName(externalBrowser, prjDir);	//absolutify file path to browser
    externalArchiver = settings.value(QString("ExternalArchiver") ).toString();
    externalArchiver = absolutifyFileName(externalArchiver, prjDir);	//absolutify file path to archiver
    externalArchiverOptions = settings.value(QString("ExternalArchiverOptions") ).toString();
    setItemAutoProperties(settings.value(QString("ItemAutoProperties")).toBool() );

    //settings from Signature window
    setDefaultSignatureID(		settings.value(QString("DefaultSignatureID")).toInt() );

    toAppLog(2, "- done");
}//loadSettings()

//-------------------------------------------------
void Config::saveSettings()
{
    toAppLog(2, "Save application settings");
    QSettings settings(iniFile, QSettings::IniFormat);
    
    //miscellaneous settings
    settings.setValue(QString("Language"), lang);
    settings.setValue(QString("Projects"), relatifyFileList(profileFNs, prjDir) );
    settings.setValue(QString("ActiveProject"), curProject /*relatifyFileName(curProject, prjDir) */);
    settings.setValue(QString("Source"),src );	 //paths relatified in MainWindow::saveSettings()
    settings.setValue(QString("SideBarPage"), sideBarPage() );
    settings.setValue(QString("RebuildDocDB"), rebuildDocs );
    settings.setValue(QString("ContentsSortOrder"), contentsSortOrder );

    //window and font settings
    settings.setValue(QString("WindowGeometry"), winGeometry);
    settings.setValue(QString("MainWindowState"), mainWinState );
    settings.setValue(QString("FontSize"), pointFntSize);
    settings.setValue(QString("WindowFont"), m_fontSettings.windowFont);
    settings.setValue(QString("BrowserFont"), m_fontSettings.browserFont);
    settings.setValue(QString("UseWindowFont"), m_fontSettings.useWindowFont);
    settings.setValue(QString("UseBrowserFont"), m_fontSettings.useBrowserFont);
    settings.setValue(QString("WindowWritingSystem"), m_fontSettings.windowWritingSystem);
    settings.setValue(QString("BrowserWritingSystem"), m_fontSettings.browserWritingSystem);

    //settings from Settings window
    settings.setValue(QString("ExternalEditor"), relatifyFileName(externalEditor, prjDir) );
    settings.setValue(QString("ExternalBrowser"), relatifyFileName(externalBrowser, prjDir) );
    settings.setValue(QString("ExternalArchiver"), relatifyFileName(externalArchiver, prjDir) );
    settings.setValue(QString("ExternalArchiverOptions"), ExternalArchiverOptions());
    settings.setValue(QString("BackupPath"), relatifyFileName(backupDir, prjDir) );
    settings.setValue(QString("ContentsAdditionalView"), contentsAdditionalView);
    settings.setValue(QString("ShowSubItemsTitle"), showSubItemsTitle);
    settings.setValue(QString("AutoCollapse"),autoCollapse);
    settings.setValue(QString("LogLevel-Application"),AppLogLevel());
    settings.setValue(QString("LogLevel-Project"),PrjLogLevel());
    settings.setValue(QString("ItemAutoProperties"),ItemAutoProperties());
    
    //settings from Signature window
    settings.setValue(QString("DefaultSignatureID"),DefaultSignatureID());
    
    toAppLog(2, "- done");
}//saveSettings()

//-------------------------------------------------
QStringList Config::mimePaths() //where to make full search. Used only in TabbedBrowser::createHelpWindow
{
    static QStringList lst;

    if( lst.count() > 0 )
        return lst;

    lst << curPrjDir;
    return lst;
}

//-------------------------------------------------
void Config::delProject(QString prj)
{
    int i, index=-1;
    int n = profileFNs.count();
    for (i=0; i<n; i++)
        if (profileFNs[i] == prj)
            index = i;
    if (index >=0)
    	profileFNs.removeAt(index);
}

//-------------------------------------------------
QString Config::profileTitle() const
{
    QString s = profil -> props[QString("title")];
    if (s.isEmpty())
        s = QObject::tr("[No title]");
    return s;
}

//-------------------------------------------------
QString Config::homePage() const
{
    return startPage.isEmpty() ? profil -> props[QString("startpage")] : startPage;
}

//-------------------------------------------------
QStringList Config::source() const
{
    return src.size() == 0 ? QStringList(profil -> props[QString("startpage")]) : src;
}

//-------------------------------------------------
QStringList Config::docFiles() const //used in HelpDialog to build keyword DB. Earlier it used to return profil -> docs
{
    QStringList tmp;
    tmp << curProject; //probably will need to change to something else.
    return tmp;
}

//-------------------------------------------------
void Config::hideSideBar( bool b ) {  hideSidebar = b; }

//-------------------------------------------------
bool Config::sideBarHidden() const {  return hideSidebar; }

//-------------------------------------------------
QString Config::getProjectProperty(QString prop, QString prjFN) 
{
    qDebug() << "Debug: _Config::getProjectProperty()" << "prjfn = " << prjFN;
    QFile file(prjFN);
    if (!file.exists()) {
        qWarning( (QString("Project does not exist1: ") + prjFN).toUtf8().constData() );
        toAppLog(1, "Failed to get property for project:" + prjFN);
        return ""; 
    }
    DocuParser *parser = DocuParser::createParser( prjFN );
    if (!parser) {
        qWarning( (QString("Failed to create parser for project: ") + prjFN).toUtf8().constData() );
        toAppLog(1, "Failed to create parser for project:" + prjFN);
        return "";
    }
    DocuParserRAP *profileParser = static_cast<DocuParserRAP*>(parser);
    parser -> parse(&file);
    profil_tmp = profileParser -> profile();
    if (!profil_tmp) {
        qWarning( (QString("Config::loadProject(), no profile in: ") + prjFN).toAscii().constData() );
        toAppLog(1, "No profile in:" + prjFN);
        return "";
    }
    return profil_tmp -> props[prop];
}

//-------------------------------------------------
void Config::setCurPrjSrc()	
{
    curPrjSrc = CurProject();
    curPrjSrc.chop(4);
    dbName = curPrjSrc + "-sources.db";
    curPrjSrc = curPrjSrc + "-sources.xml";	// !+! for future XML import-export
}	

//-------------------------------------------------
void Config::setFontPointSize(qreal size)
{
    pointFntSize = size;
    m_fontSettings.useBrowserFont = true;
    m_fontSettings.browserFont.setPointSizeF(size);
}

//-------------------------------------------------
void Config::toAppLog(int logLevel, QString msg)
{
    if (logLevel <= AppLogLevel())
        toLog(AppLogFN() ,msg);
}

//-------------------------------------------------
void Config::toPrjLog(int logLevel, QString msg)
{
    if (logLevel <= PrjLogLevel())
        toLog(PrjLogFN() ,msg);
}
