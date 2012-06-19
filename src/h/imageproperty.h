#ifndef __IMAGEPROPERTY_H__
#define __IMAGEPROPERTY_H__

#include "ui_imageproperty.h"

//===================== class ImageProperties ===========================
class ImageProperties : public QDialog
{
     Q_OBJECT

public:
	ImageProperties(QWidget *parent = 0);
	void setProperties(int height, int width, QImage image);
	
public slots:
	void LoadImage();

signals:	
	void newImage(QString html);
	void updateImage(QString html);
	//void newImage(int height, int width, QImage image);
	//void updateImage(int height, int width, QImage image);

private slots:
	void accept();
	void reject();
	void showSizeOriginal(bool state);
	void showSizeCustom(bool state);
	void setHeight(int y);
	void setWidth(int x);
private:
	void previewImage();
	
	Ui::DialogImageProperty ui;
	bool modeNewImage;
	QImage img;
	QString previewImageFN, projectImageFN, strHtml;
};



#endif // __IMAGEPROPERTY_H__
