#ifndef IMPORT_H
#define IMPORT_H

#include <QObject>
#include <QDialog>

class Import : public QObject
{
    Q_OBJECT
public:
    explicit Import(QObject *parent = 0);

signals:

public slots:
    void importChapter(QString file);
    void importBook(QString file);
    void importModule(QString file);

private slots:
    void importIni(QString file);
    QString miniparserini(QString str, QString str2);

};

#endif // IMPORT_H
