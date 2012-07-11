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



#include "pcommon.h"
#include "config.h" // для строчек аля toUtf8 // если отдельно пользоваться, то свитчи убрать

#include <QDebug>
#include <QtGui/QApplication> //для translate()
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

#include <QString>
#include <QTextCodec>




//-------------------------------------------------
//======== процедуры и функции общего назначения ============================
//-------------------------------------------------
//Возвращаем список строк, где в каждой строке по одному числу
// Deviders определяет разделители чисел, все остальные символы игнорируются
// Таким образом, если строка разделителей = "./ ", то получаем
// "12.1/98" - это три числа 12, 1 и 98,
// "12 января 98г." -  два числа: 12 и 98
// "12янв98" - одно число: 1298
QStringList SplitNumString(QString Str, QString Deviders)
{
    int i;
    QStringList str2;
    QString ss="";
    for (i=0; i<= Str.length(); i++){  //перебираем все символы строки
	if (Str[i].isDigit()) 
            ss +=Str[i];
        else {
            if (Deviders.contains(Str[i])){
                if (!ss.isEmpty()) str2.append(ss);
                ss = "";
            }
        }
    }
    if (!ss.isEmpty()) str2.append(ss);

    return str2;
}

//-------------------------------------------------
//сигнум. Возвращает 1 if x>0; -1 if x<0; 0 if x=0
int pSign(int x){
    int y=0;

    if (x !=0) y= x/abs(x);
    return y;
}

//-------------------------------------------------
// округляет вещ.число до целого;
/*int pRound(double x){ //-pm- use Qt: int qRound ( qreal value ) 
  int y;
  float z;

  y = floor(x);
  z = x - y;
  if (z>=0.5) y++;
  return y;
}*/
//-------------------------------------------------

double pRound(double x, int Accuracy){ //округляет вещ.число до указанного знака
    double y, res;
    int i, m=1;

    for (i=1; i<=Accuracy; i++) m = m*10;
    y = qRound(x * m);
    res = y / m;
    return res;
}
//-------------------------------------------------

// определяем склонение слова, напр "день","дня","дней"
QString pNumToStr(int N, QString One, QString Two, QString Five){
    QString tmpstr;
    int x;

    // нам достаточно проанализировать последние две цифры числа N
    x = abs(N);
    if (x > 100) x = x / 100 * 100;
    // проверяем если x из диапазона 11-19
    if ((x>10)&(x<20))tmpstr = Five;
    else { // значение имеет только последняя цифра
        if (x>9) x = x % 10;
        switch (x) {
        case 1: tmpstr = One;
            break;
        case 2: case 3: case 4:
            tmpstr = Two;
            break;
        case 0: case 5: case 6: case 7: case 8: case 9:
            tmpstr = Five;
            break;
        }
    }
    return tmpstr;
}

//-------------------------------------------------
// преобразования логических переменных
QString pBoolToStr(bool B){
    QString bstr;
    if (B) bstr="True";
    else bstr="False";
    return bstr;
}

//--------------------------------
bool pStrToBool(QString S){
    bool B;
    QString str = S.trimmed();
    if ((str == "true") || (str == "1"))
        B = true;
    else
        B = false;
    return B;
}

//-------------------------------------------------
// преобразуем тип убирая символ перевода строки
QString pQString(char Str[65000])
{
    QString Str2;

    if (Str[strlen(Str)-1]=='\n') Str[strlen(Str)-1]=0;
    Str2 = QString(Str);
    return Str2;
}

//-------------------------------------------------
//дополняет строку Str указанным символом Chr до длины Len
QString pFillString(QString Str, QString Chr, int Len, QString Align){
    QString ReturnStr;
    int x,i;

    ReturnStr = Str;
    if (Len > Str.length()){ //если требуемая длина строки меньше исходной, то ничего не делаем
        if (Align.toLower() == "left")
            while (ReturnStr.length() < Len) ReturnStr = ReturnStr + Chr;
        if (Align.toLower() == "right")
            while (ReturnStr.length() < Len) ReturnStr = Chr + ReturnStr;
        if (Align.toLower() == "center"){
            x = (Len - Str.length()) / 2;  //остаток учтём ниже
            for (i=1; i<=x; i++) ReturnStr = Chr + ReturnStr;
            x = Len - Str.length() - x;    //это на случай нечётного кол-ва добавляемых символов
            for (i=1; i<=x; i++) ReturnStr = ReturnStr + Chr;
        }
    }
    return ReturnStr;
}

/*
//-------------------------------------------------
QString FontStyleToString(TFont *FS){
  QString tmp="";
  if (FS -> Style.Contains(fsBold))       tmp  = "Bold+";
  if (FS -> Style.Contains(fsItalic))     tmp += "Italic+";
  if (FS -> Style.Contains(fsUnderline))  tmp += "Underline+";
  if (FS -> Style.Contains(fsStrikeOut))  tmp += "StrikeOut+";
  if (tmp == "") tmp = "Plain";
  return tmp;
}

//-------------------------------------------------

TFontStyles StringToFontStyle(QString FS){
  TFontStyles tmp;
  QString S,Stmp;
  if (FS != "Plain"){
    Stmp = FS;
    while (Stmp.length() > 0){
      S = Stmp.SubString(1, Stmp.Pos("+")-1);
      Stmp.Delete(1,Stmp.Pos("+"));
      if (S == "Bold")      tmp << fsBold;
      if (S == "Italic")    tmp << fsItalic;
      if (S == "Underline") tmp << fsUnderline;
      if (S == "StrikeOut") tmp << fsStrikeOut;
  } }
  return tmp;
}

//-------------------------------------------------
QString AlignmentToString(TAlignment A){
  QString tmp;
  if (A == taLeftJustify) tmp = "Left";
  else if (A == taRightJustify) tmp = "Right";
  else tmp = "Center";
  return tmp;
}

//-------------------------------------------------
TAlignment StringToAlignment(QString S){
   TAlignment tmp;
   if (S == "Left") tmp = taLeftJustify;
   else if (S== "Right") tmp = taRightJustify;
   else tmp = taCenter;
   return tmp;
}

//-------------------------------------------------
// -pm- use Qt SaveSettins and LoadSettings instead
QString GetOptionName(QString OptN){
   QString OName =OptN;
   int ind=0;
   ind = OptN.Pos("=");
   OName = OptN.SubString(1,ind-1);
   OName = OName.Trim();
   return OName;
}

//-------------------------------------------------
QString GetOptionValue(QString OptV){
   QString OVal;
   int ind=0;

   ind = OptV.Pos("=");
   OVal = OptV.SubString(ind+1,256);
   OVal = OVal.Trim();
   return OVal;
}
*/

//-------------------------------------------------
// порядковый номер месяца преобразуем в название
QString pMonthName(int n, QString format)
{
    QString ret = "";
    QString form = format.toLower();
    QStringList MonthsNameFull, MonthsNameShort, MonthsNameOther;

    MonthsNameFull	<< QApplication::translate("pCommon","January", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","February", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","March", 0, QApplication::UnicodeUTF8)
                        << QApplication::translate("pCommon","April", 0, QApplication::UnicodeUTF8)   <<QApplication::translate("pCommon","May", 0, QApplication::UnicodeUTF8) 		<<QApplication::translate("pCommon","June", 0, QApplication::UnicodeUTF8)
                        << QApplication::translate("pCommon","July", 0, QApplication::UnicodeUTF8)    <<QApplication::translate("pCommon","August", 0, QApplication::UnicodeUTF8) 	<<QApplication::translate("pCommon","September", 0, QApplication::UnicodeUTF8)
                        << QApplication::translate("pCommon","October", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","November", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","December", 0, QApplication::UnicodeUTF8);
    MonthsNameShort << QApplication::translate("pCommon","Jan", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Feb", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Mar", 0, QApplication::UnicodeUTF8)
                    << QApplication::translate("pCommon","Apr", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","May", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Jun", 0, QApplication::UnicodeUTF8)
                    << QApplication::translate("pCommon","Jul", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Aug", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Sep", 0, QApplication::UnicodeUTF8)
                    << QApplication::translate("pCommon","Oct", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Nov", 0, QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","Dec", 0, QApplication::UnicodeUTF8);
    MonthsNameOther << QApplication::translate("pCommon","January","alternative", QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","February","alternative", QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","March","alternative", QApplication::UnicodeUTF8)
                    << QApplication::translate("pCommon","April","alternative", QApplication::UnicodeUTF8)   <<QApplication::translate("pCommon","May","alternative", QApplication::UnicodeUTF8)      <<QApplication::translate("pCommon","June","alternative", QApplication::UnicodeUTF8)
                    << QApplication::translate("pCommon","July","alternative", QApplication::UnicodeUTF8)    <<QApplication::translate("pCommon","August","alternative", QApplication::UnicodeUTF8)   <<QApplication::translate("pCommon","September","alternative", QApplication::UnicodeUTF8)
                    << QApplication::translate("pCommon","October","alternative", QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","November","alternative", QApplication::UnicodeUTF8) <<QApplication::translate("pCommon","December","alternative", QApplication::UnicodeUTF8);
    QStringList MonthsName = MonthsNameFull;

    if (form == "short") MonthsName = MonthsNameShort;
    else if (form == "other") MonthsName = MonthsNameOther;
    if ((n > 0) && (n < 13))  ret = MonthsName[n-1];
    return ret;
}

//-------------------------------------------------
QString pMonthName(int n)
{
    return pMonthName(n, "full");
}

//-------------------------------------------------
// порядковый номер зодиакального зозвездия преобразуем в название
QString ZodiakName(int n)
{
    QString ret ="";
    QStringList  ZodiakNames; //{"Овен","Телец","Близнецы","Рак","Лев","Дева","Весы","Скорпион","Стрелец","Козерог","Водолей","Рыбы"};
    ZodiakNames << QApplication::translate("pCommon","Aries", 0, QApplication::UnicodeUTF8) 	  << QApplication::translate("pCommon","Taurus", 0, QApplication::UnicodeUTF8)
                << QApplication::translate("pCommon","Gemini", 0, QApplication::UnicodeUTF8) 	  << QApplication::translate("pCommon","Cancer", 0, QApplication::UnicodeUTF8)
                << QApplication::translate("pCommon","Leo", 0, QApplication::UnicodeUTF8) 		  << QApplication::translate("pCommon","Virgo", 0, QApplication::UnicodeUTF8)
                << QApplication::translate("pCommon","Libra", 0, QApplication::UnicodeUTF8) 	  << QApplication::translate("pCommon","Scorpius", 0, QApplication::UnicodeUTF8)
                << QApplication::translate("pCommon","Sagittarius", 0, QApplication::UnicodeUTF8) << QApplication::translate("pCommon","Capricorn", 0, QApplication::UnicodeUTF8)
                << QApplication::translate("pCommon","Aquarius", 0, QApplication::UnicodeUTF8)    << QApplication::translate("pCommon","Pisces", 0, QApplication::UnicodeUTF8);

    if ((n > 0) && (n < 13))  ret = ZodiakNames[n-1];
    return ret;
}

//-------------------------------------------------
int DaysInYear(long cYear){
    int days=365;
    if (cYear !=0){
        if ((cYear % 4) == 0) {days = 366;}  // it is a leap year
        if ((cYear % 100) == 0) {days =365;} // it is not
        if ((cYear % 400) == 0) {days =366;}  // still it is
    }
    return days;
}

//-------------------------------------------------
int DaysInYear_Jul(long cYear){
    int days=365;
    if (cYear !=0){
        if ((cYear % 4) == 0) {days = 366;}  // it is a leap year
    }
    return days;
}

//-------------------------------------------------
bool LeapYear(long cYear){
    bool IsLeap=false;
    if (DaysInYear(cYear)==366) IsLeap= true;
    return IsLeap;
}

//-------------------------------------------------
bool LeapYear_Jul(long cYear){
    bool IsLeap=false;
    if (DaysInYear_Jul(cYear)==366) IsLeap= true;
    return IsLeap;
}

//-------------------------------------------------
int HoursInYear(long cYear){
    int h;
    h = DaysInYear(cYear) * 24;
    return h;
}

//-------------------------------------------------
int MinutsInYear(long cYear){
    int m;
    m = DaysInYear(cYear) * 1440;
    return m;
}

//-------------------------------------------------
int SecondsInYear(long cYear){
    int s;
    s = DaysInYear(cYear) * 86400;
    return s;
}

//-------------------------------------------------
//сумирует цифры числа между собой до получения одной цифры
int Digit(int Number){
    int x,i;
    QString Str = QString::number(Number);

    x = 0;
    while (Str.length() > 1){
        for (i=1; i<=Str.length(); i++) {
            x += Str[i].digitValue();
        }
        Str = QString::number(x);
        x = 0;
    }
    x = Str.toInt();
    return x;
}

//-------------------------------------------------
//возвращает максимальный из аргументов
int max(int x1, int x2, int x3){
    int res;

    res = x1;
    if (x2 > res) res = x2;
    if (x3 > res) res = x3;
    return res;
}

//-------------------------------------------------
//true если все цифры числа одинаковы, напр. 777
bool SameDigits(int x){
    QString tmp,y1,y2;
    bool res;
    int i;

    tmp = QString::number(x);
    res = true;
    for (i=1; i < tmp.length(); i++){
        y1 = tmp[i];
        y2 = tmp[i+1];
        if (tmp[i] != tmp[i+1]) res = false;
    }

    return res;
}

//-------------------------------------------------
// вычисление пропорции  х - 100%
//                       y - res%       res = y * 100 / x;
double Proporsion(double x, double y){
    double res;

    res = y * 100 /x;

    return res;
}

//-------------------------------------------------
//возвращает букву из строки
QString IntToLetter(int n, QString str){
    QString ret = str.at(n-1); //-1 т.к. первая буква в строке имеет индекс = 0
    return ret;
}

//-------------------------------------------------
//возвращает букву латинского алфавита
QString IntToLetter(int n){
    return IntToLetter(n,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

//-------------------------------------------------
//возвращает номер буквы в указанном алфавите
int LetterToInt(QString letter, QString srcAlphabet)
{
    int ind = srcAlphabet.indexOf(letter.toUpper()) + 1; //+1 т.к. первая буква в строке имеет индекс = 0
    return ind;
}

//-------------------------------------------------
//возвращает номер буквы латинского алфавита
int LetterToInt(QString letter) 
{
    return LetterToInt(letter,"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}


//-------------------------------------------------
/* ?+? Процедуры для чисел > 26 (вернее больше числа букв в алфавите), так для латинского алфавита 27 = аа, 28 = ab 
QString IntToLetters(int n, QString str)
{
 QString retStr;
 int l = str.lenght();

 return retStr;
}

//-------------------------------------------------
QString IntToLetters(int n)
{
 return NumToStr(n, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

//-------------------------------------------------
int LettersToInt(QString str, QString sourceStr)
{
 int retInt;
 int r = str.lenght(); // разрядность = сколько букв
 for(int i=0; i<l; ++i){

 }
 return retInt;
}

//-------------------------------------------------
int LettersToInt(QString str)
{
 return LettersToInt(str, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}
*/

//-------------------------------------------------
/* Римские цифры
1 		I
5 		V
10 		X
50 		L
100 	C
500 	D
1000 	M
Мы Дарим Сочные Лимоны, Хватит Всем И ещё останется.
Соответственно M, D, C, L, X, V, I

Натуральные числа записываются при помощи повторения этих цифр. 
При этом, если большая цифра стоит перед меньшей, то они складываются (принцип сложения), 
если же меньшая — перед большей, то меньшая вычитается из большей (принцип вычитания).
Римские цифры предоставляют возможность записывать числа от 1 до 3999 (MMMCMXCIX). Для решения этой проблемы были созданы расширенные Римские цифры.
http://ru.wikipedia.org/ <- Римские цифры
*/
int r2i(QChar letter)
{
    int i=0;
    QChar c = letter.toUpper();

    if (c == 'I') i = 1;
    else if (c == 'V') i = 5;
    else if (c == 'X') i = 10;
    else if (c == 'L') i = 50;
    else if (c == 'C') i = 100;
    else if (c == 'D') i = 500;
    else if (c == 'M') i = 1000;

    return i;
}

//-------------------------------------------------
int RomanToInt(QString romanInt)
{
    int n = romanInt.length();
    int sum = 0;

    for(int i=0; i<n; ++i){
        if ( (r2i(romanInt[i+1]) > 0) && //проверяем не последняя ли это цифра, т.к. её вычитать не надо
             (r2i(romanInt[i]) < r2i(romanInt[i+1])) ) //проверяем на правило вычитания
            sum -= r2i(romanInt[i]);
        else
            sum += r2i(romanInt[i]);
    }
    return sum;
}

//-------------------------------------------------
QString IntToRoman(int n)
{
    QString roman = "";
    int x,y,i;
    if((n>0) && (n<4000)){   //Римские цифры предоставляют возможность записывать числа от 1 до 3999 (MMMCMXCIX)
        //тысячи
        x = n / 1000;
        y = n % 1000;
        for(i=1; i<=x; ++i)	roman = roman + "M";
        //сотни
        x = y / 100;
        y = y % 100;
        if(x==9) roman = roman + "CM";
        else if(x>4){
            roman = roman + "D";
            x -=5;
        }else if(x==4) roman = roman + "CD";
        if((x>0) && (x<4))
            for(i=1; i<=x; ++i)	roman = roman + "C";
        //десятки
        x = y / 10;
        y = y % 10;
        if(x==9) roman = roman + "XC";
        else if(x>4){
            roman = roman + "L";
            x -=5;
        }else if(x==4) roman = roman + "XL";
        if((x>0) && (x<4))
            for(i=1; i<=x; ++i)	roman = roman + "X";
        //единицы
        x = y;
        if(x==9) roman = roman + "IX";
        else if(x>4){
            roman = roman + "V";
            x -=5;
        }else if(x==4) roman = roman + "IV";
        if((x>0) && (x<4))
            for(i=1; i<=x; ++i)	roman = roman + "I";
    }
    return roman;
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
void replaceTextOfFile(QString filepath, QString beforetext, QString replacetext)
{
    QFile file(filepath);

    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.reset();

    QTextStream stream(&file);
    QStringList str;
    QString line;
    do
    {
        line = stream.readLine();
        line.replace(beforetext, replacetext);
        str.append(line);

    } while (!stream.atEnd());

    str.removeOne("");
//    qDebug() << "\nstrlist = " << str;
    file.close();
    file.remove();
    file.open(QIODevice::WriteOnly);

    QString writelist;
    for (int i = 0; i < str.size(); i++)
    {
        writelist.append(QString(str.at(i))+"\n");
    }

    if (Config::configuration()->Language() == "utf-8") file.write(writelist.toUtf8());
//    if (Config::configuration()->Language() == "Utf-16") file.write(writelist.toLocal8Bit());
//    if (Config::configuration()->Language() == "Utf-32") file.write(writelist.toUcs4().toStdVector());
}
//-------------------------------------------------
QString ist(QString str)
{
    if (str == "")
    {
        return "none";
    }
    return str;
}
//-------------------------------------------------
int BooltoInt(bool foo)
{
    if (foo == true)
        return 1;
    return 0;
}
//-------------------------------------------------
QString BooltoQString(bool foo)
{
    if (foo == true)
        return "Y";
    return "N";
}
//-------------------------------------------------
// юзать только для преобразования параметров проекта
bool QStringtoBool(QString str)
{
    if (str == "Y")
        return true;
    if (str == "0" or str.isEmpty() or str == " " or str == "N")
        return false;
    return true;
}
//-------------------------------------------------
QString incstr(QString str, int n, QString mychar)
{
    while (str.length() != n)
    {
        str = mychar + str;
    }
    return str;
}
//-------------------------------------------------
QString tagToQuestion(QString tag)
{
    tag = "<" + tag + ">";
    return tag.replace("<","?").replace(">","_.");
}
//-------------------------------------------------
QString questionToTag(QString question)
{
    question = "?" + question + "_.";
    if (question == "?p_." or question == "?h4_.")
    {
        return question.replace("?","\n<").replace("_.",">");
    }
    return question.replace("?","<").replace("_.",">");
}
//-----------------------------------------------------
QString editStringList(QString list, QStringList tags, bool f)
{
    for (int i = 0; i < tags.size(); i++)
    {
        if (f)
        {
            list.replace( QString("<"+tags.at(i)+">"), tagToQuestion(tags.at(i)) );
        }
        else
        {
            list.replace( QString("?"+tags.at(i)+"_."), questionToTag(tags.at(i)) );
        }

    }
    return list;
}
//-----------------------------------------------------
QString getCenterTag(QString str)
{
    QRegExp rxp("<p align=\"center\".*?>");

    str.remove(" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"")
            .replace(rxp, "<p><center>");
    return str;
}
//------------------------------------------------------
QString getHtmlCoolCode(QString strinput, QString i, QString mychapter, bool chap)
{
    QStringList strlist = strinput.split("\n");
    QString teststr = "";

    QRegExp title("<title>  [1]</title>");
    QRegExp title2("<title>[1]</title>");
    QRegExp rx("(<[^>]*>)");

    QRegExp rxp("(<[Pp].*?>)");
    QRegExp rxi("( [a-zA-Z:]+=)|(\"[^\"]*\"|'[^']*')");


    QStringList tags;

    tags << "p" << "i" << "b" << "u" << "br /" << "h4" << "/h4" << "pre" << "/pre" << "font" << "/font" << "sup" << "/sup" << "sub" << "/sub" << "center"
         << "/center" << "strong" << "/strong" << "em" << "/em" << "table" << "/table"
         << "tr" << "tr" << "/tr" << "td" << "td" << "/td" << "th" << "th" << "/th" << "hr /" ;/*<< "span"*/
            /*<< "/span"*/

    QString titlec = QString("<title>%1</title>").arg(incstr(i,GL_LengtItemString," "));
    QString titlec2 = QString("<title>%1</title>").arg(i);
    QString chapter = QString("\n?h4_." + mychapter +" %1?/h4_.").arg(incstr(i,GL_LengtItemString," ")); // не работает tr - почему?? заменить mtchapter на tr("Chapter")

    QString str;
    for (int i = 0; i < strlist.size(); i++)
    {
        str = strlist.at(i);

        // title and Chapter replace
        if (chap)
        {
        str.replace(titlec,chapter)
                .replace(titlec2,chapter);
        }
        else
        {
                str.remove(title)
                .remove(title2);
        }

        str.remove("p, li { white-space: pre-wrap; }")
                .replace("<P>","<p>")
                .remove(title);

        // переписать это убожество
        if (str.indexOf("<p align=\"center\"") >= 0)
        {
            str.replace("<p align=\"center\"","<center><p")
                    .replace("</p>","</center>");
        }

        str = getParseTagSpan(str, "vertical-align:super;", "<sup>");
        str = getParseTagSpan(str, "vertical-align:sub;"  , "<sub>");
        str = getParseTagSpan(str, "font-weight:600;", "<strong>");
        str = getParseTagSpan(str, "font-style:italic;", "<em>");
//        qDebug() << "Debug: getHtmlCoolCode" << " str = " << str;

        str.replace(rxp, "?p_.")
                .remove("</p>")
                .remove(rxi);

        str = editStringList(str, tags, true); // сохраняем нужные теги, заменой на ?tag_.
        str.remove(rx)
                .remove("")
                .remove("\n");
        str.replace("?p_.PathName","\nPathName")
                .replace("PathName", "\n\nPathName")
                .replace("FullName", "\nFullName")
                .replace("ShortName", "\nShortName")
                .replace("ChapterQty", "\nChapterQty");
        str = editStringList(str, tags, false); // возвращаем нужные теги


        if (!str.isEmpty())
        {
            teststr.append(str);
        }
    }

//    qDebug() << "\n\nDebug: _getHtmlCoolCode" << " teststr = " << teststr;

    return teststr;
}

QString getParseTagSpan(QString str, QString text, QString tag)
{
    QString span = "<span>";
    QString spanend = "</span>";
    QString ta = tag;
    QString tagend = ta.replace("<","</");

    while (str.indexOf(text) >= 0)
    {
        // определяем гле находится наша строка
        // определяем где находится следующая >
        // определяем где находится spanend
        // за > ставим tag
        // перед spanend ставим tagend

        int posOfOurLine = str.indexOf(text);
            str.replace(posOfOurLine, text.length(), "");
        int posOf = str.indexOf(">",posOfOurLine);
            str.replace(posOf,1,">"+tag);
        int posOfEnd = str.indexOf(spanend,posOf);
            str.replace(posOfEnd,spanend.length(),tagend+spanend);
//            qDebug() << "\nDebug: _getParseTag" << "str = " << str << "\nposOfOurLine = " << posOfOurLine << " posOf = " << posOf << " posOfEnd = " << posOfEnd;
    }
    return str;
}
