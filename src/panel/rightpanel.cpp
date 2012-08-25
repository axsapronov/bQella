#include "rightpanel.h"
#include "ui_rightpanel.h"

#include "mainwindow.h"
#include "config.h"


#include <QDebug>

RightPanel::RightPanel(QWidget *parent, MainWindow *h):
    QWidget(parent),
    ui(new Ui::RightPanel)
{
    ui->setupUi(this);
}

RightPanel::~RightPanel()
{
    delete ui;
}
void RightPanel::refreshInfo()
{
    clearInfo();
    ui->LABibleName->setText(Config::configuration() -> profile() -> props["biblename"]);
    ui->LAShortName->setText(Config::configuration() -> profile() -> props["bibleshortname"]);
    ui->LAAuthor->setText(Config::configuration() -> profile() -> props["copyright"]);
    ui->LAEnglishPsalms->setText(Config::configuration() -> profile() -> props["endlishpsalms"]);
    ui->LATypeModule->setText(Config::configuration() -> profile() -> props["moduletype"]);
    ui->LAVersion->setText(Config::configuration()->profile()->props["version"]);
    ui->LAOldTestament->setText(Config::configuration() -> profile() -> props["oldtestament"]);
    ui->LANewTestament->setText(Config::configuration() -> profile() -> props["newtestament"]);
    ui->LAApocrypha->setText(Config::configuration() -> profile() -> props["apocrypha"]);
    ui->LAChapterZero->setText(Config::configuration() -> profile() -> props["chapterzero"]);
    ui->LAUseChapterHead->setText(Config::configuration() -> profile() -> props["usechapterhead"]);
    ui->LAUseRightAlignment->setText(Config::configuration() -> profile() -> props["userightalignment"]);
    ui->LAStrongNumbers->setText(Config::configuration() -> profile() -> props["strongsnumber"]);
    ui->LAStrongNumbersDir->setText(Config::configuration() -> profile() -> props["strongsdirectory"]);
    ui->LASoundDir->setText(Config::configuration() -> profile() -> props["sounddirectory"]);
    ui->LALanguage->setText(Config::configuration() -> profile() -> props["language"]);
    ui->LAHTMLFilter->setText(Config::configuration() -> profile() -> props["htmlfilter"]);
    ui->LAInstallFonts->setText(Config::configuration() -> profile() -> props["installfonts"]);
    ui->LADesiredFontName->setText(Config::configuration() -> profile() -> props["desiredfontname"]);
    ui->LACategories->setText(Config::configuration() -> profile() -> props["categories"]);
    ui->LADesiderFontPath->setText(Config::configuration() -> profile() -> props["desiredfontpath"]);
    ui->LAEncoding->setText(Config::configuration() -> profile() -> props["defaultencoding"]);
    ui->LADesiredUiFont->setText(Config::configuration() -> profile() -> props["desireduifont"]);
}

void RightPanel::clearInfo()
{
    clearInfoProject();
}

void RightPanel::clearInfoProject()
{
    ui->LABibleName->clear();
    ui->LACategories->clear();
    ui->LADesiderFontPath->clear();
    ui->LAEncoding->clear();
    ui->LADesiderFontPath->clear();
    ui->LADesiredFontName->clear();
    ui->LAInstallFonts->clear();
    ui->LAHTMLFilter->clear();
    ui->LALanguage->clear();
    ui->LAShortName->clear();
    ui->LAAuthor->clear();
    ui->LAEnglishPsalms->clear();
    ui->LATypeModule->clear();
    ui->LAVersion->clear();
    ui->LAOldTestament->clear();
    ui->LANewTestament->clear();
    ui->LAApocrypha->clear();
    ui->LAChapterZero->clear();
    ui->LAUseChapterHead->clear();
    ui->LAUseRightAlignment->clear();
    ui->LAStrongNumbers->clear();
    ui->LAStrongNumbersDir->clear();
}
