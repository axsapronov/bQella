#ifndef PREVIEWBOOK_H
#define PREVIEWBOOK_H

#include <QDialog>

namespace Ui {
    class PreviewBook;
}

class Import;
/**
  @class PreviewBook
  class for preview book
  */
class PreviewBook : public QDialog
{
    Q_OBJECT

signals:
    void createBookPreview();
public:
    explicit PreviewBook(QWidget *parent = 0);
    ~PreviewBook();

    /**
      @function
      set data to ui
      @param filepath(QString)  param at set
      */
    void setData(QString filepath);

    void createBookPreviewFunc();
    void setPathToBook(QString file) {pathToBook = file;}

private slots:
    /**
      @function
      Show chapter in browser, edit frame
      @param int  count - number in combobox item
      */
    void showChapter(int count);
private:
    Ui::PreviewBook *ui;
    QString pathToBook;

    /**
      @fucntion
      */
    void accept();
    void createConnects();

    void createFolder(QString filepath);


//    /**
//      @fucntion
//      */
//    void reject();

    void removePreviewFiles();

};

#endif // PREVIEWBOOK_H
