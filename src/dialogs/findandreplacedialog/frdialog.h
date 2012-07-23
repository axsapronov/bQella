#ifndef FRDIALOG_H
#define FRDIALOG_H


#include <QDialog>
#include <QDir>

namespace Ui {
    class FRDialog;
    class QComboBox;
    class QLabel;
    class QPushButton;
    class QTableWidget;
    class QTableWidgetItem;
    class QStringListModel;
}

/**
@class FRDialog
class


*/
class FRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FRDialog(QWidget *parent = 0);
    ~FRDialog();

private slots:
    /**
    @function
    */
    void find();
    /**
    @function
    */
    void replace();
    /**
    @function
    */
    void updateinfo();

private:
    Ui::FRDialog *ui;

    /**
    @function
    */
    void updateCBBook();

    /**
    @function

    @param
    */
    void updateCBChapter(QString path);

    /**
    @function

    @param
    */
    void removeItemListChapter(QStringList &list);

    /**
    @function

    @param
    */
    void removeItemListBook(QStringList &list);

    /**
    @function

    @param
    */
    void updateItemforTable(QStringList &list, QStringList &list2);

    /**
    @function

    @param
    */
    QStringList findFiles(const QStringList &files, const QString &text);

    /**
    @function

    @param
    */
    void showFiles(const QStringList &files, const QStringList &files2 );

    /**
    @function

    @param
    */
    void accept();

    /**
    @function

    @param
    */
    void reject();


    QDir currentDir;
    QStringList bookfiles;
//    QString stringforfoldername;
};

#endif // FRDIALOG_H
