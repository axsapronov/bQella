/****************************************************************************
**
** Copyright (C) 2007-2009 Pavel Malakhov
**
** This file is part of Research Assistant project
** https://sourceforge.net/projects/rassistant
**
** The code is under GNU General Public License versions 2.0 or 3.0
****************************************************************************/
#include <QFile>
#include <QMessageBox>

#include "statistics.h"
#include "config.h"
#include "pcommon.h"
#include "filecommon.h"

enum {LinkRole   = Qt::UserRole + 1000}; 	// reference = file name = link to source

//==================== class ProjectStatistics ====================
ProjectStatistics::ProjectStatistics(QWidget *parent)
	 : QDialog(parent)
{
	 ui.setupUi(this);
	 connect(ui.PBGetStatistics, SIGNAL(clicked()), this, SLOT(getStatistics()));
     connect(ui.TBFileCopy, SIGNAL(clicked()), this, SLOT(copyImages()));
     connect(ui.TBFileDelete, SIGNAL(clicked()), this, SLOT(fileDelete()));
     connect(ui.CBType, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeList(QString)));
     connect(ui.CBLists, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeList(QString)));
     connect(ui.TWFiles, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(TWFilesCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
     connect(ui.PBOK, SIGNAL(clicked()), this, SLOT(accept()));

     ui.PrgBarCopyImages->setMaximum(ui.CBType->count() * ui.CBLists->count());
     ToolBarEnabled(false);
     ui.PrgBarCopyImages->setVisible(false);

     showImageControl(false);

     //set unvisible for release version, since they does not work
     ui.TBFileCopy->setVisible(false);
     ui.TBEditFileName->setVisible(false);
 }

//-------------------------------------------------
void ProjectStatistics::accept()
{
        QWidget::hide();  //close dialog
}

//-------------------------------------------------
void ProjectStatistics::getStatistics()
{
    ui.PrgBarCopyImages->setValue(0);
    ui.PrgBarCopyImages->setVisible(true);
    //ui.TWFiles->clear();
    ui.TEStatistics->clear();
    ToolBarEnabled(false);

    ui.TEStatistics->insertHtml("<p align=\"center\"><h1>" +Config::configuration()->CurPrjName()+ "</h1></p>" +
                "<p align=\"center\">" + tr("(Statistics for the project)") + "</p><hr />"+
                "<p></p>");

    emit getAllStat();      //call HelpDialog::getAllStat(), connect in MainWindow::MainWindow()
    changeList(QString());  //apply new list
    ui.TEStatistics->append("<hr>");
    ui.PrgBarCopyImages->setVisible(false);
}

//-------------------------------------------------
void ProjectStatistics::setStatCountImages(int total, int unique, int external)
{
	ui.TEStatistics->append(tr("Images in documents:")+ QString("<strong> %1</strong>").arg(total));
	ui.TEStatistics->append(tr("- unique:")+ QString("<strong> %1</strong>").arg(unique));
	ui.TEStatistics->append(tr("- external:")+ QString("<strong> %1</strong>").arg(external));
    ui.TBFileCopy->setEnabled(external > 0);
}

//-------------------------------------------------
void ProjectStatistics::setStatImageLists(QStringList total, QStringList unique, QStringList external)
{
    /*int n = total.count();
    ui.TEStatistics->append("");
        ui.TEStatistics->insertHtml(QString("<p><span style=\" text-decoration: underline;\">%1</span></p><p></p>")
                                                                .arg(tr("Total list of images [%1]:").arg(n)));
        for (int i=0; i<n; ++i)
                ui.TEStatistics->append(total[i]);*/

    int n = unique.count();
    if (n>0){
        ui.TEStatistics->append("");
        ui.TEStatistics->insertHtml(QString("<p><span style=\" text-decoration: underline;\">%1</span></p><p></p>")
                                                                .arg(tr("List of unique images [%1]:").arg(n)));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        for (int i=0; i<n; ++i)
                ui.TEStatistics->append( tr(qPrintable(unique[i])) );
    }

    QDir dirPrjImg(Config::configuration()->CurPrjImgDir());
    QStringList imgFNList = dirPrjImg.entryList(QDir::Files);
    QStringList imgUnused;
    n = imgFNList.count();
    for (int i=0; i<n; ++i)
        if (unique.indexOf(imgFNList[i]) < 0)
            imgUnused.append(imgFNList[i]);
    if (imgUnused.count() > 0)
        ui.TEStatistics->append(tr("Unused images:")+ QString("<strong> %1</strong>").arg(imgUnused.count()));

    n = external.count();
   if (n>0){
        ui.TEStatistics->append("");
        ui.TEStatistics->insertHtml(QString("<p><span style=\" text-decoration: underline;\">%1</span></p><p></p>")
                                                                .arg(tr("List of external images [%1]:").arg(n)));
        for (int i=0; i<n; ++i)
                ui.TEStatistics->append( tr(qPrintable(external[i])) );
    }
}

//-------------------------------------------------
void ProjectStatistics::setStatDocLists(QStringList unused, QStringList unique, QStringList external)
{
    ui.TEStatistics->append(tr("Documents in use:")+ QString("<strong> %1</strong>").arg(unique.count()));
    int n = unused.count();
    if (n>0){
        ui.TEStatistics->append("");
        ui.TEStatistics->insertHtml(QString("<p><span style=\" text-decoration: underline;\">%1</span></p><p></p>")
                                                                .arg(tr("List of unused documents [%1]:").arg(n)));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        for (int i=0; i<n; ++i)
                ui.TEStatistics->append( tr(qPrintable(unused[i])) );
    }
}

//-------------------------------------------------
void ProjectStatistics::copyImages()
{
    emit copyAllImages(); //!+! copy selected image and update links in all documents
    ui.TBFileCopy->setEnabled(false);
}

//-------------------------------------------------
void ProjectStatistics::setTW(QTreeWidget *tw, QStringList values, QString path)
{
    QString link;
    QFileInfo fi;
    tw->clear();
    for (int i=0; i<values.count(); ++i){
        if (path.isEmpty()) link = values[i];
        else                link = absolutifyFileName(values[i], path);
        fi.setFile(link);
        QTreeWidgetItem *item = new QTreeWidgetItem(tw);
        item->setText(0, fi.fileName());
        item->setData(0, LinkRole, link);
    }
    //qDebug() << "-- setTW: values=" << values.count() << ", path="<< path;
}

//-------------------------------------------------
void ProjectStatistics::setList(QString listName, QStringList list)
{
    QString t = listName.toLower();
    if ( t == "doclisttotal") {
        setTW(&twDocListTotal, list, Config::configuration()->CurPrjDir());
        ui.TEStatistics->append("<p align=\"center\"><h3>" + tr("Documents") + "</h3></p>");
        ui.TEStatistics->append(tr("In the project directory:")+
                                QString("<strong> %1</strong>").arg(list.count()));
    }
    if ( t == "doclistexternal") {
        setTW(&twDocListExternal, list, "");
        ui.TEStatistics->append(tr("Out of the project directory:")+
                                QString("<strong> %1</strong>").arg(list.count()));
    }
    if ( t == "doclistunique") {
        setTW(&twDocListUnique, list, Config::configuration()->CurPrjDir());
        ui.TEStatistics->append(tr("- unique:")+ QString("<strong> %1</strong>").arg(list.count()));
    }
    if ( t == "doclistunused") {
        setTW(&twDocListUnused, list, Config::configuration()->CurPrjDir());
        ui.TEStatistics->append(tr("- unused:")+ QString("<strong> %1</strong>").arg(list.count()));
    }

    if ( t == "imagelisttotal") {
        setTW(&twImageListTotal, list, Config::configuration()->CurPrjImgDir());
        ui.TEStatistics->append("<p align=\"center\"><h3>" + tr("Images") + "</h3></p>");
        ui.TEStatistics->append("<p></p>" + tr("Images in project directory:")+
                                QString("<strong> %1</strong>").arg(list.count()));
    }
    if ( t == "imagelistunique") {
        setTW(&twImageListUnique, list, Config::configuration()->CurPrjImgDir());
        ui.TEStatistics->append(tr("- unique:")+ QString("<strong> %1</strong>").arg(list.count()));
    }
    if ( t == "imagelistexternal") {
        setTW(&twImageListExternal, list, "");
        ui.TEStatistics->append(tr("- external:")+ QString("<strong> %1</strong>").arg(list.count()));
    }
    if ( t == "imagelistunused") {
        setTW(&twImageListUnused, list, Config::configuration()->CurPrjImgDir());
        ui.TEStatistics->append(tr("- unused:")+ QString("<strong> %1</strong>").arg(list.count()));
    }

    ui.PrgBarCopyImages->setValue(ui.PrgBarCopyImages->value() + 1);
    //qDebug() << "-- setList: listName=" << listName << ", numbetr=" << list.count();
}
/*
//-------------------------------------------------
void ProjectStatistics::setList(QString listName, QTreeWidget tw)
{
    QString t = listName.toLower();
    if ( t == "imagelisttotal")     twImageListTotal = tw;
    if ( t == "imagelistunique")    twImageListUnique = tw;
    if ( t == "imagelistexternal")  twImageListExternal = tw;
    if ( t == "imagelistunused")    twImageListUnused = tw;
}
*/
//-------------------------------------------------
void ProjectStatistics::changeList(QString itemName)
{
    QString lt = ui.CBType->currentText().toLower();
    QString ln = ui.CBLists->currentText().toLower();
    ui.TBSample->clear();
    ui.LFileName->clear();
    ui.TWFiles->clear();
    ui.LItemCounter->setText("");
    //qDebug() << "-- changeList: listName=" << lt << ", listName=" << ln;
    if (lt == tr("Documents").toLower()){
        //qDebug() << "-- - there is a document, total=" << twDocListTotal.topLevelItemCount() << ", unique=" << twDocListUnique.topLevelItemCount();
        if (ln == tr("Total").toLower())    setTWFiles(&twDocListTotal);
        if (ln == tr("Unique").toLower())   setTWFiles(&twDocListUnique);
        if (ln == tr("External").toLower()) setTWFiles(&twDocListExternal);
        if (ln == tr("Unused").toLower())   setTWFiles(&twDocListUnused);
        //if (ln == tr("Fake").toLower())   setTWFiles(&twDocListFake);
    }
    if (lt == tr("Images").toLower()){
        //qDebug() << "-- - there is an image";
        if (ln == tr("Total").toLower())    setTWFiles(&twImageListTotal);
        if (ln == tr("Unique").toLower())   setTWFiles(&twImageListUnique);
        if (ln == tr("External").toLower()) setTWFiles(&twImageListExternal);
        if (ln == tr("Unused").toLower())   setTWFiles(&twImageListUnused);
        //if (ln == tr("Fake").toLower())   setTWFiles(&twImageListFake);
    }
    int n = ui.TWFiles->topLevelItemCount();
    ui.LItemCounter->setText(QString::number(n));
    ToolBarEnabled(n > 0);
}

//-------------------------------------------------
// copy QTreeWidget object, since '=' operator does not work
void ProjectStatistics::setTWFiles(QTreeWidget *tw)
{
    ui.TWFiles->clear();
    for (int i=0; i < tw->topLevelItemCount(); ++i){
        ui.TWFiles->addTopLevelItem( tw->topLevelItem(i)->clone() );
    }
}

//-------------------------------------------------
void ProjectStatistics::TWFilesCurrentItemChanged(QTreeWidgetItem *curItem ,QTreeWidgetItem *prevItem)
{
    qDebug() << "preview resource";
    QString html;
    QString type = ui.CBType->currentText().toLower();
    QString fn;
    if (!curItem)
        return;

    fn = curItem->data(0,LinkRole).toString();
    qDebug() << "type: " << type.toAscii() << ", file name: " << fn;
    QFile f(fn);
    if (f.open(QFile::ReadOnly)) {
        qDebug() << "resource file exists";
        if (type.toLower() == tr("Documents").toLower()){
            fn = absolutifyFileName(fn, Config::configuration()->CurPrjDir());
            ui.TBSample->setSource( urlifyFileName(fn) );
            showImageControl(false);
        }
        if (type.toLower() == tr("Images").toLower()){
            fn = absolutifyFileName(fn, Config::configuration()->CurPrjImgDir());
            html = "<img  src=\"" + fn + "\">";
            qDebug() << "img html: " << html;
            ui.TBSample->setHtml(html);
            //showImageControl(true);
        }
        ui.LFileName->setText(fn);
    }else{
        ui.LFileName->setText(tr("Cannot open resource:\n%1").arg(fn));
        ui.TBSample->clear();
        qWarning("Cannot open resource '%s' for reading", qPrintable(fn));
    }

}

//-------------------------------------------------
void ProjectStatistics::ToolBarEnabled(bool b)
{
    ui.TBEditFileName->setEnabled(b);
    ui.TBFileCopy->setEnabled(b);
    ui.TBFileDelete->setEnabled(b);
}

//-------------------------------------------------
void ProjectStatistics::showImageControl(bool b)
{
    ui.HSImgSize->setVisible(b);
    ui.LEImgWidth->setVisible(b);
    ui.LEImgHeight->setVisible(b);
    ui.CBImgShrink->setVisible(b);
}

//-------------------------------------------------
void ProjectStatistics::Clear()
{
    twDocListTotal.clear();
    twDocListUnique.clear();
    twDocListExternal.clear();
    twDocListUnused.clear();

    twImageListTotal.clear();
    twImageListUnique.clear();
    twImageListExternal.clear();
    twImageListUnused.clear();

    ui.TBSample->clear();
    ui.TWFiles->clear();
    ui.LFileName->clear();
    ToolBarEnabled(false);
    showImageControl(false);
}

//-------------------------------------------------
void ProjectStatistics::fileDelete()
{
    QString fn = ui.LFileName->text();
    if (QFile::exists(fn)){
        int ret = QMessageBox::warning(this, "Research Assistant", tr("Delete file from disk?"),
                                        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
        if (ret == QMessageBox::Yes){
            QFile::remove(fn);
            //removeItem();
        }
    }
}

