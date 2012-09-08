
#include "strongcommon.h"
#include "pcommon.h"
#include "config.h"




//-----------------------------------------------------------------------------------------
QString getInfoFromStrongFile(QString horg, QString number)
{
    QString filename = getFileNameOfStrong(horg, number);
    // возвращает данные о стронге из файла
//    qDebug() << filename;
    if (QFile::exists(filename))
    {
        bool stop = false;
        QString line;
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&file);
            do
            {
                line = stream.readLine();
                if (line.indexOf("<number>"+number) >= 0)
                {
                    line = stream.readLine();
                    line = "<b>" + line + "</b><br>";
                    line.append(stream.readLine());
//                    line.replace("\t<option2>","\n\r<br>");
//                    line.remove("\t<option1>")
//                            .remove("</option1>")
//                            .remove("</option2>");
                    stop = true;
                }
            } while (!stream.atEnd() && !stop);
        }
        else
        {
            qDebug() << "Debug: _getInfoFromStrongFile: "
                     << "File " << filename << " not open";
        }
        file.close();
        return line;
    }
    else
    {
        qDebug() << "Debug: _getInfoFromStrongFile: "
                 << "File " << filename << " not exist";
    }
    return "";
}
//---------------------------------------------------------------------------------------
QString getFileNameOfStrong(QString horg, QString numberstr)
{
    int number = numberstr.toInt();
    QString numberfile;
    if (number % 200 == 0 && number != 0)
    {
        numberfile = QString::number(number % 200);
    }
    else
    {
        numberfile = QString::number(number / 200);
    }
    numberfile = incstr(numberfile, 2, "0");
    QString filename =
            Config::configuration()->AppDir() +
            "strong/" +
            horg + "/" +
            horg + numberfile;
    return filename;
}
