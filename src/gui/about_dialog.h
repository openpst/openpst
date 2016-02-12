/**
* LICENSE PLACEHOLDER
*
* @file about_dialog.h
* @class AboutDialog
* @package OpenPST
* @brief Shared program about dialog
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_ABOUT_WINDOW_H
#define _GUI_ABOUT_WINDOW_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

namespace OpenPST{
	namespace GUI {

		class AboutDialog : public QDialog
		{

			Q_OBJECT

			public:
				explicit AboutDialog(QWidget *parent = 0);
				~AboutDialog();


			public slots:

			private:
				Ui::AboutDialog* ui;

			};
	}
}
#endif // _GUI_ABOUT_WINDOW_H
