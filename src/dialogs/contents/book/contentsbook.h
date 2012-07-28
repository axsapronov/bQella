#ifndef CONTENTSBOOK_H
#define CONTENTSBOOK_H

#include <QDialog>

class QStringList;
namespace Ui {
    class ContentsBook;
}

class ContentsBook : public QDialog
{
    Q_OBJECT

public:
    explicit ContentsBook(QWidget *parent = 0);
    ~ContentsBook();
public slots:
    void setProperty(QString filebook,
                     QString namebook, QString chapternumber,
                     QString filecontents, QStringList list);
    void deleteContent(QString filename);
    void setEdit(bool editable);
private slots:
    void createContents();
private:
    Ui::ContentsBook *ui;

    void accept();
    void reject();
    bool editable;
};

#endif // CONTENTSBOOK_H
