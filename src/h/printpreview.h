/****************************************************************************
**
** Copyright (C) 2007-2008 Pavel Malakhov
**
** This file is part of Research Assistant project
** https://sourceforge.net/projects/rassistant
**
** The code is under GNU General Public License versions 2.0 or 3.0
** It is based on Trolltech Qt Assistant 4.3, http://trolltech.com
** and demo application TextEdit by Trolltech
****************************************************************************/

#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

#include <QMainWindow>
#include <QPrinter>
#include <QPointF>
#include <QSizeF>

class PreviewView;
class QTextDocument;

class PrintPreview : public QMainWindow
{
    Q_OBJECT
public:
    PrintPreview(const QTextDocument *document, QWidget *parent);
    virtual ~PrintPreview();

    QSizeF paperSize;
    QPointF pageTopLeft;

private slots:
    void print();
    void pageSetup();

private:
    void setup();

    QTextDocument *doc;
    PreviewView *view;
    QPrinter printer;
};

#endif // PRINTPREVIEW_H

