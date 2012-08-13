#ifndef PREVIEWMODULE_H
#define PREVIEWMODULE_H

#include <QDialog>

namespace Ui {
    class PreviewModule;
}

class PreviewBook;
/**
  @class PreviewModule
  class for preview module
  used previewbook class
  */
class PreviewModule : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewModule(QWidget *parent = 0);
    ~PreviewModule();

    /**
      @function
      @param
      */
    void setData(QString filepath);
    /**
      @fucntion
      @param
      */
    void setPrjPath(QString path) {prjDir = path;}
    /**
      @function
      @param
      */
    void setEncoding(QString en) {encoding = en;}
    /**
      @fucntion
      */
    void createPreview();
    /**
      @function
      @param
      */
    void setFileBibleqtIni(QString path) {fileBibleqtIni = path;}
    /**
      @function
      @return
      */
    QString getFileBibleqtIni() {return fileBibleqtIni;}
    /**
      @function
      @return
      */
    QString getEncoding() {return encoding;}

signals:
    void createBookPreviewModule(QString);

private slots:
    /**
      @function
      */
    void showPreviewBook();
private:
    Ui::PreviewModule *ui;
    PreviewBook *prevbook;

    QString prjDir;
    QString fileBibleqtIni;
    QString encoding;

    void accept();
    void reject();

    /**
      @function
      */
    void removePreviewFiles();
    /**
      @function
      */
    void createListBook();
    /**
      @function
      @param
      */
    void createFolder(QString path);
    /**
      @function
      @param
      */
    void createBookPreview();
};

#endif // PREVIEWMODULE_H
