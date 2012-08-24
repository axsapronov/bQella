#ifndef RIGHTPANEL_H
#define RIGHTPANEL_H

#include <QWidget>

namespace Ui {
    class RightPanel;
}
class MainWindow;

class RightPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RightPanel(QWidget *parent, MainWindow *h);
    ~RightPanel();

private:
    Ui::RightPanel *ui;
};

#endif // RIGHTPANEL_H
