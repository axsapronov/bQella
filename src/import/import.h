#ifndef IMPORT_H
#define IMPORT_H


#include "projectproperty.h"

#include <QObject>
#include <QDialog>


class Import : public QObject
{
    Q_OBJECT
public:
    explicit Import(QObject *parent = 0);

signals:

public slots:
    QString importChapter(QString file);
    void importBook(QString pathName, QString FullName, QString ShortName, int ChapterQty);
    void importModule(QString file);
    void addContentToProjectFile(QString text, bool tr);

    QString getPrjFN();
    QString getStartPage();

private slots:
    void importIni(QString file);
    QString miniparserini(QString str, QString str2);

    void importProjectFile();

    void createImportFolder(QString path);
    void createBibleIni(QString file);
    void createChaterFile(QString file,QString text, int i);
    void createBookFile(QString pathName, QString FullName, QString ShortName, int ChapterQty);
    void createProjectFile();
    void createInstructionFile();

    void addContentToEndProjectFile();
private:
    int BookQty;
    QString VerseSign;
    QString ChapterSign;

};

#endif // IMPORT_H
