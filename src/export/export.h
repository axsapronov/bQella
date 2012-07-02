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
#ifndef EXPORT_H
#define EXPORT_H

#include <QObject>

class Export : public QObject
{
    Q_OBJECT
public:
    explicit Export(QObject *parent = 0);

public slots:
    void exportCreateDir(QString current_dir);
    void exportBibleqtIni(QString path, QString count);
    QString exportChapter (QString filename, QString count, bool chapt);
    void exportBibleqtIniInfo(QString filebibleqt, QString filename, QString count);
private slots:
    QString getNameFolder(QString cur_dir);
private:
    QString file;
};

#endif // EXPORT_H
