#include "helpbrowser.h"
#include "ui_helpbrowser.h"

HelpBrowser::HelpBrowser(QWidget *parent,const QString& strPath,
                         const QString& strFileName) :
    QDialog(parent),
    ui(new Ui::HelpBrowser)
{
    ui->setupUi(this);

    connect(ui -> pcmdBack, SIGNAL(clicked()), ui-> ptxtBrowser, SLOT(backward()));
    connect(ui -> pcmdHome, SIGNAL(clicked()), ui-> ptxtBrowser, SLOT(home()));
    connect(ui -> pcmdForward, SIGNAL(clicked()), ui -> ptxtBrowser, SLOT(forward()));
    connect(ui -> ptxtBrowser, SIGNAL(backwardAvailable(bool)), ui -> pcmdBack, SLOT(setEnabled(bool)));
    connect(ui -> ptxtBrowser, SIGNAL(forwardAvailable(bool)), ui -> pcmdForward, SLOT(setEnabled(bool)));

    ui -> ptxtBrowser->setSearchPaths(QStringList() << strPath);
    ui -> ptxtBrowser->setSource(QString(strFileName));
}
HelpBrowser::~HelpBrowser()
{
    delete ui;
}
