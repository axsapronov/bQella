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
#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include "ui_settings.h"
#include "fontsettingsdialog.h"

class AppSettings : public QDialog
{
     Q_OBJECT

public:
	AppSettings(QWidget *parent = 0);
	
signals:
	void showContentsAV(bool b);
	void showContentsAVHeader(bool b);
	void updateApplicationFontSettings(FontSettings);

public slots:
	void accept();
	void reject();
	void apply();
	void set();
	
private:
	Ui::AppSettings ui; 
	
private slots:
	void chooseEditor();
	void chooseBrowser();
	void chooseArchiver();
	void chooseBackupDir();
	void showFontSettingsDialog();
};
#endif // __SETTINGS_H__