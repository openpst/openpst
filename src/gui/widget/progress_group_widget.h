/**
* LICENSE PLACEHOLDER
*
* @file 
* @class 
* @package OpenPST
* @brief 
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WIDGET_PROGRESS_GROUP_WIDGET_H
#define _GUI_WIDGET_PROGRESS_GROUP_WIDGET_H

#include <QGroupBox>
#include <QPushButton>


namespace Ui {
	class ProgressGroupWidget;
}

namespace OpenPST{ 
	namespace GUI{
		class ProgressGroupWidget : public QGroupBox
		{

			Q_OBJECT

			public:
				QPushButton* cancelButton;
				QPushButton* cancelAllButton;

				ProgressGroupWidget(QWidget *parent = 0);
				~ProgressGroupWidget();
				void setProgress(int value);
				void setProgress(int min, int max, int value);
				void setTaskCount(int count);
				void setTextLeft(QString text);
				void setTextRight(QString text);
				void enableCancel();
				void disableCancel();		
				void enableCancelAll();
				void disableCancelAll();
				void reset();


			private:
				Ui::ProgressGroupWidget *ui;


        };
	}
}
#endif // _GUI_WIDGET_PROGRESS_GROUP_WIDGET_H
