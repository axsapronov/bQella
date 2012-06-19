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
#include <QtGui>
#include <QList>
#include <QMessageBox>

#include "projectsources.h"
#include "config.h"
#include "pcommon.h"

//====================== class ProjectSources ============================
ProjectSources::ProjectSources(QWidget *parent)
     : QDialog(parent)
{
    ui.setupUi(this);
	setSignFormat();
	// hide source table details since changes in CBAuthorShort and CBAuthor comboboxes 
	//update 'source' table with varchar but not int values => changed line hides
	// !+! comment this line when fix the bug
	ui.GBSourceDetails->setVisible(false);

    connect( ui.PBOK, SIGNAL(clicked()), this, SLOT(saveSources()) );
    connect( ui.PBClose, SIGNAL(clicked()), this, SLOT(closeDialog()) );

	//tab Table.Sources
	connect( ui.PBSrcNew, SIGNAL(clicked()), this, SLOT(srcRecordNew()) );
	connect( ui.PBSrcDel, SIGNAL(clicked()), this, SLOT(srcRecordDel()) );
	connect( ui.PBSetSignString, SIGNAL(clicked()), this, SLOT(setSignString()) );
	//connect( ui.PBSrcLoad, SIGNAL(clicked()), this, SLOT(loadSources()) );
	ui.PBSrcLoad->setVisible(false);	
	//tab Table.Links
	connect( ui.PBLnkNew, SIGNAL(clicked()), this, SLOT(lnkRecordNew()) );
	connect( ui.PBLnkDel, SIGNAL(clicked()), this, SLOT(lnkRecordDel()) );
	connect( ui.PBLnkAll, SIGNAL(clicked()), this, SLOT(lnkShowAllRecords()) );
	//tab Table.Authors
	connect( ui.PBAuthorNew, SIGNAL(clicked()), this, SLOT(authorRecordNew()) );
	connect( ui.PBAuthorDel, SIGNAL(clicked()), this, SLOT(authorRecordDel()) );
	
	//tab Format
	connect( ui.PBTest, SIGNAL(clicked()), this, SLOT(testFormat()) );	
	connect( ui.PTESignCode, SIGNAL(textChanged()), this, SLOT(setSignFormat()) );
	connect( ui.SBLinkID, SIGNAL(valueChanged(int)), this, SLOT(setExampleSign(int)) );
	connect( ui.PBSetSignStringFromExample, SIGNAL(clicked()), this, SLOT(setSignStringFromExample()) );
	
}

//-------------------------------------------------
void ProjectSources::closeDialog()
{
	QWidget::hide();  //close dialog
}

//-------------------------------------------------
void ProjectSources::saveSources()
{
	// for debug, on manual commit
	//saveTable(authorModel);
	//saveTable(srcModel);
	//saveTable(lnkModel);
	emit updateSigns();
	QWidget::hide();  //close dialog
}
/*
//-------------------------------------------------
// for debug, on manual commit
void ProjectSources::saveTable(QSqlTableModel *model)
{
	 model->database().transaction();
     if (model->submitAll()) {
         model->database().commit();
     } else {
         model->database().rollback();
         QMessageBox::warning(this, tr("Error while saving database"), tr("The database reported an error: %1").arg(model->lastError().text()));
     }

}

//-------------------------------------------------
void ProjectSources::ApplyDbChanges()
{
	saveTable(authorModel);
	saveTable(srcModel);
	saveTable(lnkModel);
}
*/
//-------------------------------------------------
void ProjectSources::setSignString()
{
	int linkId = curLnkId().toInt();
	if (linkId < 1 )
		linkId = lnkModel->record(0).value("id").toInt();
	setSignString(linkId);
}

//-------------------------------------------------
void ProjectSources::setSignStringFromExample()
{
	setSignString(ui.SBLinkID->value());
}

//-------------------------------------------------
void ProjectSources::setSignString(int id)  
{
	Config::configuration()->setDefaultSignatureID(id);
	ui.SBLinkID->setValue(id); 
	signString = getSignString(id);
	ui.LDefaultSignString->setText(signString);
	Config::configuration()->toPrjLog(3, tr("Set sign string to: id=%1 sign=%2", "For log").arg(id).arg(signString));
}

//-------------------------------------------------
void ProjectSources::testFormat()
{
	setSignString(ui.SBLinkID->value());
	ui.LSignExample->setText(getSignString(ui.SBLinkID->value()));
	ui.PBSetSignStringFromExample->setEnabled(true);
}

//-------------------------------------------------
void ProjectSources::setExampleSign(int id)  
{
	ui.LSignExample->setText(getSignString(id));
	if (Config::configuration()->DefaultSignatureID() != ui.SBLinkID->value())
		ui.PBSetSignStringFromExample->setEnabled(true);	
}

//-------------------------------------------------
QString ProjectSources::getSignString(int id) 
{
	QString strSign = "";
	QSqlQuery qSrc, qLnk, qAuthor;
	
	QString strQ = "SELECT id_source,link,comment FROM links WHERE id='" +QString::number(id)+ "'";
	Config::configuration()->toPrjLog(3,"sql = "+strQ);
    if (!qLnk.exec(strQ)){
    	Config::configuration()->toPrjLog(3,tr("Failed to execute query: %1", "For log").arg(strQ));
    	return strSign;
   	}
   	if (!qLnk.next()) return strSign;
	strQ = "SELECT title,author,title_short,title_original,title_original_short,comment FROM sources WHERE id='" +qLnk.value(0).toString()+ "'";
	Config::configuration()->toPrjLog(3,"sql = "+strQ);
    if (!qSrc.exec(strQ)){
    	Config::configuration()->toPrjLog(3,tr("Failed to execute query: %1", "For log").arg(strQ));
    	return strSign;
   	}
   	if (!qSrc.next()) return strSign;
	strQ = "SELECT name,name_short,comment FROM authors WHERE id='" +qSrc.value(1).toString()+ "'";
	Config::configuration()->toPrjLog(3,"sql = "+strQ);
    if (!qAuthor.exec(strQ)){
    	Config::configuration()->toPrjLog(3,tr("Failed to execute query: %1", "For log").arg(strQ));
    	return strSign;
   	}
   	if (!qAuthor.next()) return strSign;
   	
	//Format signature 
	QString strFormat = ui.PTESignCode->toPlainText();
	QString str, letter;
	for (int i = 0; i < strFormat.size(); i++){
		if (strFormat[i] != '%') //place all text in signature exept variables, which start with char '%'
			strSign = strSign + strFormat[i];
		else{ //get variable value
			i++; 
			str = "";
			while (strFormat[i].isDigit()) {
				str = str + strFormat[i];
				i++;
			} 
			i--; //since cycle 'for' will increase i again
			int n = str.toInt();
			switch (n){ 
				case 1:		str = qSrc.value(0).toString();		break;
				case 2:		str = qSrc.value(2).toString();		break;
				case 3:		str = qSrc.value(3).toString();		break;
				case 4:		str = qSrc.value(4).toString();		break;
				case 9:		str = qSrc.value(5).toString();		break;
				case 11:	str = qAuthor.value(0).toString();	break;
				case 12:	str = qAuthor.value(1).toString();	break;
				case 19:	str = qAuthor.value(2).toString();	break;
				case 21:	str = qLnk.value(1).toString();		break;
				case 29:	str = qLnk.value(2).toString();		break;
				default:	str = "";	break;
			}
			strSign = strSign + str; 
		} //finish processing variable
	} //finish processing format string
	// !+! add signature to text with comment to identify it in future use.
	// somehow str2 = str + str; does not work, so we forced to use str2.append()
	//QString str2 = "<!-- SIGNATURE_START SignID="; str2.append(QString::number(lnkIndex)); 
	//str2.append(" -->\n"); str2.append(strSign); str2.append("\n<!-- SIGNATURE_END -->\n");
	return strSign;
}//getSignString

//-------------------------------------------------
void ProjectSources::setSignList()
{
	SignList.clear();
	SignList.append(getSignString(Config::configuration()->DefaultSignatureID())); //place default singature at the top of the list
	QSqlQuery q("SELECT id FROM links");
	while (q.next()){
		int id = q.value(0).toInt();
		QString signature = getSignString(id);
		if (!signature.isEmpty())
			SignList.append(signature);
	}
}

//-------------------------------------------------
void ProjectSources::setSignFormat()
{
	ui.LSignFormat->setText(ui.PTESignCode->toPlainText());	
}

//-------------------------------------------------
void ProjectSources::loadSources()
{
    if (!QSqlDatabase::drivers().contains("QSQLITE")){
        QMessageBox::critical(this, tr("Unable to load database"), tr("Could not find SQLITE driver! The database of sources will be disabled!"));
        return;
    }

    // initialize the database
    QSqlError err = initDb();
    if (err.type() != QSqlError::NoError) {
         QMessageBox::critical(this, tr("Unable to initialize Database"), tr("Error initializing database: %1").arg(err.text()));
        return;
    }
    
    // ======= Create the data model for Authors =======
    authorModel = new QSqlTableModel(ui.TVAuthors);
    authorModel->setEditStrategy(QSqlTableModel::OnFieldChange);	//OnManualSubmit - for debug, on manual changes commit
    authorModel->setTable("authors");
    // Set the localized header captions    
    authorModel->setHeaderData(authorModel->fieldIndex("name"), Qt::Horizontal, tr("Name"));
    authorModel->setHeaderData(authorModel->fieldIndex("name_short"), Qt::Horizontal, tr("Short name"));
    // Populate the authorModel
    if (!authorModel->select()) {
        QMessageBox::critical(this, tr("Unable to use Database"), tr("Error selecting from database: %1").arg(authorModel->lastError().text()));
        return;
    }
    // Set the authorModel and hide some columns
    ui.TVAuthors->setModel(authorModel);
    ui.TVAuthors->setColumnHidden(authorModel->fieldIndex("id"), true);
    ui.TVAuthors->setColumnHidden(authorModel->fieldIndex("comment"), true);
    ui.TVAuthors->resizeRowsToContents();
    ui.TVAuthors->resizeColumnsToContents();
    
	// Connect widgets with database (map widgets to model's fields)  
    QDataWidgetMapper *authorMapper = new QDataWidgetMapper(this);
    authorMapper->setModel(authorModel);
    authorMapper->addMapping(ui.TEAuthorComment, authorModel->fieldIndex("comment"));
    connect(ui.TVAuthors->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), authorMapper, SLOT(setCurrentModelIndex(QModelIndex)));
    ui.TVAuthors->setCurrentIndex(authorModel->index(0, 0));
    
    // ======= Create the data model for Sources =======
    srcModel = new QSqlRelationalTableModel(ui.TVSources);
    srcModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    srcModel->setTable("sources");

    // Remeber the indexes of the columns
    int authorIdx = srcModel->fieldIndex("author");
    /*int author_shortIdx = srcModel->fieldIndex("author_short");*/

    // Set the relations to the other database tables
    srcModel->setRelation(authorIdx, QSqlRelation("authors", "id", "name"));
    //srcModel->setRelation(srcModel->fieldIndex("author_short"), QSqlRelation("authors", "id", "name_short"));

    // Set the localized header captions    
    srcModel->setHeaderData(authorIdx, Qt::Horizontal, tr("Author name"));
    srcModel->setHeaderData(srcModel->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    srcModel->setHeaderData(srcModel->fieldIndex("title_short"), Qt::Horizontal, tr("Short title"));
    srcModel->setHeaderData(srcModel->fieldIndex("title_original"), Qt::Horizontal, tr("Original title"));
	srcModel->setHeaderData(srcModel->fieldIndex("title_original_short"), Qt::Horizontal, tr("Short original title"));

    // Populate the srcModel
    if (!srcModel->select()) {
        QMessageBox::critical(this, tr("Unable to use Database"), tr("Error selecting from database: %1").arg(srcModel->lastError().text()));
        return;
    }
    // Set the srcModel and hide some columns
    ui.TVSources->setModel(srcModel);
    ui.TVSources->setItemDelegate(new QSqlRelationalDelegate(ui.TVSources));
    ui.TVSources->setColumnHidden(srcModel->fieldIndex("id"), true);
    ui.TVSources->setColumnHidden(srcModel->fieldIndex("comment"), true);
    ui.TVSources->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.TVSources->resizeRowsToContents();
    ui.TVSources->resizeColumnsToContents();
    
    // Initialize the Author combo box
    ui.CBAuthor->setModel(srcModel->relationModel(authorIdx));
    ui.CBAuthor->setModelColumn(srcModel->relationModel(authorIdx)->fieldIndex("name"));
    ui.CBAuthorShort->setModel(srcModel->relationModel(authorIdx));
    ui.CBAuthorShort->setModelColumn(srcModel->relationModel(authorIdx)->fieldIndex("name_short"));
    
	// Connect widgets with database (map widgets to model's fields)
    QDataWidgetMapper *srcMapper = new QDataWidgetMapper(this);
    srcMapper->setModel(srcModel);    
    srcMapper->addMapping(ui.LETitle, 				srcModel->fieldIndex("title"));
    srcMapper->addMapping(ui.LETitleShort, 			srcModel->fieldIndex("title_short"));
    srcMapper->addMapping(ui.LETitleOriginal, 		srcModel->fieldIndex("title_original"));
    srcMapper->addMapping(ui.LETitleOriginalShort, 	srcModel->fieldIndex("title_original_short"));
    srcMapper->addMapping(ui.CBAuthor, 				authorIdx); //!+! updates table with name(!) not id value, 
    srcMapper->addMapping(ui.CBAuthorShort,			authorIdx); //!+! and does not react on currentRowChanged()
    srcMapper->addMapping(ui.TESrcComment, 			srcModel->fieldIndex("comment"));
    
    connect(ui.TVSources->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), srcMapper, SLOT(setCurrentModelIndex(QModelIndex)));
    ui.TVSources->setCurrentIndex(srcModel->index(0, 0));
    
    // ======= Create the data model for Links =======
    lnkModel = new QSqlTableModel(ui.TVLinks);
    lnkModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    lnkModel->setTable("links");
    lnkModel->setFilter("id_source='" +curSrcId()+ "'");
    // Set the localized header captions    
    lnkModel->setHeaderData(lnkModel->fieldIndex("link"), Qt::Horizontal, tr("Link"));
    // Populate the lnkModel
    if (!lnkModel->select()) {
        QMessageBox::critical(this, tr("Unable to use Database"), tr("Error selecting from database: %1").arg(lnkModel->lastError().text()));
        return;
    }
    // Set the lnkModel and hide some columns
    ui.TVLinks->setModel(lnkModel);
    ui.TVLinks->setColumnHidden(lnkModel->fieldIndex("id"), true);
    ui.TVLinks->setColumnHidden(lnkModel->fieldIndex("id_source"), true);
    ui.TVLinks->setColumnHidden(lnkModel->fieldIndex("comment"), true);
    ui.TVLinks->resizeRowsToContents();
    ui.TVLinks->resizeColumnsToContents();
    
	// Connect widgets with database (map widgets to model's fields)  
    QDataWidgetMapper *lnkMapper = new QDataWidgetMapper(this);
    lnkMapper->setModel(lnkModel);
    lnkMapper->addMapping(ui.TELnkComment, lnkModel->fieldIndex("comment"));
    connect(ui.TVLinks->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), lnkMapper, SLOT(setCurrentModelIndex(QModelIndex)));
    connect(ui.TVSources->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(refreshLnkModel()));
    ui.TVLinks->setCurrentIndex(lnkModel->index(0, 0));
	//All tables are loaded now and models are set.
	
    // ======= Other settings =======
    // Set maximum to test signature spin box    
    QSqlQuery query("SELECT MAX(id) FROM links");
    int maxLinkId = 1;
    if (query.next())
        maxLinkId = query.value(0).toInt();
    ui.SBLinkID->setMaximum(maxLinkId);
    // Set default signature
    setSignString( Config::configuration()->DefaultSignatureID() );
    setSignList();
}
   
//-------------------------------------------------
QSqlError ProjectSources::initDb()
{
    // check if there is a database. Check for one table is enough.
    // !+! check every field in every table for future versions to upgrade database structure
	if (!QFile::exists( Config::configuration()->DbName() ))
		newDb();
		
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName( Config::configuration()->DbName() );
    if (!db.open()){ 
        return db.lastError();
    }
    return QSqlError();
}

//-------------------------------------------------
void ProjectSources::closeDb()
{
	db.close();	
}

//-------------------------------------------------
QSqlError ProjectSources::newDb()
{
	return newDb( Config::configuration()->DbName() );	
}

//-------------------------------------------------
QSqlError ProjectSources::newDb(QString dbFileName)
{
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName( dbFileName );
    if (!db.open())
        return db.lastError();
	Config::configuration()->toPrjLog(3, tr("Create a new database: %1", "For log").arg(dbFileName));	
	
    // do not create tables if they are exist
    QStringList tables = db.tables();
    if ( tables.contains("sources", Qt::CaseInsensitive) &&
         tables.contains("authors", Qt::CaseInsensitive) &&
         tables.contains("links",   Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(QLatin1String("CREATE TABLE sources(id integer primary key, \
    		title varchar, \
	   		author integer, \
			title_short varchar, \
			title_original varchar, \
			title_original_short varchar, \
			comment varchar)")))
	   	return q.lastError();
    if (!q.exec(QLatin1String("CREATE TABLE authors(id integer primary key, \
	   		name varchar, \
	   		name_short varchar, \
	   		comment varchar)")))	
	   	return q.lastError();
    if (!q.exec(QLatin1String("CREATE TABLE links(id integer primary key, \
			id_source integer, \
    		link varchar, \
    		comment varchar)")))
        return q.lastError();
    authorRecordNew();
    srcRecordNew();
    db.close();
    return QSqlError();
}

//-------------------------------------------------
QString ProjectSources::dbAddSource(QSqlQuery &q, const QString &title, const QString &authorId, const QString &title_short, 
			const QString &title_original, const QString &title_original_short, const QString &comment)
{
    q.addBindValue(title);
    q.addBindValue(authorId);
    q.addBindValue(title_short);
    q.addBindValue(title_original);
    q.addBindValue(title_original_short);
    q.addBindValue(comment);
    q.exec();
    QVariant id = q.lastInsertId();
    return id.toString();
}

//-------------------------------------------------
int ProjectSources::dbAddLink(QSqlQuery &q, const QString &id_source, const QString &link, const QString &comment)
{
    q.addBindValue(id_source);
    q.addBindValue(comment);
    q.addBindValue(link);
    q.exec();
    QVariant id = q.lastInsertId();
    return id.toInt();
}

//-------------------------------------------------
void ProjectSources::dbAddAuthor(QSqlQuery &q, const QString &name, const QString &name_short, const QString &comment)
{
    q.addBindValue(name);
    q.addBindValue(name_short);
    q.addBindValue(comment);
    q.exec();
}

//-------------------------------------------------
void ProjectSources::srcRecordNew()
{
    QSqlQuery q;
	if (!q.prepare(QLatin1String("INSERT INTO sources(title,author,title_short,title_original,title_original_short,comment) VALUES(?,?,?,?,?,?)"))){
        QMessageBox::warning(this, tr("Failed to add a new record"), tr("The database reported an error: %1").arg(q.lastError().text()));
        return;
    }
    QString id = dbAddSource(q, "", "1", "", "", "", "");
    //srcModel->insertRow(row); //this works, but after refresh new record skiped (it exists in database though) возможно из-за поля author_short

	Config::configuration()->toPrjLog(3, tr("A new record added to table 'sources', id=%1", "For log").arg(id));	
	refreshSrcModel();  	
	// set focus back to QTableView to the last row
	int row = srcModel->rowCount();			 	// last record
	int col = srcModel->fieldIndex("title");	// 'title' field
	ui.TVSources->setFocus();
	ui.TVSources->scrollToBottom();
	ui.TVSources->setCurrentIndex( srcModel->index(row, col) );
    ui.TVSources->edit( srcModel->index(row, col) );	//!+! does not work
	//ui.TVSources->selectRow(row);
    lnkRecordNew(id);	
}

//-------------------------------------------------
void ProjectSources::srcRecordDel()
{
	//!+! ask if sure
	
	// remember current record number
	int row = ui.TVAuthors->currentIndex().row(); 	//current record
	int col = srcModel->fieldIndex("title");		//'title' field
	// delete record from 'sources' and according records from 'links'
	QSqlQuery q;
	q.exec("DELETE FROM links WHERE id_source='" + curSrcId() + "'");	
	q.exec("DELETE FROM sources WHERE id='" + curSrcId() + "'");	
	refreshSrcModel();	 
	// if it was last record, create an empty new one
	if (srcModel->rowCount() == 0){
		srcRecordNew();
		refreshSrcModel();	 
	}
	// set focus back to QTableView to the same row
	ui.TVSources->setFocus();
	ui.TVSources->setCurrentIndex( srcModel->index(row, col) ); //!+!   ( const QModelIndex & index )
	refreshLnkModel();
}

//-------------------------------------------------
void ProjectSources::lnkRecordNew()
{
	lnkRecordNew(curSrcId());
}

//-------------------------------------------------
void ProjectSources::lnkRecordNew(QString sourceID)
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("INSERT INTO links(id_source,link,comment) VALUES(?,?,?)"))){
        QMessageBox::warning(this, tr("Failed to add a new record"), tr("The database reported an error: %1").arg(q.lastError().text()));
        return;
    }
    int lnkId = dbAddLink(q, sourceID, "", "");
	refreshLnkModel();
	ui.TVLinks->setFocus();
	ui.SBLinkID->setMaximum(lnkId);
	Config::configuration()->toPrjLog(3, tr("New record added to table 'links'", "For log"));		 	
    setSignList();
}

//-------------------------------------------------
void ProjectSources::lnkRecordDel()
{
	QSqlQuery q;
	q.exec("DELETE FROM links WHERE id='" + curLnkId() + "'");
	// if it was last record, create an empty new one
	refreshLnkModel();
	if (lnkModel->rowCount() == 0){
		lnkRecordNew();
		refreshLnkModel();
	}
	ui.TVLinks->setFocus();
    setSignList();
}

//-------------------------------------------------
void ProjectSources::authorRecordNew()
{
    QSqlQuery q;
    if (!q.prepare(QLatin1String("INSERT INTO authors(name,name_short,comment) VALUES(?,?,?)"))){
        QMessageBox::warning(this, tr("Failed to add a new record"), tr("The database reported an error: %1").arg(q.lastError().text()));
        return;
    }
    dbAddAuthor(q, tr("<Not defined>"), "", "");
	refreshAuthorModel();
	ui.TVAuthors->setFocus();
	Config::configuration()->toPrjLog(3, tr("New record added to table 'authors'", "For log"));	
}

//-------------------------------------------------
void ProjectSources::authorRecordDel()
{
	QSqlQuery sql("DELETE FROM authors WHERE id='" + curAuthorId() + "'");
	refreshAuthorModel();
	if (authorModel->rowCount() == 0){ 
		authorRecordNew();
		refreshAuthorModel();
	}
	ui.TVAuthors->setFocus();
}

//-------------------------------------------------
void ProjectSources::refreshAuthorModel()
{
    authorModel->select(); 
    ui.TVAuthors->resizeRowsToContents();
    ui.TVAuthors->resizeColumnsToContents();	
    setSignList();
}

//-------------------------------------------------
void ProjectSources::refreshSrcModel()
{
    srcModel->select(); 
    ui.TVSources->resizeRowsToContents();
    ui.TVSources->resizeColumnsToContents();	
    setSignList();
}

//-------------------------------------------------
void ProjectSources::refreshLnkModel()
{
    lnkModel->setFilter("id_source='" +curSrcId()+ "'");
    lnkModel->select(); 
    ui.TVLinks->resizeRowsToContents();
    ui.TVLinks->resizeColumnsToContents();	
}

//-------------------------------------------------
void ProjectSources::lnkShowAllRecords()
{
    lnkModel->setFilter("");
    lnkModel->select(); 
    ui.TVLinks->resizeRowsToContents();
    ui.TVLinks->resizeColumnsToContents();	
	
}

//-------------------------------------------------
QString ProjectSources::curAuthorId()
{
	return authorModel->record( ui.TVAuthors->currentIndex().row() ).value("id").toString();
	//the same could be done with the following code:
	/*int row = ui.TVAuthors->currentIndex().row(); 	//current record
	int col = authorModel->fieldIndex("id");			//'id' field
	return authorModel->index(row, col).data().toInt();	*/
}

//-------------------------------------------------
QString ProjectSources::curSrcId()
{
	return srcModel->record( ui.TVSources->currentIndex().row() ).value("id").toString();
}

//-------------------------------------------------
QString ProjectSources::curLnkId()
{
	return lnkModel->record( ui.TVLinks->currentIndex().row() ).value("id").toString();
}
