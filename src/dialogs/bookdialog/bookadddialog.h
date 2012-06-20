#ifndef BOOKADDDIALOG_H
#define BOOKADDDIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
    class BookAddDialog;
}
class QStringListModel;

class BookAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookAddDialog(QWidget *parent = 0);
    ~BookAddDialog();
    QString bookFullName;
    QString bookShortName;
    int bookChapterQty;
    bool bookCheckAutoChapterCreate;
private:
    Ui::BookAddDialog *ui;
    QStringListModel *typeModel;
private slots:
    void clickslot(QAbstractButton *AButton);
    void send();
    void reject();
signals:
    void signalbookaddChanged();



};

#endif // BOOKADDDIALOG_H
