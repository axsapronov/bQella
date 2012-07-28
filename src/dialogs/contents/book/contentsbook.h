#ifndef CONTENTSBOOK_H
#define CONTENTSBOOK_H

#include <QDialog>

namespace Ui {
    class ContentsBook;
}

class ContentsBook : public QDialog
{
    Q_OBJECT

public:
    explicit ContentsBook(QWidget *parent = 0);
    ~ContentsBook();

private:
    Ui::ContentsBook *ui;
};

#endif // CONTENTSBOOK_H
