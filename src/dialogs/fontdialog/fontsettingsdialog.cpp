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
#include "fontsettingsdialog.h"
#include "fontpanel.h"

#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QStackedWidget>
#include <QtGui/QDialogButtonBox>

FontSettingsDialog::FontSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_windowFontPanel(new FontPanel(this))
    , m_browserFontPanel(new FontPanel(this))
    , m_dialogButtonBox(new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel))
{
    setModal(true);
    setWindowTitle(tr("Font Settings"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    QVBoxLayout *mainVLayout = new QVBoxLayout(this);
    QHBoxLayout *hboxLayout = new QHBoxLayout;
    mainVLayout -> addLayout(hboxLayout);

    QLabel *label = new QLabel(tr("Font settings for:"), this);
    label -> setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    hboxLayout -> addWidget(label);
    QComboBox *comboBox = new QComboBox(this);
    comboBox -> addItem(tr("Documents"));
    comboBox -> addItem(tr("Application"));
    hboxLayout -> addWidget(comboBox);

    m_windowFontPanel -> setCheckable(true);
    m_browserFontPanel -> setCheckable(true);

    const QString customSettings(tr("Use custom settings"));
    m_windowFontPanel -> setTitle(customSettings);
    m_browserFontPanel -> setTitle(customSettings);

    QStackedWidget *stackWidget = new QStackedWidget(this);
    stackWidget -> addWidget(m_browserFontPanel);
    stackWidget -> addWidget(m_windowFontPanel);

    mainVLayout -> addWidget(stackWidget);
    mainVLayout -> addWidget(m_dialogButtonBox);

    connect(m_dialogButtonBox , SIGNAL(rejected()), this, SLOT(reject()));
    connect(m_dialogButtonBox , SIGNAL(accepted()), this, SLOT(accept()));
    connect(comboBox, SIGNAL(activated(int)), stackWidget, SLOT(setCurrentIndex(int)));
}

//------------------------------------------------------------------------------
FontSettingsDialog::~FontSettingsDialog()
{
    // nothing todo
}

//------------------------------------------------------------------------------
bool FontSettingsDialog::showDialog(FontSettings *settings)
{
    setupFontSettingsDialog(settings);

    if (exec() != Accepted)
        return false;

    updateFontSettings(settings);
    return true;
}

//------------------------------------------------------------------------------
void FontSettingsDialog::updateFontSettings(FontSettings *settings)
{
    settings -> useWindowFont = m_windowFontPanel -> isChecked();
    settings -> useBrowserFont = m_browserFontPanel -> isChecked();

    settings -> windowFont = settings -> useWindowFont ? m_windowFontPanel -> selectedFont() : qApp -> font();
    settings -> browserFont = settings -> useBrowserFont ? m_browserFontPanel -> selectedFont() : qApp -> font();

    settings -> windowWritingSystem = settings -> useWindowFont ? m_windowFontPanel -> writingSystem() : QFontDatabase::Latin;
    settings -> browserWritingSystem = settings -> useBrowserFont ? m_browserFontPanel -> writingSystem() : QFontDatabase::Latin;
}

//------------------------------------------------------------------------------
void FontSettingsDialog::setupFontSettingsDialog(const FontSettings *settings)
{
    m_windowFontPanel -> setSelectedFont(settings -> windowFont);
    m_browserFontPanel -> setSelectedFont(settings -> browserFont);

    m_windowFontPanel -> setWritingSystem(settings -> windowWritingSystem);
    m_browserFontPanel -> setWritingSystem(settings -> browserWritingSystem);

    m_windowFontPanel -> setChecked(settings -> useWindowFont);
    m_browserFontPanel -> setChecked(settings -> useBrowserFont);
}
