#ifndef IMPORT_H
#define IMPORT_H


#include <QWidget>

class Import
{
public:
    Import();
public slots:
    void setTestValue(QString test);
    QString getTestValue();
    void test();
private:
    QString mystr;

};

#endif // IMPORT_H
