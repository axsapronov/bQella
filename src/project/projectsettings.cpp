#include "projectsettings.h"
#include "ui_projectsettings.h"

ProjectSettings::ProjectSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProjectSettings)
{
    ui->setupUi(this);
}

ProjectSettings::~ProjectSettings()
{
    delete ui;
}
