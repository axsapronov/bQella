#ifndef FRDIALOG_H
#define FRDIALOG_H


#include <QDialog>
#include <QDir>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QStringListModel;
QT_END_NAMESPACE

namespace Ui {
    class FRDialog;
}

class FRDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FRDialog(QWidget *parent = 0);
    ~FRDialog();

private slots:
    void find();
    void replace();
    void updateCBChapter(QString path);
        void updateinfo();
private:
    Ui::FRDialog *ui;

    void updateCBBook();

    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);

    QDir currentDir;
};

#endif // FRDIALOG_H
