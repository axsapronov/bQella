#include "filecommon.h"
#include "pcommon.h"
#include "config.h"


//------------------------------------------------------------
QStringList getChapterList()
{
    QString path = Config::configuration()->PrjDir() + "/" + "_Preview_/_Preview_/";
    QDir dir(path);
    QStringList listFiles = dir.entryList(QDir::Files);
//    qDebug() << "path = " << path;

    QStringList files;
    foreach (QString entry, listFiles)
    {
        QString entryAbsPath = dir.absolutePath() + "/" + entry;
        if (entry.indexOf("_chapter_") != -1)
        {
            files << entryAbsPath;
        }
    }
//        qDebug() << " files " << files << "\n";
    return files;
}

//------------------------------------------------------------
QStringList  getChapterComboText()
{
    QStringList list = getChapterList();
    QStringList chapters;
    for (int index = 0; index < list.size(); index++)
    {
        QString str = list.at(index);
        /// get number chapter
        str = getFileNameAbs(str);
        int pos = str.indexOf("_chapter_");
        str = str.remove(0,pos)
                .remove("_chapter_")
                .remove("_");
        str = QObject::tr("Chapter") + " " + incstr(str,3," ");

        chapters << str;
    }
    //    qDebug() << "chapters" << chapters << "\n";
    return chapters;
}
///-------------------------------------------------------
QStringList getListFilesFromBibleqtIni(QString filename)
{
    QStringList listFiles;

    /// hindi
    /// copy from import module

    QFile file(filename);
    filename.remove(filename.length()-11, 11);
    QString line;
    if ( file.open(QIODevice::ReadOnly) )
    {
        int BookQtyIn = 2000;
        int book = 0;
        //        int count;
        // file opened successfully
        QTextStream stream( &file );        // use a text stream

        // until end of file...
        do {
            // read and parse the command line
            line = stream.readLine();         // line of text excluding '\n'
            // do something with the line
            if (line != "" and line.indexOf("//") <0)
            {
                if (miniparserini(line,"PathName") != "")
                {
                    //парсим инфу о книгке
                    QString line2 = stream.readLine();
                    QString line3 = stream.readLine();
                    QString line4 = stream.readLine();
                    QString path = filename + miniparserini( line, "PathName");
                    book++;
                    //                    qDebug() << "path = " << path;
                    listFiles << path;
                }
            }
        } while ((!line.isNull()) || (BookQtyIn == book));
        // Close the file
        file.close();
    }
    return listFiles;
}
///-------------------------------------------------------
QStringList getListValueTextFromBibleqtIni(QString filename, QString encoding)
{

    QStringList listValueText;

    /// hindi
    /// copy from import module

    QFile file(filename);
    filename.remove(filename.length()-11, 11);
    QString line;
    if ( file.open(QIODevice::ReadOnly) )
    {
        int BookQtyIn = 2000;
        int book = 0;
        //        int count;
        // file opened successfully
        QTextStream stream( &file );        // use a text stream
        QTextCodec * codec = getCodecOfEncoding(encoding);
        stream.setCodec(codec);

        // until end of file...
        do {
            // read and parse the command line
            line = stream.readLine();         // line of text excluding '\n'
            // do something with the line
            if (line != "" and line.indexOf("//") <0)
            {
                if (miniparserini(line,"PathName") != "")
                {
                    //парсим инфу о книгке
                    QString line2 = stream.readLine();
                    QString line3 = stream.readLine();
                    QString line4 = stream.readLine();
                    QString full = miniparserini( line2, "FullName");
                    book++;
                    listValueText << full;
                }
            }
        } while ((!line.isNull()) || (BookQtyIn == book));
        // Close the file
        file.close();
    }
    return listValueText;
}

///-------------------------------------------------------
QString getEncodingFromFile(QString file, QString language)
{
    QProcess pr;

#ifdef Q_OS_WIN
    pr.start("enca.exe -L " +language + " \""  + file + "\"");
#endif
#ifdef Q_OS_LINUX
    pr.start("enca -L " + language + " \""  + file + "\"");
#endif
//    pr.start("pwd");

    QString encoding = "";
    if (pr.waitForFinished() == true)
    {
        QByteArray b = pr.readAll();
        encoding = QString(b);
    }
    if (encoding.indexOf("Universal transformation format 8 bits;") >= 0 ) encoding = "UTF-8";
    if (encoding.indexOf("Universal transformation format 16 bits;") >= 0 ) encoding = "UTF-16";
    if (encoding.indexOf("Universal transformation format 32 bits;") >= 0 ) encoding = "UTF-32";
    if (encoding.indexOf("MS-Windows code page 1251") >= 0) encoding = "Windows-1251";
    if (encoding.indexOf("MS-Windows code page 1252") >= 0) encoding = "Windows-1252";
    if (encoding.indexOf("MS-Windows code page 1253") >= 0) encoding = "Windows-1253";
    if (encoding.indexOf("MS-Windows code page 1254") >= 0) encoding = "Windows-1254";
    if (encoding.indexOf("MS-Windows code page 1255") >= 0) encoding = "Windows-1255";
    if (encoding.indexOf("MS-Windows code page 1256") >= 0) encoding = "Windows-1256";
    if (encoding.indexOf("MS-Windows code page 1257") >= 0) encoding = "Windows-1257";
    if (encoding.indexOf("MS-Windows code page 1258") >= 0) encoding = "Windows-1258";
    if (encoding.indexOf("KOI8-R Cyrillic") >= 0) encoding = "KOI8-R";
    if (encoding.indexOf("KOI8-U Cyrillic") >= 0) encoding = "KOI8-U";
    if (encoding.indexOf("Unrecognized encoding") >= 0) encoding = "UTF-8";
//    qDebug() << "encoding = " << encoding;
    return encoding;
}



//======= работа с файлами =======
//-------------------------------------------------

QString urlifyFileName(const QString &fileName)
{
    QString name = fileName;
    QUrl url(name);
#if defined(Q_OS_WIN32)
    if (!url.isValid() || url.scheme().isEmpty() || url.scheme().toLower() != QString("file:")) {
        int i = name.indexOf(QChar('#'));
        QString anchor = name.mid(i);
        name = name.toLower();
        if (i > -1)
            name.replace(i, anchor.length(), anchor);
        name.replace(QChar('\\'), QChar('/'));
        foreach (QFileInfo drive, QDir::drives()) {
            if (name.startsWith(drive.absolutePath().toLower())) {
                name = QString("file:") + name;
                break;
            }
        }
    }
#else
    if (!url.isValid() || url.scheme().isEmpty())
        name.prepend(QString("file:"));
#endif
    return name;
}

//-------------------------------------------------
QString unurlifyFileName(const QString &fileName)
{
    QString fn = fileName;
#if defined(Q_OS_WIN32) //!+! check in Linux if this works correct
    if (fn.startsWith("file:///")) {
        fn.remove(0,8);
    }
#endif
    if (fn.startsWith("file:")) {
        fn.remove(0,5);
    }
    return fn;
}

//-------------------------------------------------
//Turns absolute path to relative to 'path'
//Difference from QDir::relativeFilePath() in: adds "./" and removes "file:" or "file:///"
QString relatifyFileName(QString url, QString path)
{
    QString str = unurlifyFileName(url);
    if ((!str.isEmpty()) && (!str.startsWith("."))){
        //we assume that the passing paths have "/" as a dir separator, since that's how Qt stores paths
        if (!path.endsWith("/")) path = path + "/";
        QFileInfo fi(str);
        QString path1 = fi.absolutePath()  + "/"; path1 = path1.trimmed();
        QString path2 = path.trimmed();
        //Now we have two correct defined paths, let's make "str" relative to path1
        //For Windows we need to be sure that both paths are on the same drive
        if (path1[0] == path2[0]) {
            if ( path1 == path2){	//same path
                str = "./" + fi.fileName();
            }else{	//build relative path
                int n1 = path1.length();
                int n2 = path2.length();
                int n = n1;
                if (n > n2)  n = n2;
                QString commonPath = "";
                QString prefix = "";
                int i;
                for (i=0; i<n; i++){
                    if (path1[i] == path2[i]) commonPath += path2[i];
                    else break;
                }
                path1.remove(0, commonPath.length() );
                path2.remove(0, commonPath.length() );
                n = path2.count("/");
                if (n == 0)
                    prefix = "./";
                else
                    for (i=1; i<=n; i++) prefix += "../";
                str = prefix + path1 + fi.fileName();
            }
        }
    }
    return str;
}	//relatifyFileName

//-------------------------------------------------
QStringList relatifyFileList(QStringList urls, QString path)
{
    QStringList sl;

    QStringList::iterator it = urls.begin();
    for (; it != urls.end(); ++it) {
        sl << relatifyFileName(*it, path);
    }
    return sl;
}

//-------------------------------------------------
QString absolutifyFileName(QString fn, QString path)
{
    QString afn;
    if (!fn.isEmpty()){
        QDir dir(path);
        afn = dir.cleanPath( dir.absoluteFilePath(fn) );
    }
    return afn;
}

//-------------------------------------------------
QStringList absolutifyFileList(QStringList fns, QString path)
{
    QStringList sl;

    QStringList::iterator it = fns.begin();
    for (; it != fns.end(); ++it) {
        sl << absolutifyFileName(*it, path);
    }
    return sl;
}

//-------------------------------------------------
QString removeAnchorFromLink(const QString &link)
{
    int i = link.length();
    int j = link.lastIndexOf(QChar('/'));
    int l = link.lastIndexOf(QDir::separator());
    if (l > j)
        j = l;
    if (j > -1) {
        QString fileName = link.mid(j+1);
        int k = fileName.lastIndexOf(QChar('#'));
        if (k > -1)
            i = j + k + 1;
    }
    return link.left(i);
}

//-------------------------------------------------
bool verifyDirectory(const QString &str)
{
    QFileInfo dirInfo(str);
    if (!dirInfo.exists())
        return QDir().mkdir(str);
    if (!dirInfo.isDir()) {
        qWarning("'%s' exists but is not a directory", str.toUtf8().constData());
        return false;
    }
    return true;
}

//-------------------------------------------------
bool pathIsRelative(QString path)
{
    bool r = false;
    if ( (path.startsWith("./")) || (path.startsWith("../")) )
        r = true;
    return r;
}

//-------------------------------------------------
QString CreateValidWebFileName(QString str)
{
    QString validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_,.()[]{}<>~!@#$%^&*+=";
    QString fn;
    int i;

    for (i=0; i < str.size(); ++i){
        if ( validChars.contains(str[i]) )
            fn = fn + str[i];
    }
    if (fn.isEmpty() ) fn = "file";
    return fn;
}

//-------------------------------------------------
bool toLog(QString logFN, QString logMessage)
{
    QFile file(logFN);
    if(!file.open(QIODevice::Append | QIODevice::Text)) {
        return false;
    }
    QTextStream ts(&file);
    //ts.setCodec("UTF-8"); //do not set codec, write in system codepage yet
    QDateTime dt = QDateTime::currentDateTime();
    ts << dt.toString("yyyy.MM.dd hh:mm:ss  ") << logMessage << endl;
    file.close();
    return true;
}

//-------------------------------------------------
bool createEmptyHtml(QString fileName, QString title)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.exists()){		//create file if it's not exist
        if (file.open(QIODevice::ReadWrite)){	//try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            ts << "<html>\n<head>" << endl;
            ts << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
            ts << "<title>" << title <<"</title>" << endl;
            ts << "</head>\n<body>\n</body></html>" << endl;
            file.close();

            //QFile
        }else{
            ret = false;
        }
    }
    return ret;
}
//-------------------------------------------------
bool createEmptyHtml(QString fileName, QString title, QString text)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.exists()){		//create file if it's not exist
        if (file.open(QIODevice::ReadWrite)){	//try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            ts << "<html>\n<head>" << endl;
            ts << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
            ts << "<title>" << title <<"</title>" << endl;
            ts << "</head>\n<body>\n" << text << "\n</body>\n</html>" << endl;
            file.close();

            //QFile
        }else{
            ret = false;
        }
    }
    return ret;
}
//-------------------------------------------------
bool createEmptyHtmlWithEncoding(QString fileName, QString title, QString text, QString encoding)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.exists()){		//create file if it's not exist
        if (file.open(QIODevice::ReadWrite)){	//try to open or create file
            QTextStream ts(&file);
            QTextCodec * codec = getCodecOfEncoding (encoding);
            ts.setCodec(codec);
            ts << "<html>\n<head>" << endl;
            ts << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />" << endl;
            ts << "<title>" << title <<"</title>" << endl;
            ts << "</head>\n<body>\n" << text << "\n</body>\n</html>" << endl;
            file.close();

            //QFile
        }else{
            ret = false;
        }
    }
    return ret;
}
