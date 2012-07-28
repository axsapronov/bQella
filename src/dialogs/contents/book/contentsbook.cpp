#include "contentsbook.h"
#include "ui_contentsbook.h"

ContentsBook::ContentsBook(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContentsBook)
{
    ui->setupUi(this);
}
//------------------------------------------
ContentsBook::~ContentsBook()
{
    delete ui;
}
//------------------------------------------
