#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QDialog>

namespace Ui {
    class HelpBrowser;
}

class HelpBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit HelpBrowser(QWidget *parent = 0, const QString& strPath = "ti",
                         const QString& strFileName = "ta" );
    ~HelpBrowser();

private:
    Ui::HelpBrowser *ui;
};

#endif // HELPBROWSER_H
