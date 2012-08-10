#ifndef SPLITFILE_H
#define SPLITFILE_H

#include <QDialog>

namespace Ui {
    class SplitFile;
}

class SplitFile : public QDialog
{
    Q_OBJECT

public:
    explicit SplitFile(QWidget *parent = 0);
    ~SplitFile();


private slots:
    /**
      @function
      browse file for edit
      */
    void browse();
    /**
      @function
      function for run show html, show edit text
      */
    void showFileText();

    /**
      @function
      Activated tab with auto split
      Show buttons
      Show data
      and other
      */
    void AutoSplitOn();

    /**
      @function
      Estimate split
      */
    void AutoEstimate();

    /**
      @function
      Browse path for output
      */
    void AutoBrowse();

    /**
      @function
      Run split
      */
    void AutoRun();

private:
    Ui::SplitFile *ui;

    /**
      @function
     */
    void setData();

    /**
      @fucntion
      Show file in textbrowser
      @param file  file path for show
      */
    void showFileHtml(QString file);

    /**
      @function
      Show text file for edit
      @param filepath  path file
      */
    void showFileEdit(QString filepath);

    /**
      @function
      create connects
      */
    void createConnect();

    QString TextOfFile;
    QString TagOfFile;
    int countFiles;

};

#endif // SPLITFILE_H
