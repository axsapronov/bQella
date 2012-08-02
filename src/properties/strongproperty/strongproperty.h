#ifndef STRONGPROPERTY_H
#define STRONGPROPERTY_H

#include <QDialog>

namespace Ui {
class StrongProperties;
}

class StrongProperties : public QDialog
{
    Q_OBJECT

public:
    explicit StrongProperties(QWidget *parent = 0);
    ~StrongProperties();
    bool valid() { return validProperties; }
    QString StrongText() { return strongText; }
    QString StrongLocation() { return strongLocation; }


signals:
    void removeStrong();
    void updateStrong(QString lText, QString lLocation);

public slots:
    void RemoveStrong();
//    void accept();
//    void reject();
    void setProperties(QString lText, QString numberstr);


private slots:

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::StrongProperties *ui;
    QString strongText, strongLocation;
    int strongNumber;
    bool validProperties;

};

#endif // STRONGPROPERTY_H
