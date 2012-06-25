#ifndef EXPORT_H
#define EXPORT_H

#include "helpdialog.h"



#include <QTextCodec>
#include <QTextStream>
#include <QWidget>
#include <QString>
#include <QObject>


class HelpDialog;

class Export : public QObject
{
    Q_OBJECT
public:
    explicit Export(QObject *parent = 0);

public slots:

    void test();
//    void setFileBibleqtName(QString);
//    void setTopLevelItemCount(int i);

    void exportCreateDir(QString current_dir);
    void exportBibleqtIni(QString path, int count);
    QString exportChapter (QString filename, int count, bool chapt);

    void exportBibleqtIniInfo(QString filebibleqt, QString filename, int count);

private slots:
    QString getNameFolder(QString cur_dir);
private:
    QString file;
};

#endif // EXPORT_H
