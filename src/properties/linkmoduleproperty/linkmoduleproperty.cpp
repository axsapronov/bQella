#include "linkmoduleproperty.h"
#include "ui_linkmoduleproperty.h"


#include "config.h"
#include <QFileDialog>

LinkModuleProperties::LinkModuleProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinkModuleProperties)
{
    ui->setupUi(this);
    connect(ui->BRemove, SIGNAL(clicked()), this, SLOT(RemoveLink()) );
    connect(ui->BChooseFile, SIGNAL(clicked()), this, SLOT(chooseFile()) );

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));
}
///-----------------------------------------------------------------------------
LinkModuleProperties::~LinkModuleProperties()
{
    delete ui;
}
///-----------------------------------------------------------------------------
void LinkModuleProperties::RemoveLink()
{
    emit removeLink();
    QWidget::hide();  //close dialog
}
///-----------------------------------------------------------------------------
void LinkModuleProperties::chooseFile()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Choose file to link to"),
                                              Config::configuration() -> CurPrjDir(), tr("HTML (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty()){
        ui->ELinkLocation -> setText(fn);
    }
}
///-----------------------------------------------------------------------------
void LinkModuleProperties::setProperties(QString lText, QString lLocation)
{
    linkText = lText;
    linkLocation = lLocation;

    ui->ELinkText -> setText(lText);
    ui->ELinkLocation -> setText(lLocation);
}
///-----------------------------------------------------------------------------
void LinkModuleProperties::reject()
{
    validProperties = false;
    QWidget::hide();  //close dialog
}
///-----------------------------------------------------------------------------
void LinkModuleProperties::accept()
{
    linkText = ui->ELinkText -> text();
    linkLocation = ui->ELinkLocation -> text();
    validProperties = true;
    if ( ui->ELinkLocation -> text().isEmpty() )
        emit removeLink();
    else
        emit updateLink(linkText, linkLocation);
    QWidget::hide();  //close dialog
}
///-----------------------------------------------------------------------------

///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
///-----------------------------------------------------------------------------
