#ifndef IMPORTBOOKDIALOG_H
#define IMPORTBOOKDIALOG_H

#include <QDialog>

namespace Ui {
    class ImportBookDialog;
}

class Import;
class PreviewBook;

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


public slots:
    /**
      @function
      function to show preview
      */
    void showPreview();

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
      Function of emit signal ProjectPropsShow()
      */
    void showPropertiesDialog();

    /**
      @function
      Create files for preview book (chapters files)
      */
    void createBookPreview();


    void estimate();
    void deletePreviewFolder();



private:
    Ui::ImportBookDialog *ui;

    Import * importm;
    PreviewBook *prevbook;
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
    void saveData();
    QStringList getReplaceList();

    QString bookShortName, bookFullName, bookHtmlFilter, projectfile;
    QString bookEncoding, bookTagChapter, bookTagVerse, bookPathFile;
    int bookCount;
};

#endif // IMPORTBOOKDIALOG_H
