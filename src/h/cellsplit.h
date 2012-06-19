#ifndef __CELLSPLIT_H__
#define __CELLSPLIT_H__

#include "ui_cellsplit.h"

//===================== class DialogCellSplit ===========================
class DialogCellSplit : public QDialog
{
     Q_OBJECT

public:
	DialogCellSplit(QWidget *parent = 0);

signals:
	void applySplit(int rows, int columns);

private slots:
	void accept();
	void reject();

private:
	Ui::DialogCellSplit ui;

};

#endif // __CELLSPLIT_H__
