#include "import.h"

#include "helpdialog.h"
#include "mainwindow.h"

Import::Import()
{
    mystr = "test1";
    qDebug() << "Debug: _Import()" << "mystr =" << mystr;
}


void Import::setTestValue(QString test)
{
    mystr = test;
}

QString Import::getTestValue()
{
    return mystr;
}

void Import::test()
{
    qDebug() << "Debug: _Import::test()" << "mystr = " << mystr;
}
