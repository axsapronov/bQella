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

#include "bookadddialog.h"
#include "ui_bookadddialog.h"

#include <QTextCodec>
#include <QStringListModel>
#include <QDebug>

BookAddDialog::BookAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookAddDialog)
{
    bookFullName = "";
    bookShortName = "";
    bookChapterQty = 0;
    bookCheckAutoChapterCreate = false;

    QTextCodec *codec = QTextCodec::codecForName("UTF8");  // назначаем кодировку для всех надписей
    QTextCodec::setCodecForTr(codec); //устанавливаем кодек

    ui -> setupUi(this);

    QStringList items;
    items << tr("Быт. Быт Бт. Бт Бытие Ge. Ge Gen. Gen Gn. Gn Genesis")
          << tr("Исх. Исх Исход Ex. Ex Exo. Exo Exod. Exod Exodus")
          << tr("Лев. Лев Лв. Лв Левит Lev. Lev Le. Le Lv. Lv Levit. Levit Leviticus")
          << tr("Чис. Чис Чс. Чс Числ. Числ Числа Nu. Nu Num. Num Nm. Nm Numb. Numb Numbers")
          << tr("Втор. Втор Вт. Вт Втрзк. Втрзк Второзаконие De. De Deut. Deut Deu. Deu Dt. Dt Deuteron. Deuteron Deuteronomy")
          << tr("Иис.Нав. Иис.Нав Нав. Нав Иисус Навин Jos. Jos Josh. Josh Joshua")
          << tr("Суд. Суд Сд. Сд Судьи Jdg. Jdg Judg. Judg Judge. Judge Judges")
          << tr("Руф. Руф Рф. Рф Руфь Ru. Ru Ruth Rth. Rth Rt. Rt")
          << tr("1Цар. 1Цар 1Цр. 1Цр 1Ц 1Царств. 1Царств 1Sa. 1Sa 1S. 1S 1Sam. 1Sam 1Sm. 1Sm 1Sml. 1Sml 1Samuel")
          << tr("2Цар. 2Цар 2Цр. 2Цр 2Ц 2Царств. 2Царств 2Sa. 2Sa 2S. 2S 2Sam. 2Sam 2Sm. 2Sm 2Sml. 2Sml 2Samuel")
          << tr("3Цар. 3Цар 3Цр. 3Цр 3Ц 3Царств. 3Царств 1Ki. 1Ki 1K. 1K 1Kn. 1Kn 1Kg. 1Kg 1King. 1King 1Kng. 1Kng 1Kings")
          << tr("4Цар. 4Цар 4Цр. 4Цр 4Ц 4Царств. 4Царств 2Ki. 2Ki 2K. 2K 2Kn. 2Kn 2Kg. 2Kg 2King. 2King 2Kng. 2Kng 2Kings")
          << tr("1Пар. 1Пар 1Пр. 1Пр 1Chr. 1Chr 1Ch. 1Ch 1Chron. 1Chron")
          << tr("2Пар. 2Пар 2Пр. 2Пр 2Chr. 2Chr 2Ch. 2Ch 2Chron. 2Chron")
          << tr("Ездр. Ездр Езд. Езд Ез. Ез Ездра Ezr. Ezr Ezra")
          << tr("Неем. Неем. Нм. Нм Неемия Ne. Ne Neh. Neh Nehem. Nehem Nehemiah")
          << tr("Есф. Есф Ес. Ес Есфирь Esth. Esth Est. Est Esther")
          << tr("Иов. Иов Ив. Ив Job. Job Jb. Jb")
          << tr("Пс. Пс Псалт. Псалт Псал. Псал Псл. Псл Псалом Псалтирь Псалмы Ps. Ps Psa. Psa Psal. Psal Psalm Psalms")
          << tr("Прит. Прит Притч. Притч Пр. Пр Притчи Притча Pr. Pr Prov. Prov Pro. Pro Proverb Proverbs")
          << tr("Еккл. Еккл Ек. Ек Екк. Екк Екклесиаст Ec. Ec Eccl. Eccl Ecc. Ecc Ecclesia. Ecclesia")
          << tr("Песн. Песн Пес. Пес Псн. Псн Песн.Песней Песни Song. Song Songs SS. SS Sol. Sol")
          << tr("Ис. Ис Иса. Иса Исаия Исайя Isa. Isa Is. Is Isaiah")
          << tr("Иер. Иер Иерем. Иерем Иеремия Je. Je Jer. Jer Jerem. Jerem Jeremiah")
          << tr("Плач. Плач Плч. Плч Пл. Пл Пл.Иер. Пл.Иер Плач Иеремии La. La Lam. Lam Lament. Lament Lamentation")
          << tr("Иез. Иез Из. Из Иезек. Иезек Иезекииль Ez. Ez Eze. Eze Ezek. Ezek Ezekiel")
          << tr("Дан. Дан Дн. Дн Днл. Днл Даниил Da. Da Dan. Dan Daniel")
          << tr("Ос. Ос Осия Hos. Hos Ho. Ho Hosea")
          << tr("Иоил. Иоил Ил. Ил Иоиль Joel. Joel Joe. Joe")
          << tr("Ам. Ам Амс. Амс Амос Am. Am Amos Amo. Amo")
          << tr("Авд. Авд Авдий Ob. Ob Obad. Obad. Obadiah Oba. Oba")
          << tr("Ион. Ион. Иона Jon. Jon Jnh. Jnh. Jona. Jona Jonah")
          << tr("Мих. Мих Мх. Мх Михей Mi. Mi Mic. Mic Micah")
          << tr("Наум. Наум Na. Na Nah. Nah Nahum")
          << tr("Авв. Авв Аввак. Аввак Аввакум Hab. Hab Habak. Habak Habakkuk")
          << tr("Соф. Соф Софон. Софон Софония Zeph. Zeph Zep. Zep Zephaniah")
          << tr("Агг. Агг Аггей Hag. Hag Haggai")
          << tr("Зах. Зах Зхр. Зхр Захар. Захар Захария Ze. Ze Zec. Zec Zech. Zech Zechariah")
          << tr("Мал. Мал Малах. Малах Млх. Млх Малахия Mal. Mal Malachi")
          << tr("Матф. Матф Мтф. Мтф Мф. Мф Мт. Мт Матфея Матфей Мат Мат. Mt. Mt Ma. Ma Matt. Matt Mat. Mat Matthew")
          << tr("Мар. Мар Марк. Марк Мрк. Мрк Мр. Мр Марка Мк Мк. Mk. Mk Mar. Mar Mr. Mr Mrk. Mrk Mark")
          << tr("Лук. Лук Лк. Лк Лукa Луки Lk. Lk Lu. Lu Luk. Luk Luke")
          << tr("Иоан. Иоан Ин. Ин Иоанн Иоанна Jn. Jn Jno. Jno Joh. Joh John")
          << tr("Деян. Деян Дея. Дея Д.А. Деяния Ac. Ac Act. Act Acts")
          << tr("Иак. Иак Ик. Ик Иаков Иакова Jas. Jas Ja. Ja Jam. Jam Jms. Jms James")
          << tr("1Пет. 1Пет 1Пт. 1Пт 1Птр. 1Птр 1Петр. 1Петр 1Петра 1Pe. 1Pe 1Pet. 1Pet 1Peter")
          << tr("2Пет. 2Пет 2Пт. 2Пт 2Птр. 2Птр 2Петр. 2Петр 2Петра 2Pe. 2Pe 2Pet. 2Pet 2Peter")
          << tr("1Иоан. 1Иоан 1Ин. 1Ин 1Иоанн 1Иоанна 1Jn. 1Jn 1Jo. 1Jo 1Joh. 1Joh 1Jno. 1Jno 1John")
          << tr("2Иоан. 2Иоан 2Ин. 2Ин 2Иоанн 2Иоанна 2Jn. 2Jn 2Jo. 2Jo 2Joh. 2Joh 2Jno. 2Jno 2John")
          << tr("3Иоан. 3Иоан 3Ин. 3Ин 3Иоанн 3Иоанна 3Jn. 3Jn 3Jo. 3Jo 3Joh. 3Joh 3Jno. 3Jno 3John")
          << tr("Иуд. Иуд Ид. Ид Иуда Иуды Jud. Jud Jude Jd. Jd")
          << tr("Рим. Рим Римл. Римл Римлянам Ro. Ro Rom. Rom Romans")
          << tr("1Кор. 1Кор 1Коринф. 1Коринф 1Коринфянам 1Коринфянам 1Co. 1Co 1Cor. 1Cor 1Corinth. 1Corinth 1Corinthians")
          << tr("2Кор. 2Кор 2Коринф. 2Коринф 2Коринфянам 2Коринфянам 2Co. 2Co 2Cor. 2Cor 2Corinth. 2Corinth 2Corinthians")
          << tr("Гал. Гал Галат. Галат Галатам Ga. Ga Gal. Gal Galat. Galat Galatians")
          << tr("Еф. Еф Ефес. Ефес Ефесянам Eph. Eph Ep. Ep Ephes. Ephes Ephesians")
          << tr("Фил. Фил Флп. Флп Филип. Филип Филиппийцам Php. Php Ph. Ph Phil. Phil Phi. Phi. Philip. Philip Philippians")
          << tr("Кол. Кол Колос. Колос Колоссянам Col. Col Colos. Colos Colossians")
          << tr("1Фесс. 1Фесс 1Фес. 1Фес 1Фессалоникийцам 1Сол. 1Сол 1Солунянам 1Th. 1Th 1Thes. 1Thes 1Thess. 1Thess")
          << tr("2Фесс. 2Фесс 2Фес. 2Фес 2Фессалоникийцам 2Сол. 2Сол 2Солунянам 2Th. 2Th 2Thes. 2Thes 2Thess. 2Thess")
          << tr("1Тим. 1Тим 1Тимоф. 1Тимоф 1Тимофею 1Ti. 1Ti 1Tim. 1Tim 1Timothy")
          << tr("2Тим. 2Тим 2Тимоф. 2Тимоф 2Тимофею 2Ti. 2Ti 2Tim. 2Tim 2Timothy")
          << tr("Тит. Тит Титу Tit. Tit Ti. Ti Titus")
          << tr("Флм. Флм Филимон. Филимон Филимону Phm. Phm Phile. Phile Phlm. Phlm Philemon")
          << tr("Евр. Евр Евреям He. He Heb. Heb Hebr. Hebr Hebrews")
          << tr("Откр. Откр Отк. Отк Откровен. Откровен Апок. Апок Откровение Апокалипсис Rev. Rev Re. Re Rv. Rv Revelation")
          << tr("2Ездр. 2Ездр 2Езд. 2Езд 2Ездра 2Ездры 2Ез 2Ез. 2Esdras 2Es. 2Es")
          << tr("Тов. Тов Товит Tobit Tob. Tob")
          << tr("Иудиф. Иудиф Иудифь Judith Jdt. Jdt")
          << tr("Прем.Сол. Премудр.Соломон. Премудр.Сол. Премудр. Премудр.Соломона Премудрость Премудрости Прем. Прем Wisdom Wis. Wis")
          << tr("Сир. Сир Сирах SirachSip Sirach SirSip Sir.Sip SirSip Sir. Sir")
          << tr("Посл.Иер. Посл.Иер Посл.Иерем. Посл.Иерем Посл.Иеремии Ep.Jer. EpJer Epj. Epj")
          << tr("Вар. Вар Варух Baruch Bar. Bar")
          << tr("1Макк. 1Макк. 1Маккав. 1Маккав 1Мак. 1Мак 1Маккавейская 1Maccabees 1Macc. 1Macc 1Mac. 1Mac 1Ma. 1Ma")
          << tr("2Макк. 2Макк. 2Маккав. 2Маккав 2Мак. 2Мак 2Маккавейская 2Maccabees 2Macc. 2Macc 2Mac. 2Mac 2Ma. 2Ma")
          << tr("3Макк. 3Макк. 3Маккав. 3Маккав 3Мак. 3Мак 3Маккавейская 3Maccabees 3Macc. 3Macc 3Mac. 3Mac 3Ma. 3Ma")
          << tr("3Ездр. 3Ездр 3Езд. 3Езд 3Ездра 3Ездры 3Ез 3Ез. 3Esdras 3Es. 3Es");


    typeModel = new QStringListModel(items, this);
    ui -> comboBoxShortName -> setModel(typeModel);


    connect(ui -> buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(clickslot(QAbstractButton *)));
    connect(ui -> buttonBox, SIGNAL(accepted()), this, SIGNAL(signalbookaddChanged()));
}

BookAddDialog::~BookAddDialog()
{
    delete ui;
}


void BookAddDialog::reject()
{
    QWidget::hide();  //close dialog
}

void BookAddDialog::clickslot(QAbstractButton *AButton)
{
    if (ui -> buttonBox -> standardButton(AButton) == QDialogButtonBox::Cancel)
    {
        reject();
    }

    if (ui -> buttonBox -> standardButton(AButton) == QDialogButtonBox::Ok)
    {
        bookFullNameWithSpace = ui->lineEditFullName->text();
        QString str = ui->lineEditFullName->text();
                str.replace(" ","_")
                .remove(str.length(),1);
//        qDebug() << " str = " << str << " strreplace " << str.replace(" ", "_") << " strremove = " << str.remove(str.length()-1,1);
        ui->lineEditFullName->setText(str);
        send();
        reject();
    }
}


void BookAddDialog::send()
{
    bookChapterQty = ui -> spinBoxChapterQty -> value();
    bookFullName = ui -> lineEditFullName -> text();
    bookShortName = ui -> comboBoxShortName -> currentText();
    bookCheckAutoChapterCreate = ui -> checkBoxAutoCreateChapter -> isChecked();
}
