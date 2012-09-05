#ifndef __FILECOMMON_H
#define __FILECOMMON_H

#include <QString>

//======= работа с файлами =======
//--------------------------------
QString unurlifyFileName(const QString &fileName);		//remove "file:"
QString urlifyFileName(const QString &fileName);		//add "file:"
QString relatifyFileName(QString url, QString path);	//returns path like ../../images/pict.jpg Difference from QDir::relativeFilePath() in: it adds "./" and removes "file:"
QStringList relatifyFileList(QStringList urls, QString path);
QString absolutifyFileName(QString fn, QString path);	//returns absolute file path
QStringList absolutifyFileList(QStringList fns, QString path);
QString removeAnchorFromLink(const QString &link);		//returns link without anchor
bool verifyDirectory(const QString &str);	//create dir if not exist and if there is no such file name
bool pathIsRelative(QString path);	//return true if path starts with ./ or ../
QString CreateValidWebFileName(QString str); //выбирает из строки только разрешённые символы



/**
  @function
  get list chapter files in curprj/_Preview_/
  used in previewbook
  @return QStringList  list of file path to chapter files
  */
QStringList getChapterList();

/**
  @function
  get list text chapter files in curprj/_Preview_/
  used in previewbook: set to combobox
  @return QStringList  list of value text
  */
QStringList getChapterComboText();

/**
  @function
  Get list files from bibleqt.ini file
  @param QString filepath  path to bibleqt.ini file
  @return QStringList list  list book files
  */
QStringList getListFilesFromBibleqtIni(QString filepath);

/**
  @function
  Get list full names from book from bibleqt.ini file
  @param QString filepath  path to bibleqt.ini file
  @param encoding  QString encoding file
  @return QStringList list  list full text
  */
QStringList getListValueTextFromBibleqtIni(QString filepath, QString encoding);


/**
  @function
  get text from file
  @param file  QString path to file
  @param encoding
  @return QString
  */
QString getTextFromFile(QString file, QString encoding); // достать весь текст из файла


/**
  @function
  Auto detect encoding file
  @param filepath  QString of file path
  @param language  QString of language, default = russian
  @return encoding  QString encoding
  */
QString getEncodingFromFile(QString file, QString language="russian");


bool toLog(QString logFN, QString logMessage); //добавляет в журнал строку в формате "Дата Время Сообщение"
bool createEmptyHtml(QString fileName, QString title);
bool createEmptyHtml(QString fileName, QString title, QString text);

/**
  @function
  @param
  @param
  @param
  @param
  */
bool createEmptyHtmlWithEncoding(QString fileName, QString title, QString text, QString encoding); // тоже самое что и предыдущее но с учетом кодировки

void replaceTextOfFile(QString filepath, QString beforetext, QString replacetext); // заменяет в файле тест с before на replacetext
QString getShortName(QString filename); // возвращает shortname из файла (можно добывать не только shortname)
QString getParamBook(QString filename, QString param); // возвращает параметр из файла. Вид файла    "PARAM = VALUE"
QString miniparserini(QString str, QString po);
QString replaceFullShortName(QString line, QString text, QString name); // возвращает строку с заменой текста в fullname и shortname
void writeQStringList(QString file, QStringList list); // записывает QStringList в файл ( обычный цикл)
QString checkExistenceFile(QString file); // проверяет существует файл, если существует, то добавляет к нему символ _ в конце. Происходит в цикле
QString getFileNameAbs(QString file); // возвращает название файла без разрешения и пути. Т.е. /home/warmonger/develop/чтони-ть там еще/ файл.py  вернет файл
void removeStringInFile(QString file, QStringList strings); // удаляет из файла строки из qstringlist
bool checkFileContainsText(QString filename, QString text); // проверяет есть ли текст в файле
bool createFileText(QString fileName, QString text); // создает  файл с текстом ( так как книги не юзаются, то можно и обычным файлом юзать их)
bool addToEndFile(QString fileName, QString text); // добавить в конец файла текст


#endif //__FILECOMMON_H
