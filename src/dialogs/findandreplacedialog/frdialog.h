#ifndef FRDIALOG_H
#define FRDIALOG_H

#include <QDialog>

namespace Ui {
    class FRDialog;
}

class FRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FRDialog(QWidget *parent = 0);
    ~FRDialog();

private:
    Ui::FRDialog *ui;
};

#endif // FRDIALOG_H
