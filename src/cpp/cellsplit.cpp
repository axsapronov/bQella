#include "cellsplit.h"

//===================== class DialogCellSplit ===========================

DialogCellSplit::DialogCellSplit(QWidget *parent)
     : QDialog(parent)
{	
	ui.setupUi(this);
	connect(ui.PBOk, SIGNAL(clicked()), this, SLOT(accept()));	
	connect(ui.PBCancel, SIGNAL(clicked()), this, SLOT(reject()));	
}

//-------------------------------------------------
void DialogCellSplit::reject()
{
	QWidget::hide();  //close dialog
}

//-------------------------------------------------
void DialogCellSplit::accept()
{	
	emit applySplit(ui.SBRows -> value(), ui.SBColumns -> value());			
	QWidget::hide();  //close dialog
}
