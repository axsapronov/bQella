#ifndef IMPORTBOOKDIALOG_H
#define IMPORTBOOKDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportBookDialog;
}

class Import;

/**
  @class
  ImportBookDialog
  class for import only book dialog
  use Import class
  */
class ImportBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportBookDialog(QWidget *parent = 0);
    ~ImportBookDialog();

signals:
    void SuccessfulImportBook();
    void ProjectPropsShow();

private slots:
    /**
      @function
      browse file for import
      */
    void browse();

    /**
      @function
      function of emit signal ProjectPropsShow()
      */
    void showPropertiesDialog();


private:
    Ui::ImportBookDialog *ui;

    Import * importm;

    /**
      @function
      set data
      */
    void setData();
    /**
      @fucntion
      function for first parse, and begin import
      */
    void accept();

    QString bookShortName, bookFullName, bookHtmlFilter;
    QString bookEncoding, bookTagChapter, bookTagVerse, bookPathFile;
    int bookCount;
};

#endif // IMPORTBOOKDIALOG_H
