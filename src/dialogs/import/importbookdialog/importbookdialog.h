#ifndef IMPORTBOOKDIALOG_H
#define IMPORTBOOKDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportBookDialog;
}

class ImportBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportBookDialog(QWidget *parent = 0);
    ~ImportBookDialog();

private slots:
    void browse();

private:
    Ui::ImportBookDialog *ui;

    void setData();
    void accept();

    QString bookShortName, bookFullName, bookCount, bookHtmlFilter;
    QString bookEncoding, bookTagChapter, bookTagVerse, bookPathFile;
};

#endif // IMPORTBOOKDIALOG_H
