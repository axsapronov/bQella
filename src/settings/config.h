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


#ifndef CONFIG_H
#define CONFIG_H

#include "profile.h"
#include "docuparser.h"

#include <QString>
#include <QStringList>
#include <QMap>

#include <QtGui/QFont>
#include <QtGui/QFontDatabase>

#define GL_PROG_VERSION_STR	"0.11"
#define GL_PROG_BUILD_STR	"2012.08.25"
#define GL_PROG_NAME "bQella"
#define GL_PROG_AUTHOR "Sapronov Alexander"


//.pem  = project module
#define GL_PROJECT_FILE ".pem"
#define GL_PROJECT_CONF_FILE ".ini"

#define GL_WEB_SITE "http://warmonger1.github.com/bQella/"

#define GL_LENGT_ITEM_STRING 3  // минимальная длина для title в таблице. (надо чтобы export удачен был)
#define GL_PROCENT_OF_MATCHES 50 // процент совпадения shortname из файла и из списка, чтобы сократить название файлов

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
    QString profileName() const { return profil -> props[QString("name")]; }
    QString profileTitle() const;
    bool validProfileName() const;
    void hideSideBar( bool b );
    bool sideBarHidden() const;
    void hideRightPanel ( bool b );
    bool rightPanelHidden() const;
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

    //variables for global use via  Config::configuration() -> variable()
    QString AppDir()	{ return appDir; }
    QString BackupDir()	{ return backupDir; }
    QString CacheDir()	{ return cacheDir; }
    QString PrjDir()	{ return prjDir; }
    QString ImgDir()	{ return imgDir; }
    QString CurPrjDir()	{ return curPrjDir; }
    QString CurPrjName(){ return curPrjName; }
    QString CurProject(){ return curProject; }
    QString CurPrjImgDir()	{ return curPrjImgDir; }
    QString CurFile()	{ return curFile; }
    QString CurPrjSrc()	{ return curPrjSrc; }
    QString Lang()	{ return lang; }
    QString ErrPage()	{ return errPage; }
    QString IniFile()	{ return iniFile; }
    QString DbName()	{ return dbName; }
    QString ContentsSortOrder()	{ return contentsSortOrder; }
    QString IndentString()	{ return "   "; }

    bool AcceptDropImages()			{ return acceptDropImages; }

    bool AutoNumbers() { return autoNumbers; }
    //
    //settings modules
    QString ModuleBiblename()  { return moduleBiblename;}
    QString ModuleCopyright()  { return moduleCopyright;}
    QString ModuleBibleShortName() { return moduleBibleShortName;}

    double Version() {return version;}
    bool ModuleType() {return moduleType;}
    bool OldTestament() {return oldTestament;}
    bool NewTestament() {return newTestament;}
    bool Apocrypha() {return apocrypha;}
    bool ChapterZero() {return chapterZero;}
    bool EnglishPsalms() {return englishPsalms;}
    bool StrongNumber() {return strongNumber;}
    QString StrongsDirectory() {return strongsDirectory;}
    QString SoundDirectory() {return soundDirectory;}
    bool NoForcedLineBreaks() {return noForcedLineBreaks;}
    // HTMLFilter должен автоматом создаваться
    QString Language() {return language;}
    QString InstallFonts() {return installFonts;}
    QString HtmlFilter() {return htmlFilter;}
    QString DesiredFontName() {return desiredFontName;}
    QString Categories() {return categories;}
    QString DesiredFontPath() {return desiredFontPath;}
    QString DefaultEncoding() {return defaultEncoding;}
    QString DesiredUIFont() {return desiredUIFont;}
    bool UseRightAlignment() {return useRightAlignment;}
    bool UseChapterHead() {return useChapterHead;}



//    double ModuleVersion() { return moduleVersion;}


    void setAutoNumbers(bool bo)    { autoNumbers = bo; }
    void setAppDir(QString dir)		{ appDir = dir; }
    void setBackupDir(QString dir)	{ backupDir = dir; }
    void setCacheDir(QString dir)	{ cacheDir = dir; }
    void setPrjDir(QString dir)		{ prjDir = dir; }
    void setImgDir(QString dir)		{ imgDir = dir; }
    void setCurPrjDir(QString dir)	{ curPrjDir = dir; curPrjImgDir = dir + "/images";}
    void setCurPrjName(QString name){ curPrjName = name; }
    void setCurProject(QString prjFN)	{ curProject = prjFN; }
    void setCurFile(QString fn)		{ curFile = fn; }
    void setCurPrjSrc();
    void setLang(QString lng)		{ lang = lng; }
    void setErrPage(QString fn)		{ errPage = fn; }
    void setIniFile(QString fn)		{ iniFile = fn; }
    void setDbName(QString fn)		{ dbName = fn; }
    void setContentsSortOrder(QString order)	{ contentsSortOrder = order; }

    //Module
    void setModuleBiblename(QString fn)   { moduleBiblename = fn; }
    void setModuleCopyright(QString fn)   { moduleCopyright = fn; }
    void setModuleBibleShortName(QString fn)    { moduleBibleShortName = fn; }
    void setModuleVersion(double ve)    { version = ve; }


    void setModuleType(bool fn) { moduleType = fn; }
    void setOldTestament(bool fn) { oldTestament = fn; }
    void setNewTestament(bool fn) { newTestament = fn; }
    void setApocrypha(bool fn) { apocrypha = fn; }
    void setChapterZero(bool fn) { chapterZero = fn; }
    void setEnglishPsalms(bool fn) { englishPsalms = fn; }
    void setStrongNumber(bool fn) { strongNumber = fn; }
    void setStrongsDirectory(QString fn) { strongsDirectory = fn;}
    void setSoundDirectory(QString fn) { soundDirectory = fn;}
    void setNoForcedLineBreaks(bool fn) {noForcedLineBreaks = fn; }

    void setLanguage(QString fn) { language = fn;}
    void setHtmlFilter(QString fn) { htmlFilter = fn;}
    void setInstallFonts(QString fn) { installFonts = fn;}
    void setDesiredFontName(QString fn) { desiredFontName = fn;}
    void setCategories(QString fn) { categories = fn;}
    void setDesiredFontPath(QString fn) { desiredFontPath = fn;}
    void setDefaultEncoding(QString fn) { defaultEncoding = fn;}
    void setDesiredUIFont(QString fn) { desiredUIFont = fn;}
    void setUseRightAlignment(bool fn) {useRightAlignment = fn; }
    void setUseChapterHead(bool fn) {useChapterHead = fn; }




    void setAcceptDropImages(bool b)	{ acceptDropImages = b; }
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

    Profile *profil;
    Profile *profil_tmp; //to build project list

    QStringList profileFNs;	//list of projects
    QString startPage;  //show this page when application is started.  Should be last veiwed page before app close
    QStringList src;    //opened files in tab browser
    QByteArray mainWinState;
    QByteArray winGeometry;
    qreal pointFntSize;
    int sideBar;
    int rightPanel;
    bool hideSidebar;
    bool hideRightpanel;
    bool rebuildDocs;
    FontSettings m_fontSettings;

    //variables for global use via  Config::configuration() -> variable()

    bool autoNumbers;
    QString appDir;
    QString backupDir;
    QString cacheDir;	//path for cache
    QString prjDir;
    QString imgDir;
    QString curPrjDir;
    QString curProject;	// current project file name *.pep
    QString curPrjImgDir;
    QString curFile;	// opened file. We need this to save any changes automaticly
    QString curPrjSrc;
    QString curPrjName;
    QString lang;
    QString errPage;	// html file to output application errors
    QString iniFile;
    QString appLogFN;
    QString prjLogFN;
    int appLogLevel;
    int prjLogLevel;
    QString dbName;
    QString contentsSortOrder;

    //module
    QString moduleBiblename;
    QString moduleCopyright;
    QString moduleBibleShortName;

    bool moduleType;
    double version;
    bool oldTestament;
    bool newTestament;
    bool apocrypha;
    bool chapterZero;
    bool englishPsalms;
    bool strongNumber;
    QString strongsDirectory;
    QString soundDirectory;
    bool noForcedLineBreaks;
    QString htmlFilter;
    QString language;
    QString installFonts;
    QString desiredFontName;
    QString categories;
    QString desiredFontPath;
    QString defaultEncoding;
    QString desiredUIFont;
    bool useRightAlignment;
    bool useChapterHead;

    //variables from Settings window
    bool contentsAdditionalView;
    bool showSubItemsTitle;
    bool autoCollapse;
    bool itemAutoProperties;
    bool acceptDropImages;
    QString externalEditor;
    QString externalBrowser;
    QString externalArchiver;
    QString externalArchiverOptions;
    int defaultSignatureID;

};

#endif // CONFIG_H
