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

    void setData(QString filepath);
    void setPrjPath(QString path) {prjDir = path;}
    void createPreview();

private slots:
    void showPreviewBook();
private:
    Ui::PreviewModule *ui;
    PreviewBook *prevbook;

    QString prjDir;
    void accept();
    void reject();
    void removePreviewFiles();

    void createFolder(QString path);

    void createBookPreview();
};

#endif // PREVIEWMODULE_H
