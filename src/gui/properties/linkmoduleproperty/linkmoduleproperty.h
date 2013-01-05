#ifndef LINKMODULEPROPERTY_H
#define LINKMODULEPROPERTY_H

#include <QDialog>

namespace Ui {
    class LinkModuleProperties;
}

class LinkModuleProperties : public QDialog
{
    Q_OBJECT

public:
    explicit LinkModuleProperties(QWidget *parent = 0);
    ~LinkModuleProperties();
    bool valid() { return validProperties; }
    QString LinkText() { return linkText; }
    QString LinkLocation() { return linkLocation; }

signals:
    void removeLink();
    void updateLink(QString lText, QString lLocation);

public slots:
    void chooseFile();
    void RemoveLink();
    void accept();
    void reject();
    void setProperties(QString lText, QString lLocation);


private:
    Ui::LinkModuleProperties *ui;

    QString linkText, linkLocation;
    bool validProperties;
};

#endif // LINKMODULEPROPERTY_H
