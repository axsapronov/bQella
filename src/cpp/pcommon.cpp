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
bool createFileText(QString fileName, QString text)
{
    bool ret = true;
    QFile file(fileName);
    if (!file.exists())
    {		//create file if it's not exist
        if (file.open(QIODevice::ReadWrite))
        {	//try to open or create file
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            ts << text << endl;
            file.close();
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
//-------------------------------------------------
void replaceTextOfFile(QString filepath, QString beforetext, QString replacetext)
{
    QFile file(filepath);

    QString encoding = Config::configuration ()->profile ()->props["defaultencoding"];
    QTextCodec * codec = getCodecOfEncoding(encoding);

    file.close();
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.reset();

    QTextStream stream(&file);
    stream.setCodec(codec);
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
    //    qDebug() << " filepath  = " << filepath;

    QString writelist;
    for (int i = 0; i < str.size(); i++)
    {
        writelist.append(QString(str.at(i))+"\n");
    }

    QTextStream st(&file);
    st.setCodec(codec);
    st << writelist;
}
//-------------------------------------------------
bool addToEndFile(QString fileName, QString text)
{
    if (QFile::exists(fileName))
    {
        QFile file(fileName);

        if (file.open(QIODevice::Append))
        {
            QTextStream stream(&file);
            stream << text;
            file.close();
            return true;
        }
    }
    return false;
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
    QString tag;
    for (int i = 0; i < tags.size(); i++)
    {
        tag = tags.at (i);
        if (tag.indexOf ("hr") != -1)
            tag = "hr /";


        QString untag = tag;
        if (tag.indexOf("br") != -1)
            untag = "br /";

        if (f)
        {
            list.replace( QString(checkTag (untag)), tagToQuestion(uncheckTag(tag)) );
            //                    qDebug() << " list = " << list << " tags,at i = " << tags.at (i) << " ?tag = " << tagToQuestion(uncheckTag(tags.at(i))) << " orig tag = " << checkTag (tags.at (i));
        }
        else
        {
            list.replace( QString("?"+uncheckTag (tag)+"_."), questionToTag(uncheckTag (tag) ));
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
QString getHtmlCoolCode(QString strinput, QString inumber, QString mychapter ,bool createcontent)
{
    QStringList strlist = strinput.split("\n");
    QString teststr = "";

    QRegExp title("<title>  [1]</title>");

    QRegExp rx("(<[^>]*>)");
    QRegExp rxp("(<[Pp].*?>)");
    QRegExp rxi("( [a-zA-Z:]+=)|(\"[^\"]*\"|'[^']*')");

    QStringList tags;
    tags << "p" << "i" << "/i" << "b" << "/b" << "h4" << "/h4" <<QString(Config::configuration() -> profile() -> props["htmlfilter"]).split (" ");

    QString titlec = QString("<title>%1</title>").arg(incstr(inumber,GL_LENGT_ITEM_STRING," "));
    QString titlec2 = QString("<title>%1</title>").arg(inumber);

    QString chapter;
    chapter = QString("\n?h4_." + mychapter +" %1?/h4_.").arg(incstr(inumber,GL_LENGT_ITEM_STRING," ")); // не работает tr - почему?? заменить mtchapter на tr("Chapter")

    QString str;
    int number = 0;
    for (int i = 0; i < strlist.size(); i++)
    {
        str = strlist.at(i);

        // title and Chapter replace
        str.replace(titlec,chapter)
                .replace(titlec2,chapter);

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
        str = getParseTagSpan(str, "font-family:'Courier New,courier';", "<pre>");

        str.replace(rxp, "?p_.")
                .remove("</p>")
                .remove(rxi);

        str = editStringList(str, tags, true); // сохраняем нужные теги, заменой на ?tag_.
        str.remove(rx)
                .remove("")
                .remove("\n");
        str.remove("Новый пункт")
                .remove("New item"); /// от куда эти хери вылезли? (к чему вообще тайтлы файлов)
        str.replace("?p_.PathName","\nPathName")
                .replace("PathName", "\n\nPathName")
                .replace("FullName", "\nFullName")
                .replace("ShortName", "\nShortName")
                .replace("ChapterQty", "\nChapterQty")
                .remove("Content = yes")
                .remove("Content = no");

        str = editStringList(str, tags, false); /// возвращаем нужные теги
        str.remove ("<span>>")
                .remove ("</span>");

        if (!str.isEmpty())
        {
            if (str.indexOf(mychapter) >= 0
                    && createcontent)
            {
                str.replace("</h4>","</a></h4>")
                        .replace("<h4>","<h4>"+QString("<a name=\"chapter"+inumber + "\">"));
            }

            if (Config::configuration()->AutoNumbers())
            {
                /// create autonumbers
                str.replace("<p>", QString("<p>%1 ").arg(number));
                number++;
            }
            teststr.append(str);
        }
    }
    return teststr;
}
//-----------------------------------------------------
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
//-----------------------------------------------------
int getDepthTreeWidgetItem(QTreeWidgetItem *item)
{
    int depth = 0;
    while(item != 0)
    {
        depth++;
        item = item->parent();
    }
    return depth;
}
//-----------------------------------------------------
QStringList getFillShortName()
{
    QTextCodec * codec = QTextCodec::codecForName("UTF-8"); // set encoding for progs
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);

    QStringList items;

    items << QString ("Бут. Бут 1M. 1M Буття Быт. Быт Бт. Бт Бытие Ge. Ge Gen. Gen Gn. Gn Genesis")
          << QString ("Вих. Вих 2М. 2М Вихід Исх. Исх Исход Ex. Ex Exo. Exo Exod. Exod Exodus")
          << QString ("Лев. Лев 3М. 3М Лв. Лв Левит Lev. Lev Le. Le Lv. Lv Levit. Levit Leviticus")
          << QString ("Чис. Чис 4М. 4М Чс. Чс Числ. Числ Числа Nu. Nu Num. Num Nm. Nm Numb. Numb Numbers")
          << QString ("Повт. Повт Повтор. Повтор Повторення 5М. 5М Втор. Втор Вт. Вт Втрзк. Втрзк Второзаконие De. De Deut. Deut Deu. Deu Dt. Dt  Deuteron. Deuteron Deuteronomy ")
          << QString ("Iс.Нав. Iсус Єг Єг. Иис.Нав. Иис.Нав Нав. Нав Иисус Навин Jos. Jos Josh. Josh Joshua")
          << QString ("Суд. Суд Суддiв Сд. Сд Судьи Jdg. Jdg Judg. Judg Judge. Judge Judges")
          << QString ("Рут. Рут Руф. Руф Рф. Рф Руфь Ru. Ru Ruth Rth. Rth Rt. Rt")
          << QString ("1Сам. 1Сам 1См 1См. 1С 1С. 1Царств. 1Царств 1Sa. 1Sa 1S. 1S 1Sam. 1Sam 1Sm. 1Sm 1Sml. 1Sml 1Samuel ")
          << QString ("2Сам. 2Сам 2См 2См. 2С 2С. 2Царств. 2Царств 2Sa. 2Sa 2S. 2S 2Sam. 2Sam 2Sm. 2Sm 2Sml. 2Sml 2Samuel")
          << QString ("1Цар. 1Цар 1Царiв 1Цр. 1Цр 1Ц. 1Ц 1Царей 3Цар. 3Цар 3Царiв 3Цр. 3Цр 3Ц. 3Ц 3Царств. 3Царств 1Ki. 1Ki 1K. 1K 1Kn. 1Kn 1Kg. 1Kg 1King. 1King 1Kng. 1Kng 1Kings")
          << QString ("2Цар. 2Цар 2Царiв 2Цр. 2Цр 2Ц. 2Ц 2Царей 4Цар. 4Цар 4Царiв 4Цр. 4Цр 4Ц. 4Ц 4Царств. 4Царств 2Ki. 2Ki 2K. 2K 2Kn. 2Kn 2Kg. 2Kg 2King. 2King 2Kng. 2Kng 2Kings")
          << QString ("1Хр. 1Хр 1Хрон. 1Хрон 1Хронiки 1Пар. 1Пар 1Пр. 1Пр 1Chr. 1Chr 1Ch. 1Ch 1Chron. 1Chron")
          << QString ("2Хр. 2Хр 2Хрон. 2Хрон 2Хронiки 2Пар. 2Пар 2Пр. 2Пр 2Chr. 2Chr 2Ch. 2Ch 2Chron. 2Chron")
          << QString ("Езд. Езд Ездр. Ездр Ез. Ез Ездра Ezr. Ezr Ezra")
          << QString ("Неем. Неем. Неемія Нм. Нм Неемия Ne. Ne Neh. Neh Nehem. Nehem Nehemiah")
          << QString ("Ест. Ест Естер Есф. Есф Ес. Ес Есфирь Esth. Esth Est. Est Esther")
          << QString ("Йов. Йов Иов. Иов Ив. Ив Job. Job Jb. Jb")
          << QString ("Пс. Пс Псалт. Псалт Псал. Псал Псл. Псл Псалми Псалом Псалтирь Псалмы Ps. Ps Psa. Psa Psal. Psal Psalm Psalms")
          << QString ("Пр. Пр Прип. Прип Приповiстi Прит. Прит Притч. Притч Притчи Притча Pr. Pr Prov. Prov Pro. Pro Proverb Proverbs")
          << QString ("Екл. Екл Ек. Ек Еккл. Еккл Екк. Екк Екклесиаст Ec. Ec Eccl. Eccl Ecc. Ecc Ecclesia. Ecclesia Ecclesiastes ")
          << QString ("Пiсн. Пiсн Пiсня Песн. Песн Пес. Пес Псн. Псн Песн.Песней Песни Song. Song Songs SS. SS Sol. Sol")
          << QString ("Iс. Iс Iсая Ис. Ис Иса. Иса Исаия Исайя Isa. Isa Is. Is Isaiah ")
          << QString ("Єр. Єр Єрем. Єрем Єремiя Иер. Иер Иерем. Иерем Иеремия Je. Je Jer. Jer Jerem. Jerem Jeremiah")
          << QString ("Плач. Плач Плч. Плч Пл. Пл Пл.Єрем. Пл.Єрем Пл.Єр. Пл.Єрем Пл.Иер. Пл.Иер Плач Иеремии La. La Lam. Lam Lament. Lament Lamentation Lamentations ")
          << QString ("Єз. Єз Єзек. Єзек Єзекiїль Иез. Иез Из. Из Иезек. Иезек Иезекииль Ez. Ez Eze. Eze Ezek. Ezek Ezekiel")
          << QString ("Дан. Дан Дн. Дн Днл. Днл Даниїл Даниил Da. Da Dan. Dan Daniel ")
          << QString ("Ос. Ос Осiя Осия Hos. Hos Ho. Ho Hosea")
          << QString ("Йоїл. Йоїл Иоил. Иоил Ил. Ил Иоиль Joel. Joel Joe. Joe")
          << QString ("Ам. Ам Амс. Амс Амос Am. Am Amos Amo. Amo ")
          << QString ("Овд. Овд Овдiй Авд. Авд Авдий Ob. Ob Obad. Obad. Obadiah Oba. Oba")
          << QString ("Йона. Йона Йон. Йон Ион. Ион. Иона Jon. Jon Jnh. Jnh. Jona. Jona Jonah")
          << QString ("Мих. Мих Мх. Мх Михей Mi. Mi Mic. Mic Micah")
          << QString ("Наум. Наум Na. Na Nah. Nah Nahum ")
          << QString ("Ав. Ав Авв. Авв Аввак. Аввак Аввакум Hab. Hab Habak. Habak Habakkuk")
          << QString ("Соф. Соф Софон. Софон Софонiя Софония Zeph. Zeph  Zep. Zep Zephaniah")
          << QString ("Ог. Ог Огiй Аг. Аг Агг. Агг Аггей Hag. Hag Haggai")
          << QString ("Зах. Зах Зхр. Зхр Захар. Захар Захарiя Захария Ze. Ze Zec. Zec Zech. Zech Zechariah")
          << QString ("Мал. Мал Малах. Малах Млх. Млх Малахия Mal. Mal Malachi")
          << QString ("Матв. Матв Мтв. Мтв Мв. Мв Матвiя Матвiї Матф. Матф Мтф. Мтф Мф. Мф Мт. Мт Матфея Матфей Мат Мат. Mt. Mt Ma. Ma Matt. Matt Mat. Mat Matthew")
          << QString ("Мар. Мар Мр. Мр. Марк. Марк Мрк. Мрк Марка Мк Мк. Мр Мр. Mk. Mk Mar. Mar Mr. Mr Mrk. Mrk Mark")
          << QString ("Лук. Лук Лк. Лк Лукa Луки Lk. Lk Lu. Lu Luk. Luk Luke")
          << QString ("Iван. Iван Iв. Iв Iн. Iн Iвн. Iвн Iвана Иоан. Иоан Ин. Ин Иоанн Иоанна Jn. Jn Jno. Jno Joh. Joh John")
          << QString ("Дiї. Дiї Деян. Деян Дея. Дея Д.А. Деяния Ac. Ac Act. Act Acts")
          << QString ("Рим. Рим Римл. Римл Римлян Римлянам Ro. Ro Rom. Rom Romans")
          << QString ("1Кор. 1Кор 1Коринтян. 1Коринтян 1Коринт. 1Коринт 1Коринф. 1Коринф 1Коринфянам 1Коринфянам 1Co. 1Co 1Cor. 1Cor 1Corinth. 1Corinth 1Corinthians")
          << QString ("2Кор. 2Кор 2Коринтян. 2Коринтян 2Коринт. 2Коринт 2Коринф. 2Коринф 2Коринфянам 2Коринфянам 2Co. 2Co 2Cor. 2Cor 2Corinth. 2Corinth 2Corinthians")
          << QString ("Гал. Гал Галат. Галат Галатiв Галатам Ga. Ga Gal. Gal Galat. Galat Galatians")
          << QString ("Еф. Еф Ефес. Ефес Ефесян Ефесянам Eph. Eph Ep. Ep Ephes. Ephes Ephesian")
          << QString ("Фил. Фил Флп. Флп Филип. Филип Филиппийцам Php. Php Ph. Ph Phil. Phil Phi. Phi. Philip. Philip Philippians")
          << QString ("Кол. Кол Колос. Колос Колоссян Колоссянам Col. Col Colos. Colos Colossians")
          << QString ("1Сол. 1Сол. 1Солун. 1Солун 1Солунян 1Фесс. 1Фесс 1Фес. 1Фес 1Фессалоникийцам 1Солунянам 1Th. 1Th 1Thes. 1Thes 1Thess. 1Thess 1Thessalonians")
          << QString ("2Сол. 2Сол 2Солун. 2Солун 2Солунян 2Фесс. 2Фесс 2Фес. 2Фес 2Фессалоникийцам 2Солунянам 2Th. 2Th 2Thes. 2Thes 2Thess. 2Thess 2Thessalonians")
          << QString ("1Тим. 1Тим  1Тимоф. 1Тимоф 1Тимофiю 1Тимофею 1Ti. 1Ti 1Tim. 1Tim 1Timothy")
          << QString ("2Тим. 2Тим 2Тимоф. 2Тимоф 2Тимофiю 2Тимофею 2Ti. 2Ti 2Tim. 2Tim 2Timothy")
          << QString ("Тит. Тит Титу Тита Tit. Tit Ti. Ti Titus")
          << QString ("Флм. Флм Филим. Филим Филимон. Филимон Филимона Филимону Phm. Phm Phile. Phile Phlm. Phlm Philemon")
          << QString ("Євр. Євр Євреїв Евр. Евр Евреям He. He Heb. Heb Hebr. Hebr Hebrews")
          << QString ("Як. Як Яков. Яков Якв. Якв Якова Иак. Иак Ик. Ик Иаков Иакова Jas. Jas Ja. Ja Jam. Jam Jms. Jms James")
          << QString ("Об. Об Об'явл. Об'явл Об'явлен. Об'явлен Об'явлення Откр. Откр Отк. Отк Откровен. Откровен Апок. Апок ")
          << QString ("2Ездр. 2Ездр 2Езд. 2Езд 2Ездра 2Ездры 2Ез 2Ез. 2Esdras 2Es. 2Es")
          << QString ("Тов. Тов Товит Tobit Tob. Tob")
          << QString ("Иудиф. Иудиф Иудифь Judith Jdt. Jdt")
          << QString ("Прем.Сол. Премудр.Соломон. Премудр.Сол. Премудр. Премудр.Соломона Премудрость Премудрости Прем. Прем Wisdom Wis. Wis")
          << QString ("Сир. Сир Сирах SirachSip Sirach SirSip Sir.Sip SirSip Sir. Sir")
          << QString ("Посл.Иер. Посл.Иер Посл.Иерем. Посл.Иерем Посл.Иеремии Ep.Jer. EpJer Epj. Epj")
          << QString ("Вар. Вар Варух Baruch Bar. Bar")
          << QString ("1Макк. 1Макк. 1Маккав. 1Маккав 1Мак. 1Мак 1Маккавейская 1Maccabees 1Macc. 1Macc 1Mac. 1Mac 1Ma. 1Ma")
          << QString ("2Макк. 2Макк. 2Маккав. 2Маккав 2Мак. 2Мак 2Маккавейская 2Maccabees 2Macc. 2Macc 2Mac. 2Mac 2Ma. 2Ma")
          << QString ("3Макк. 3Макк. 3Маккав. 3Маккав 3Мак. 3Мак 3Маккавейская 3Maccabees 3Macc. 3Macc 3Mac. 3Mac 3Ma. 3Ma")
          << QString ("3Ездр. 3Ездр 3Езд. 3Езд 3Ездра 3Ездры 3Ез 3Ез. 3Esdras 3Es. 3Es")
          << QString ("1Петр. 1Петр 1Пет. 1Пет 1Пт. 1Пт 1Птр. 1Птр 1Петра 1Pe. 1Pe 1Pet. 1Pet 1Peter")
          << QString ("2Петр. 2Петр 2Пет. 2Пет 2Пт. 2Пт 2Птр. 2Птр 2Петра 2Pe. 2Pe 2Pet. 2Pet 2Peter")
          << QString ("1Iван. 1Iван 1Iв. 1Iв 1Iн. 1Iн 1Iвн. 1Iвн 1Iвана 1Иоан. 1Иоан 1Ин. 1Ин 1Иоанн 1Иоанна 1Jn. 1Jn 1Jo. 1Jo 1Joh. 1Joh 1Jno. 1Jno 1John")
          << QString ("2Iван. 2Iван 2Iв. 2Iв 2Iн. 2Iн 2Iвн. 2Iвн 2Iвана 2Иоан. 2Иоан 2Ин. 2Ин 2Иоанн 2Иоанна 2Jn. 2Jn 2Jo. 2Jo 2Joh. 2Joh 2Jno. 2Jno 2John")
          << QString ("3Iван. 3Iван 3Iв. 3Iв 3Iн. 3Iн 3Iвн. 3Iвн 3Iвана 3Иоан. 3Иоан 3Ин. 3Ин 3Иоанн 3Иоанна 3Jn. 3Jn 3Jo. 3Jo 3Joh. 3Joh 3Jno. 3Jno 3John")
          << QString ("Юд. Юд Юди. Юди Юда. Юда Иуд. Иуд Ид. Ид Иуда Иуды Jud. Jud Jude Jd. Jd");
    return items;
}
//-----------------------------------------------------

QStringList getFillEncoding()
{
    QTextCodec * codec = QTextCodec::codecForName("UTF-8"); // set encoding for progs
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForTr(codec);

    QStringList items;

    items << QString("UTF-8")
          << QString("Windows-1251")
          << QString("KOI8-R")
          << QString("KOI8-U")
          << QString("UTF-16")
             //          << QString("UTF-16BE")
             //          << QString("UTF-16LE")
          << QString("UTF-32")
             //          << QString("UTF-32BE")
             //          << QString("UTF-32LE")
          << QString("Windows-1252")
          << QString("Windows-1253")
          << QString("Windows-1254")
          << QString("Windows-1255")
          << QString("Windows-1256")
          << QString("Windows-1257")
          << QString("Windows-1258");
    //            << tr("MuleLao-1")
    //            << tr("ROMAN8")
    //            << tr("Shift-JIS")
    //            << tr("TIS-620")
    //            << tr("TSCII")
    //            << tr("Apple Roman")
    //            << tr("Big5")
    //            << tr("Big5-HKSCS")
    //            << tr("CP949")
    //            << tr("EUC-JP")
    //            << tr("EUC-KR")
    //            << tr("GB18030-0")
    //            << tr("IBM 850")
    //            << tr("IBM 866")
    //            << tr("IBM 874")
    //            << tr("ISO 2022-JP")
    //            << tr("JIS X 0201")
    //            << tr("JIS X 0208")
    return items;

}

//-----------------------------------------------------
QString getShortName(QString filename)
{
    // translate to hindi
    QString str= "";
    QString line;
    QString shortma = "ShortName = ";
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream( &file );
        do {
            line = stream.readLine();
            if (line.indexOf("ShortName = ") >= 0)
            {
                //                str = getTextInStr(line);
                str = line.remove("ShortName = ");
            }
        } while (str.isEmpty() and !line.isNull());
        file.close();
    }
    return str;
}
//-----------------------------------------------------
QString miniparserini(QString str, QString po)
{
    po.append(" = ");
    //    qDebug() << "_str " << str;
    if (str.indexOf(po) >= 0)
    {
        str.remove(po);
        if (po == "BibleName = ")
        {
            str.replace(" ", "_");
            //                    .remove(str.length()-1, 1);
        }

        if (po != "ShortName = " and
                po != "FullName = " and
                po != "ChapterSign = ")
        {
            str.remove(" \0");
        }

        if (str == "")
        {
            //               qDebug() << "po = " << po;
            if ( po == "Language = ")
            {
                //                qDebug() << "po = " << po;
                return "rus";
            }
            if ( po == "DefaultEncoding = ")
                return "UTF-8";
            return "none";
        }
        str.remove("\n");

        //        qDebug() << "__str = " << str;
        return str;
    }
    return "";
}
//-----------------------------------------------------
QString getTextInStr(QString strf, int begin, int end)
{
    //    qDebug() << " str before = " << strf;
    //    QString str = strf.section(" = ", 3, 4);
    ////    str.remove(str.indexOf(" ChapterQty"),strf.length()-str.indexOf(" ChapterQty"));
    ////    str.remove

    //    qDebug() << " str = " << str;
    return strf;
    strf = strf;
    begin = begin;
    end = end;
}
//-----------------------------------------------------
QString replaceFullShortName(QString line, QString text, QString name)
{
    QString str;
    if (line.indexOf(name) >= 0)
    {
        str = QString(line).remove(name);
        if (str != text)
        {
            line = name + text;
        }
    }
    return line;
}
//-----------------------------------------------------
void writeQStringList(QString filename, QStringList list)
{
    QFile file(unurlifyFileName(filename));
    if (file.open(QIODevice::WriteOnly))
    {
        for (int i = 0; i < list.size(); i++)
            file.write(QString(list.at(i)+"\n").toUtf8());
    }
    //    qDebug() << " list = " << list;
}
//-----------------------------------------------------
QString getCheckShortNameForFile(QString str, QString full)
{
    // если нету в short неймах такого названия, то юзаем все полное название книги
    // если есть, то берем номер шорт нейма и называем файл его номером.
    QStringList list = getFillShortName();
    for (int i = 0; i < list.size(); i++)
    {
        if (str == list.at(i))
            return QString::number(i+1);
    }
    return full;
}
//-----------------------------------------------------
QString checkExistenceFile(QString filename)
{
    QString end = QString(filename).split(".").last();
    filename.remove("."+end);
    while (QFile::exists(filename+"."+end))
    {
        filename.append("_");
    }
    //    qDebug() << "filename = " << filename;
    return filename+"."+end;
}
//-------------------------------------------------------
QString checkTag(QString tag)
{
    //    if ((tag.at(0) != "<") and (tag.at(tag.length()) != ">"))
    if ((tag.indexOf("<", 0) == -1) and (tag.indexOf(">", tag.length()-2) == -1))
    {
        tag = "<" + tag + ">";
    }
    return tag;
}
//--------------------------------------------------------
QString uncheckTag(QString tag)
{
    return tag.remove ("<").remove (">");
}
//-------------------------------------------------------
QString checkEndTag(QString tag)
{
    tag = checkTag (tag);
    tag.replace ("<","</");
    return tag;
}
//-------------------------------------------------------
QString getFileNameAbs(QString file)
{
    QStringList list;
    list << file.split("/");
    QString str = list.last();
    list = str.split(".");
    str = list.first();

    return str;
}
//----------------------------------------------------
void removeStringInFile(QString filename, QStringList list)
{
    QFile file(filename);
    QString output = "";
    if (file.open(QIODevice::ReadOnly))
    {
        QString line;
        QTextStream stream(&file);
        do
        {
            line = stream.readLine();
            for (int i = 0; i < list.size(); i++)
            {
                if (line == list.at(i))
                {
                    line = "";
                }
            }

            if (!line.isEmpty())
            {
                output.append(line+"\n");
            }
        } while (!stream.atEnd());
    }
    file.close();
    file.remove();
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(output.toUtf8());
    }
}
//---------------------------------------------------
QString checkProcentRol(QString str, QString out, int procent)
{
    QStringList liststr = QString(str).split(" ");
    QStringList list = getFillShortName();

    //    for (int i = 0; i < liststr.size(); i++)
    //    {
    //        qDebug() << " listat = " << liststr.at(i);
    //        if (list.indexOf(liststr.at(i)) !=-1)
    //        {
    //            qDebug() << " find !" << " int i = " << i;
    //        }
    //    }
    //    qDebug() << "liststr = " << liststr;
    int pr = 0;
    //how  optimised?

    for (int j = 0; j < list.size(); j++)
    {
        pr = 0;
        QStringList listup = QString(list.at(j)).split(" ");
        //        qDebug() << "\nlistup = " << listup;
        for (int foo = 0; foo < listup.size(); foo++)
        {
            for (int i = 0; i < liststr.size(); i++)
            {
                //                qDebug() << " ttest[2]" << "list = " << listup.at(foo) << " listg =" << liststr.at(i);
                if (listup.at(foo) == liststr.at(i))
                {
                    pr++;
                    //                    qDebug() << "lsitatfoo = " <<  listup.at(foo) << " liststrati = " << liststr.at(i);
                }
            }
        }
        //        qDebug() << "role = "<< int(double(pr/listup.size())*100) << " procent = " << procent;
        double procentvalue = double(pr) / double(listup.size()) * 100;
        if ( pr != 0 )
        {
            //             qDebug() << " role = " << pr << " procent = " << procent << " lastyp.size = " << listup.size() << " testprocent = " << test;
        }
        if (procentvalue >= procent )
        {
            return QString::number(j+1);
        }

    }
    return out;

}
//-----------------------------------------
QTextCodec * getCodecOfEncoding(QString encoding)
{
    //    encoding = encoding.toUpper ();
    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    if (encoding.toUpper ()== "UTF-8")        codec = QTextCodec::codecForName("UTF-8");
    if (encoding.toUpper ()== "UTF-16")       codec = QTextCodec::codecForName("UTF-16");
    if (encoding.toUpper ()== "UTF-32")       codec = QTextCodec::codecForName("UTF-32");
    if (encoding == "Windows-1251") codec = QTextCodec::codecForName("Windows-1251");
    if (encoding == "Windows-1252") codec = QTextCodec::codecForName("Windows-1252");
    if (encoding == "Windows-1253") codec = QTextCodec::codecForName("Windows-1253");
    if (encoding == "Windows-1254") codec = QTextCodec::codecForName("Windows-1254");
    if (encoding == "Windows-1255") codec = QTextCodec::codecForName("Windows-1255");
    if (encoding == "Windows-1256") codec = QTextCodec::codecForName("Windows-1256");
    if (encoding == "Windows-1257") codec = QTextCodec::codecForName("Windows-1257");
    if (encoding == "Windows-1258") codec = QTextCodec::codecForName("Windows-1258");
    if (encoding.toUpper ()== "KOI8-R")       codec = QTextCodec::codecForName("KOI8-R");
    if (encoding.toUpper ()== "KOI8-U")       codec = QTextCodec::codecForName("KOI8-U");

    //    qDebug() << " encoding = " << encoding;
    return codec;
}
//---------------------------------------------------
QString removeFirst(QString str, QString remove)
{
    int pos = str.indexOf(remove);
    if (pos != -1)
    {
        str.remove(pos, remove.length());
    }
    return str;
}
//----------------------------------------------------
QString getParamBook(QString filename, QString param)
{
    // translate to hindi
    QString str= "";
    QString line;
    QString parama = param + " = ";
    QFile file(filename);
    file.close();
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream( &file );
        do {
            line = stream.readLine();
            if (line.indexOf(parama) >= 0)
            {
                str = line.remove(parama);
            }
        } while (str.isEmpty() and !line.isNull());
        file.close();
    }
    else
    {
        qDebug() << "Error";
    }
    return str;
}
//----------------------------------------------------
void visitTree(QStringList &list, QTreeWidgetItem *item)
{
    list << item->text(0);
    for(int i = 0; i < item->childCount(); ++i)
        visitTree(list, item->child(i));
}
//----------------------------------------------------
QStringList visitTree(QTreeWidget *tree)
{
    QStringList list;
    for(int i = 0; i < tree->topLevelItemCount(); ++i)
        visitTree(list, tree->topLevelItem( i ));
    return list;
}
//----------------------------------------------------
QStringList getParentText(QTreeWidgetItem *item)
{
    QStringList list;
    for(int i = 0; i < item->childCount(); ++i)
        visitTree(list, item->child(i));
    return list;
}
//----------------------------------------------------
bool checkFileContainsText(QString filename, QString text)
{
    QFile file(filename);

    QString line;
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream( &file );
        line = stream.readAll();
        if (line.indexOf(text) > -1)
        {
            file.close();
            return true;
        }
        file.close();
    }
    return false;
}
//------------------------------------------------------------
QString getInfoFromStrongFile(QString horg, QString number)
{
    QString filename = getFileNameOfStrong(horg, number);
    // возвращает данные о стронге из файла
    qDebug() << filename;
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
                if (line.indexOf(number) >= 0)
                {
                    line = stream.readLine();
                    line = "<b>" + line + "</b>";
                    line.append(stream.readLine());
                    line.replace("\t<option2>","\n\r<br>");
                    line.remove("\t<option1>")
                            .remove("</option1>")
                            .remove("</option2>");
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
//----------------------------------------------------------
QString getFileNameOfStrong(QString horg, QString numberstr)
{
    int number = numberstr.toInt();
    QString numberfile = QString::number(number/100);
    numberfile = incstr(numberfile, 2, "0");
    QString filename =
            Config::configuration()->AppDir() +
            "strong/" +
            horg + "/" +
            horg + numberfile + ".htm";
    return filename;
}
