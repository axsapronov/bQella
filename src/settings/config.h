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
#ifndef CONFIG_H
#define CONFIG_H

#include "profile.h"
#include "docuparser.h"

#include <QString>
#include <QStringList>
#include <QMap>

#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

#define GL_Prog_RA_VERSION_STR	"0.0"
#define GL_Prog_BUILD_STR	"2011.08.2"
#define GL_Prog_Name "WarmongeR Project"
#define GL_Prog_Author "Alexander Sapronov"

//.pjm  = project module
#define GL_Project_File ".pem"
#define GL_Project_Conf_File ".ini"


class Profile;

struct FontSettings
{
    FontSettings() : useWindowFont(false), useBrowserFont(false),
        windowWritingSystem(QFontDatabase::Latin), browserWritingSystem(QFontDatabase::Latin)
    { }

    QFont windowFont;
    QFont browserFont;

    bool useWindowFont;
    bool useBrowserFont;

    QFontDatabase::WritingSystem windowWritingSystem;
    QFontDatabase::WritingSystem browserWritingSystem;
};

//====================== class Config ============================
class Config
{
public:

    Config();

    void loadSettings();
    void saveSettings();
    Profile *profile() const { return profil; }
    QString profileName() const { return profil->props[QLatin1String("name")]; }
    QString profileTitle() const;
    bool validProfileName() const;
    void hideSideBar( bool b );
    bool sideBarHidden() const;
    QStringList mimePaths();
    QString getProjectProperty(QString prop, QString prjFN); //especially for not current projects. Gets projects' property without loading.
    void toAppLog(int logLevel, QString msg);
    void toPrjLog(int logLevel, QString msg);


    // From profile, read only
    QStringList docFiles() const;
    QStringList ProfileFNs() const { return profileFNs; }
    void addProject(QString prj) { profileFNs << prj; }
    void delProject(QString prj);

    // From QSettings, read / write
    QString homePage() const;
    QStringList source() const;

    void setHomePage( const QString &hom ) { startPage = hom; }
    void setSource( const QStringList &s ) { src = s; }

    int sideBarPage() const { return sideBar; }
    void setSideBarPage( int sbp ) { sideBar = sbp; }

    QByteArray windowGeometry() const { return winGeometry; }
    void setWindowGeometry( const QByteArray &geometry ) { winGeometry = geometry; }

    QByteArray mainWindowState() const { return mainWinState; }
    void setMainWindowState( const QByteArray &state ) { mainWinState = state; }

    qreal fontPointSize() const 						{ return pointFntSize; }
    void setFontPointSize(qreal size);
    FontSettings fontSettings() 						{ return m_fontSettings; }
    void setFontSettings(const FontSettings &settings)	{ m_fontSettings = settings; }

    bool docRebuild() const			{ return rebuildDocs; }
    void setDocRebuild( bool rb )	{ rebuildDocs = rb; }

    static Config *configuration();
    void loadProject(const QString &projectFileName);

    //variables for global use via  Config::configuration()->variable()
    QString AppDir()	{ return appDir; }
    QString BackupDir()	{ return backupDir; }
    QString CacheDir()	{ return cacheDir; }
    QString PrjDir()	{ return prjDir; }
    QString ImgDir()	{ return imgDir; }
    QString CurPrjDir()	{ return curPrjDir; }
    QString CurProject(){ return curProject; }
    QString CurFile()	{ return curFile; }
    QString CurPrjSrc()	{ return curPrjSrc; }
    QString Lang()		{ return lang; }
    QString ErrPage()	{ return errPage; }
    QString IniFile()	{ return iniFile; }
    QString DbName()	{ return dbName; }
    QString ContentsSortOrder()	{ return contentsSortOrder; }
    QString IndentString()	{ return "   "; }

    //
    //warmonger
    QString ModuleBiblename()  { return moduleBiblename;}
    QString ModuleCopyright()  { return moduleCopyright;}
    QString ModuleBibleShortName() { return moduleBibleShortName;}



    void setAppDir(QString dir)		{ appDir = dir; }
    void setBackupDir(QString dir)	{ backupDir = dir; }
    void setCacheDir(QString dir)	{ cacheDir = dir; }
    void setPrjDir(QString dir)		{ prjDir = dir; }
    void setImgDir(QString dir)		{ imgDir = dir; }
    void setCurPrjDir(QString dir)	{ curPrjDir = dir; }
    void setCurProject(QString prjFN)	{ curProject = prjFN; }
    void setCurFile(QString fn)		{ curFile = fn; }
    void setCurPrjSrc();
    void setLang(QString lng)		{ lang = lng; }
    void setErrPage(QString fn)		{ errPage = fn; }
    void setIniFile(QString fn)		{ iniFile = fn; }
    void setDbName(QString fn)		{ dbName = fn; }
    void setContentsSortOrder(QString order)	{ contentsSortOrder = order; }

    //warmonger
    void setModuleBiblename(QString fn)   { moduleBiblename = fn; }
    void setModuleCopyright(QString fn)   { moduleCopyright = fn; }
    void setModuleBibleShortName(QString fn)    { moduleBibleShortName = fn; }




    //variables from Settings window
    QString AppLogFN()	{ return appLogFN; }
    QString PrjLogFN()	{ return prjLogFN; }
    int AppLogLevel()	{ return appLogLevel; }
    int PrjLogLevel()	{ return prjLogLevel; }
    QString ExternalEditor()	{ return externalEditor; }
    QString ExternalBrowser()	{ return externalBrowser; }
    QString ExternalArchiver()	{ return externalArchiver; }
    QString ExternalArchiverOptions()	{ return externalArchiverOptions; }
    bool ContentsAdditionalView()	{ return contentsAdditionalView; }
    bool ShowSubItemsTitle()		{ return showSubItemsTitle; }
    bool AutoCollapse()				{ return autoCollapse; }
    bool ItemAutoProperties()		{ return itemAutoProperties; }

    void setAppLogFN(QString fn)	{ appLogFN = fn; }
    void setAppLogLevel(int n)		{ appLogLevel = n; }
    void setPrjLogLevel(int n)		{ prjLogLevel = n; }
    void setItemAutoProperties(bool b)	{ itemAutoProperties = b; }
    void setExternalEditor(QString fn)	{ externalEditor = fn; }
    void setExternalBrowser(QString fn)	{ externalBrowser = fn; }
    void setExternalArchiver(QString fn){ externalArchiver = fn; }
    void setExternalArchiverOptions(QString opts)	{ externalArchiverOptions = opts; }

    //variables from Signature window
    int DefaultSignatureID()	{ return defaultSignatureID; }

    void setDefaultSignatureID(int id)	{ defaultSignatureID = id; }

public slots:
    void setContentsAdditionalView(bool checked)	{ contentsAdditionalView = checked; }
    void setShowSubItemsTitle(bool checked)			{ showSubItemsTitle = checked; }
    void setAutoCollapse(bool checked)				{ autoCollapse = checked; }

private:
    Config( const Config &c );
    Config& operator=( const Config &c );

private:
    Profile *profil;
    Profile *profil_tmp; //to build project list
    
    QStringList profileFNs;	//list of projects
    QString startPage;  //show this page when application is started.  Should be last veiwed page before app close
    QStringList src;    //opened files in tab browser
    QByteArray mainWinState;
    QByteArray winGeometry;
    qreal pointFntSize;
    int sideBar;
    bool hideSidebar;
    bool rebuildDocs;
    FontSettings m_fontSettings;

    //variables for global use via  Config::configuration()->variable()
    QString appDir;
    QString backupDir;
    QString cacheDir;	//path for cache
    QString prjDir;
    QString imgDir;
    QString curPrjDir;
    QString curProject;	// current project file name *.pep
    QString curFile;	// opened file. We need this to save any changes automaticly
    QString curPrjSrc;
    QString lang;
    QString errPage;	// html file to output application errors
    QString iniFile;
    QString appLogFN;
    QString prjLogFN;
    int appLogLevel;
    int prjLogLevel;
    QString dbName;
    QString contentsSortOrder;

    //warmonger
    QString moduleBiblename;
    QString moduleCopyright;
    QString moduleBibleShortName;


    //variables from Settings window
    bool contentsAdditionalView;
    bool showSubItemsTitle;
    bool autoCollapse;
    bool itemAutoProperties;
    QString externalEditor;
    QString externalBrowser;
    QString externalArchiver;
    QString externalArchiverOptions;
    int defaultSignatureID;
};

#endif // CONFIG_H
