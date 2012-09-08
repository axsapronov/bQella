#include <QFileDialog>
#include <QDebug>

#include "imageproperty.h"
#include "config.h"
#include "pcommon.h"
#include "filecommon.h"

//===================== class ImageProperties ===========================

ImageProperties::ImageProperties(QWidget *parent)
	 : QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.PBOpen, SIGNAL(clicked()), this, SLOT(LoadImage()));
	connect(ui.RBSizeOriginal, SIGNAL(clicked(bool)), this, SLOT(showSizeOriginal(bool)));
	connect(ui.RBSizeCustom, SIGNAL(clicked(bool)), this, SLOT(showSizeCustom(bool)));
	connect(ui.SBHeight, SIGNAL(valueChanged(int)), this, SLOT(setHeight(int)));
	connect(ui.SBWidth, SIGNAL(valueChanged(int)), this, SLOT(setWidth(int)));

    ui.GBText->setVisible(false);
    ui.GBSize->setVisible(false);
}

//------------------------------------------------------------------------------
void ImageProperties::reject()
{
        QWidget::hide();  //close dialog
}

//------------------------------------------------------------------------------
void ImageProperties::accept()
{
	//!+! copy image to prj image dir. See HelpWindow::htmlInsert(QString html) to decide where to do this.
	//QString src = copyResource(ui.LEFileName->text());
	if (modeNewImage)
		emit newImage(strHtml);
		//emit newImage(ui.SBHeight->value(), ui.SBWidth->value(), img);
	else
		emit updateImage(strHtml);
		//emit updateImage(ui.SBHeight->value(), ui.SBWidth->value(), img);
	QWidget::hide();  //close dialog
}

//------------------------------------------------------------------------------
void ImageProperties::setProperties(int height, int width, QImage image)
{
	ui.SBHeight->setValue(height);
	ui.SBWidth->setValue(width);
	if (image.isNull()){
		modeNewImage = true;
	}else{
		modeNewImage = false;
		img = image;
		//imgFN = img.fileName;
		//ui.LEFileName->setText(imgFN);
		ui.LEAltText->setText(img.text("Description"));
	}
}

//------------------------------------------------------------------------------
void ImageProperties::LoadImage()
{
	QString path = Config::configuration()->CurPrjImgDir();
	if (!ui.LEFileName->text().isEmpty()){
		QDir dir(ui.LEFileName->text());
		QString prevDir = dir.absolutePath();
		if (dir.exists(prevDir)) path = prevDir;
	}
	QString fn = QFileDialog::getOpenFileName(this, tr("Load Image..."),
							 path , tr("Images (*.png *.jpg *.gif);;All Files (*)"));
	QFileInfo fi(fn);
	if (fi.exists()){
		ui.LEFileName->setText(fn);
		img.load(fn);
		ui.SBHeight->setValue( img.height() );
		ui.SBWidth->setValue( img.width() );
		previewImageFN = relatifyFileName(fn, Config::configuration()->AppDir());
		projectImageFN = relatifyFileName(fn, Config::configuration()->CurPrjDir());
		ui.PTEImageInfo->clear();
		ui.PTEImageInfo->appendPlainText(tr("Image info:"));
		ui.PTEImageInfo->appendPlainText(tr("Height: %1 pixels","Image info. Prepend some spaces to align labels to the right").arg(img.height()));
		ui.PTEImageInfo->appendPlainText(tr(" Width: %1 pixels","Image info, align labels to the right").arg(img.width()));
		ui.PTEImageInfo->appendPlainText(tr(" Depth: %1 bits","Image info, align labels to the right").arg(img.depth()));
		ui.PTEImageInfo->appendPlainText(tr("  Size: %1 bytes","Image info, align labels to the right").arg(fi.size()));
		//for debug. See html code
		//ui.PTEImageInfo->appendPlainText(tr("dotsPerMeterXY: %1 x %2").arg(img.dotsPerMeterX()).arg(img.dotsPerMeterY()) );
		QStringList keys = img.textKeys();
		keys.removeAll("");
		if (keys.count() > 0){
			ui.PTEImageInfo->appendPlainText(tr("\nEmbeded text: %1, 0=%2").arg(keys.count()).arg(keys[0]));
			QStringList::iterator it = keys.begin();
			for (; it != keys.end(); ++it) {
				ui.PTEImageInfo->appendPlainText(*it + ": " + img.text(*it));
			}
		}
		previewImage();
	}
}

//------------------------------------------------------------------------------
void ImageProperties::previewImage()
{
	QString s = "<img";
	if ( !ui.LEPopUpHint->text().isEmpty() )
		s = s + " title=\"" + ui.LEPopUpHint->text() + "\"";
	if ( !ui.LEAltText->text().isEmpty() )
		s = s + " alt=\"" + ui.LEAltText->text() + "\"";
	if ( ui.RBSizeCustom->isChecked() ){
		s = s + QString(" style=\"width:%1px; height:%2px;\"").arg(ui.SBWidth->value()).arg(ui.SBHeight->value());
	//}else{
		//s = s + QString(" style=\"width: %1px; height: %2px;\"").arg(img.width()).arg(img.height());
	}
	//s = s + " align=\"middle\""
	strHtml = s + " src=\"" + projectImageFN +"\">";
	s       = s + " src=\"" + previewImageFN +"\">";
	ui.TEImagePreview->clear();
	ui.TEImagePreview->insertHtml(s);
	// for debug
    //ui.PTEImageInfo->appendPlainText(s);
    //ui.PTEImageInfo->appendPlainText(strHtml);
}

//------------------------------------------------------------------------------
void ImageProperties::showSizeOriginal(bool state)
{
	showSizeCustom(!state);
	previewImage();
}

//------------------------------------------------------------------------------
void ImageProperties::showSizeCustom(bool state)
{
	ui.CBKeepRatio->setEnabled(state);
	ui.LHeight->setEnabled(state);
	ui.LWidth->setEnabled(state);
	ui.SBHeight->setEnabled(state);
	ui.SBWidth->setEnabled(state);
	previewImage();
}

//------------------------------------------------------------------------------
void ImageProperties::setHeight(int y)
{
	ui.SBHeight->setValue(y);
	if (ui.CBKeepRatio->isChecked()){
		QImage tmpImage = img.scaledToHeight(y, Qt::FastTransformation);
		ui.SBWidth->setValue(tmpImage.width());
	}
	previewImage();
}

//------------------------------------------------------------------------------
void ImageProperties::setWidth(int x)
{
	ui.SBWidth->setValue(x);
	if (ui.CBKeepRatio->isChecked()){
		QImage tmpImage = img.scaledToWidth(x, Qt::FastTransformation);
		ui.SBHeight->setValue(tmpImage.height());
	}
	previewImage();
}


