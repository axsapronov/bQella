#ifndef IMPORT_H
#define IMPORT_H

#include <QObject>

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

};

#endif // IMPORT_H
