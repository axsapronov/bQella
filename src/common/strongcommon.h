#ifndef __STRONGCOMMON_H
#define __STRONGCOMMON_H


#include <QString>

/// strong
/**
  @function
  Return info from number
  (filename from getFileNameOfStrong function)
  @param filepath QString of file path
  @param number  QString (int) number
  @return info  QString
  */
QString getInfoFromStrongFile(QString filename, QString number); // возвращает данные о стронге из файла

/**
  @function
  Return path of filename
  @param QString  hebrew or greek (bool :-D)
  @param number  QString
  @return filepath QString
  */
QString getFileNameOfStrong(QString horg, QString numberstr); // возвращает название файла. ПРинимает греческй или иврит стронг и номер стронга


#endif // __STRONGCOMMON_H
