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


#include "settings.h"
#include "config.h"


#include <QtDebug> //to use qWarning and qDebug messages
#include <QFileDialog>


//------------------------------------------------------------------------------
AppSettings::AppSettings(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    createConnects();
    ui.CBItemAutoProperties->setChecked(true);
    ui.CBItemAutoProperties->setVisible(false);
    ui.CBContentsAdditionalView->setVisible(false);

}
//------------------------------------------------------------------------------
void AppSettings::createConnects()
{
    connect(ui.PBFontSettings, SIGNAL(clicked()), this, SLOT(showFontSettingsDialog()));
    connect(ui.pBSpellDict, SIGNAL(clicked()), this, SLOT(setSpell()));
}
//------------------------------------------------------------------------------
void AppSettings::reject()
{
    QWidget::hide();  //close dialog
}

//------------------------------------------------------------------------------
void AppSettings::accept()
{
    apply();
    QWidget::hide();  //close dialog
}

//------------------------------------------------------------------------------
void AppSettings::apply()
{
    Config::configuration() -> setContentsAdditionalView(ui.CBContentsAdditionalView -> isChecked());
    emit showContentsAV(ui.CBContentsAdditionalView -> isChecked());
    Config::configuration() -> setShowSubItemsTitle(ui.CBShowSubItemsTitle -> isChecked());
    emit showContentsAVHeader(ui.CBShowSubItemsTitle -> isChecked());
    Config::configuration() -> setAutoCollapse(ui.CBAutoCollapse -> isChecked());
    Config::configuration() -> setAppLogLevel(ui.SBAppLogLevel -> value());
    Config::configuration() -> setPrjLogLevel(ui.SBPrjLogLevel -> value());
    //	Config::configuration() -> setItemAutoProperties(ui.CBItemAutoProperties -> isChecked());
    Config::configuration() -> setAutoNumbers(ui.cBAutoSetNumbers->isChecked());
    Config::configuration() -> setItemAutoProperties(true);
}

//------------------------------------------------------------------------------
void AppSettings::set()
{
    ui.CBContentsAdditionalView -> setChecked(Config::configuration() -> ContentsAdditionalView());
    ui.CBShowSubItemsTitle -> setChecked(Config::configuration() -> ShowSubItemsTitle());
    ui.CBAutoCollapse -> setChecked(Config::configuration() -> AutoCollapse());
    ui.CBItemAutoProperties -> setChecked(true);
    ui.SBAppLogLevel -> setValue(Config::configuration() -> AppLogLevel());
    ui.SBPrjLogLevel -> setValue(Config::configuration() -> PrjLogLevel());
    ui.cBAutoSetNumbers -> setChecked(Config::configuration()->AutoNumbers());
    //    ui.CBAcceptDropImages->setChecked(Config::configuration()->AcceptDropImages());
}

//------------------------------------------------------------------------------
void AppSettings::showFontSettingsDialog()
{
    Config *config = Config::configuration();
    FontSettings settings = config -> fontSettings();

    { // It is important that the dialog be deleted before UI mode changes.
        FontSettingsDialog dialog;
        if (!dialog.showDialog(&settings))
            return;
    }
    config -> setFontPointSize(settings.browserFont.pointSizeF());
    config -> setFontSettings(settings);

    emit updateApplicationFontSettings(settings);
}
//------------------------------------------------------------------------------
void AppSettings::setSpell()
{
    QString SpellDic = "";
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Select Dictionary "), SpellDic, tr("Dictionary (*.dic)"));

    QString new_Dict = fileName;

    if (SpellDic != new_Dict)
    {
        SpellDic = new_Dict;
        highlighter->setDict(SpellDic);
        Config::configuration()->setSpellDict(SpellDic);
        emit signalSetSpell();
    }
}
