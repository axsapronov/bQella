/****************************************************************************
**
** Copyright (C) 2007-2009 Pavel Malakhov
**
** This file is part of Research Assistant project
** https://sourceforge.net/projects/rassistant
**
** The code is under GNU General Public License versions 2.0 or 3.0
****************************************************************************/
#ifndef STATISTICS_H
#define STATISTICS_H

#include "ui_statistics.h"

//==================== class ProjectStatistics ====================
class ProjectStatistics : public QDialog
{
    Q_OBJECT

public:
    ProjectStatistics(QWidget *parent = 0);
    void Clear();

public slots:
    void setStatCountImages(int total, int unique, int external);
    void setStatImageLists(QStringList total, QStringList unique, QStringList external);
    void setStatDocLists(QStringList unused, QStringList unique, QStringList external);
    void setList(QString listName, QStringList list);
    //void setList(QString listName, QTreeWidget tw);

signals:
    //void statCountImages();
    //void updateContentFileList();
    void copyAllImages();
    void getAllStat();

private slots:
    void getStatistics();
    void accept();
    void copyImages();
    void changeList(QString itemName);
    void TWFilesCurrentItemChanged(QTreeWidgetItem *curItem ,QTreeWidgetItem *prevItem);
    void fileDelete();

private:
    void setTW(QTreeWidget * tw, QStringList values, QString path);
    void setTWFiles(QTreeWidget *tw);
    void ToolBarEnabled(bool b);
    void showImageControl(bool b);

    Ui::Statistics ui;
    QTreeWidget twDocListTotal, twDocListUnique, twDocListExternal, twDocListUnused;
    QTreeWidget twImageListTotal, twImageListUnique, twImageListExternal, twImageListUnused;
};

#endif // STATISTICS_H
