#ifndef RIGHTPANEL_H
#define RIGHTPANEL_H

#include <QWidget>

namespace Ui {
    class RightPanel;
}
class MainWindow;

/**
  @class
  RightPanel class.  This is right panel mainwindow
  */
class RightPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RightPanel(QWidget *parent, MainWindow *h);
    ~RightPanel();

    /**
      @function
      Show project info
      */
    void refreshInfo();

private:
    Ui::RightPanel *ui;

    /**
      @function
      Clear info project
      */
    void clearInfoProject();
    /**
      @function
      Function for begin clear labels and other of rightpanel ui
      */
    void clearInfo();
};

#endif // RIGHTPANEL_H
