#include "previewmodule.h"
#include "ui_previewmodule.h"

PreviewModule::PreviewModule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreviewModule)
{
    ui->setupUi(this);
}

PreviewModule::~PreviewModule()
{
    delete ui;
}
