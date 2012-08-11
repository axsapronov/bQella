#ifndef PREVIEWMODULE_H
#define PREVIEWMODULE_H

#include <QDialog>

namespace Ui {
    class PreviewModule;
}

class PreviewModule : public QDialog
{
    Q_OBJECT

public:
    explicit PreviewModule(QWidget *parent = 0);
    ~PreviewModule();

private:
    Ui::PreviewModule *ui;
};

#endif // PREVIEWMODULE_H
