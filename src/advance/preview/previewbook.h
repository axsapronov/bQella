#ifndef PREVIEWBOOK_H
#define PREVIEWBOOK_H

#include <QDialog>

namespace Ui {
    class PreviewBook;
}

/**
  @class PreviewBook
  class for preview book
  */
class PreviewBook : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewBook(QWidget *parent = 0);
    ~PreviewBook();

    /**
      @function
      set data to ui
      @param filepath(QString)  param at set
      */
    void setData(QString filepath);

private:
    Ui::PreviewBook *ui;

    /**
      @fucntion
      */
    void accept();


    /**
      @fucntion
      */
    void reject();

    void removePreviewFiles();

};

#endif // PREVIEWBOOK_H
