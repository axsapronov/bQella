#ifndef TAGDIALOG_H
#define TAGDIALOG_H

#include <QDialog>

namespace Ui {
    class TagDialog;
}

class TagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagDialog(QWidget *parent = 0);
    ~TagDialog();
    bool valid() { return validProperties; }
    QString getTag() { return tag; }

signals:
    void addTag(QString lText);

public slots:
    void accept();
    void reject();
    void setProperties(QString lText);

private:
    Ui::TagDialog *ui;

    QString tag;
    bool validProperties;

};

#endif // TAGDIALOG_H
