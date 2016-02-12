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
#ifndef _GUI_WIDGET_LOG_WIDGET_H
#define _GUI_WIDGET_LOG_WIDGET_H

#include <QGroupBox>
#include <QFileDialog>
#include <fstream>

namespace Ui {
	class LogWidget;
}

namespace OpenPST{ 
	namespace GUI{
		class LogWidget : public QGroupBox
		{

			Q_OBJECT

			public:
	
				LogWidget(QWidget *parent = 0);
				~LogWidget();


				void log(const char* message);
				void log(std::string message);
				void log(QString message);

			public slots:
				void clear();
				void save();

			private:
				Ui::LogWidget *ui;


        };
	}
}
#endif // _GUI_WIDGET_LOG_WIDGET_H
