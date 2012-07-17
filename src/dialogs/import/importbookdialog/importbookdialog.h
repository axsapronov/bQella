#ifndef IMPORTBOOKDIALOG_H
#define IMPORTBOOKDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportBookDialog;
}

class Import;

class ImportBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportBookDialog(QWidget *parent = 0);
    ~ImportBookDialog();

signals:
    void SuccessfulImportBook();

private slots:
    void browse();

private:
    Ui::ImportBookDialog *ui;

    Import * importm;
    void setData();
    void accept();

    void importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty);

//    void addInfoInProjectFile(QString filename, QString text, bool chapter );

    QString bookShortName, bookFullName, bookHtmlFilter;
    QString bookEncoding, bookTagChapter, bookTagVerse, bookPathFile;
    int bookCount;
};

#endif // IMPORTBOOKDIALOG_H
