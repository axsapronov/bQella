#include "rightpanel.h"
#include "ui_rightpanel.h"

#include "mainwindow.h"

RightPanel::RightPanel(QWidget *parent, MainWindow *h):
    QWidget(parent),
    ui(new Ui::RightPanel)
{
    ui->setupUi(this);
}

RightPanel::~RightPanel()
{
    delete ui;
}
