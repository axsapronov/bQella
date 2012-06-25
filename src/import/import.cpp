#include "import.h"


#include "helpdialog.h"
#include "mainwindow.h"



Import::Import(QObject *parent) :
    QObject(parent)
{

    //    mystr = "test1";
    //    qDebug() << "Debug: _Import()" << "mystr =" << mystr;
}



void Import::importBook(QString file)
{
    qDebug() << "Debug: _Import::importBook(QString file)" << "Start import book";

}

void Import::importChapter(QString file)
{
    qDebug() << "Debug: _Import::importBook(QString file)" << "Start import Chapter";

}

void Import::importModule(QString file)
{
    qDebug() << "Debug: _Import::importBook(QString file)" << "Start import Module";

}

//void Import::setTestValue(QString test)
//{
//    mystr = test;
//}

//QString Import::getTestValue()
//{
//    return mystr;
//}

//void Import::test()
//{
//    qDebug() << "Debug: _Import::test()" << "mystr = " << mystr;
//}
