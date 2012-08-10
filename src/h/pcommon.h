/*!
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *    Author: Sapronov Alexander/WarmongeR
 *    E-Mail: sapronov.alexander92@.gmail.com
 *    Years: 2011, 2012
 *    WebSite: https://github.com/WarmongeR1/bQella
 */


#ifndef __PCOMMON_H__
#define __PCOMMON_H__

class QString;
class QStringList;
#include  <QTreeWidgetItem>
class QTextCodec;


#include "projectproperty.h"
#include "config.h"

//======== процедуры и функции общего назначения ============================
//---------------------------------------------------------------------------
QStringList SplitNumString(QString Str, QString Deviders);
int pSign(int x);                //сигнум. Возвращает 1 if x>0; -1 if x<0; 0 if x=0
//int pRound(double x);             //округляет вещ.число до целого;
double pRound(double x, int Accuracy); //округляет вещ.число до указанного знака;
QString pNumToStr(int N, QString One, QString Two, QString Five); //определяем склонение слова (день,дня,дней)
QString pBoolToStr(bool B);
bool pStrToBool(QString S);
QString pQString(char str[65000]);  //преобразуем тип убирая символ перевода строки
//дополняет строку до длины Len, выравнивая по Align (Left,Center,Right)
QString pFillString(QString Str, QString Chr, int Len, QString Align);
//QString FontStyleToString(TFont *FS);
//TFontStyles StringToFontStyle(QString FS);
//QString AlignmentToString(TAlignment A);
//TAlignment StringToAlignment(QString S);
QString GetOptionName(QString OptN);
QString GetOptionValue(QString OptN);
QString pMonthName(int n); //Полное название месяца из его номера
QString pMonthName(int n, QString format); //Название месяца из его номера в формате format
QString ZodiakName(int n);    //Название зодиакального зозвездия из его номера
int DaysInYear(long cYear);      //кол-во дней в году
int DaysInYear_Jul(long cYear);  //кол-во дней в году по Юлианскому календарю
bool LeapYear(long cYear);	//високосный ли год
bool LeapYear_Jul(long cYear);
int HoursInYear(long cYear);   //кол-во часов в году
int MinutesInYear(long cYear); //кол-во минут в году
int SecondsInYear(long cYear); //кол-во секунд в году
int Digit(int Number);  //ссумирует цифры числа между собой до получения одной цифры
QString IntToLetter(int n); //возвращает букву латинского алфавита
QString IntToLetter(int n, QString str); //возвращает букву из строки
int LetterToInt(QString letter); //возвращает номер буквы латинского алфавита
int LetterToInt(QString letter, QString srcAlphabet);//возвращает номер буквы в указанном алфавите
int r2i(QChar letter); //вспомогательная функция для RomanToInt()
int RomanToInt(QString romanInt); //переводит латинские числа в арабские
QString IntToRoman(int n); //переводит арабские числа в латинские
int max(int x1, int x2, int x3); //возвращает максимальный из аргументов
bool SameDigits(int x); //true если все цифры числа одинаковы, напр. 777
double Proporsion(double x, double y);//вычисляет пропорцию
int BooltoInt(bool foo); // возвращает true=1 false=0
QString BooltoQString(bool foo); // возвращает true= "1" false =  "0"
bool QStringtoBool(QString foo); // возвращает false if str.indexOf("0") else true
QString ist(QString str); // возвращает "none" если строка "", если нет, то возвращает саму строку
QString incstr(QString str, int n, QString mychar);  // increase the line, увеличивает строку добавляя символ в начале строки
QString tagToQuestion(QString tag); // функции, которые делают из <p>  ?p? и обратно.
QString questionToTag(QString question);
QString editStringList(QString list, QStringList tags, bool f); // функция нужная для экспорта, в зависимости от bool меняет теги на ?*_? или <*>
QString getCenterTag(QString str); // заменяет <p align="center" на <center
QString getHtmlCoolCode(QString str, QString i, QString chapter, bool chap);
QString getParseTagSpan(QString str, QString text, QString tag);
int getDepthTreeWidgetItem(QTreeWidgetItem *item); // вернуть глубину QTreeWidgetItem
QStringList getFillShortName(); // возвращает заполнение для combobox базовых коротких названий
QStringList getFillEncoding(); // возвращает заполнение для combobox кодировок
QString getTextInStr(QString str, int begin = 5, int end = 9); // возвращает текст от begin до end символа ( не реализована, работает чтобы доставать shortname)
QString getCheckShortNameForFile(QString str, QString full); // возвращает порядковый номер shortname(str) или полное название книги. Работает для уменьшения длины названия файла
QString checkTag(QString tag); // проверяет строку, начинается и заканчивается ли она на < и >
QString uncheckTag(QString tag); // обратная операция
QString checkEndTag(QString tag); // возвращает из тега <tag> </tag>
QString checkProcentRol(QString shortname, QString out, int procent = GL_PROCENT_OF_MATCHES); // разбивает shortname на части и проверяет насколько совпадает с getFillShortName()
//если с точностью до процента совпадает, то возвращает номер в shortname, а если нету такого, то out Применяется для импорта книги
QTextCodec* getCodecOfEncoding(QString encoding); // получает строку с названием кодировки и возвращает кодес с этй кодировкой ( написана для уменьшения дублирования кода)
QString removeFirst(QString str, QString remove); // возвращает строку с удаленным первым вхождением

/**
  @function
  Replace spaces in qstring to text(default = }<B-B>{)
  @param str  qstring
  @param text  default = }<B-B>{
  */
QString replaceSpaceInStrToText(QString str, QString text = "}<B-B>{");

/**
  @function
  Function to show to qDebug moduleproperties
  @param *pr  moduleproperties
  */
void printToDebugModuleProperties(ModuleProperties *pr);

void visitTree(QStringList &list, QTreeWidgetItem *item);
QStringList visitTree(QTreeWidget *tree);
QStringList getParentText(QTreeWidgetItem *item);


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
bool toLog(QString logFN, QString logMessage); //добавляет в журнал строку в формате "Дата Время Сообщение"
bool createEmptyHtml(QString fileName, QString title);
bool createEmptyHtml(QString fileName, QString title, QString text);
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

/// strong
QString getInfoFromStrongFile(QString filename, QString number); // возвращает данные о стронге из файла
QString getFileNameOfStrong(QString horg, QString numberstr); /// возвращает название файла. ПРинимает греческй или иврит стронг и номер стронга

#endif // __PCOMMON_H__
