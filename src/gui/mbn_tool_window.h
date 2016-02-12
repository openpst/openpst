/**
* LICENSE PLACEHOLDER
*
* @file mbn_tool_window.h
* @class MbnToolWindow
* @package OpenPST
* @brief MBN Tool GUI interface class definitions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _GUI_MBNTOOLWINDOW_H
#define _GUI_MBNTOOLWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "ui_mbn_tool_window.h"
#include "qc/mbn.h"
#include "util/hexdump.h"
#include "util/endian.h"
#include <fstream>
#include <ostream>

namespace Ui {
class MbnToolWindow;
}

namespace OpenPST{
	namespace GUI {
	class MbnToolWindow : public QMainWindow
	{
		Q_OBJECT

		public:
			explicit MbnToolWindow(QWidget *parent = 0);
			~MbnToolWindow();

			public slots:
			void browseForFile();
			void loadFile();
			void readX509Chain();
			void readSignature();
			void readCode();

		private:
			Ui::MbnToolWindow *ui;
			void log(const char* message);
			void log(std::string message);
			void log(QString message);
			void extractSegment(int segment);
			bool isValidHeader(uint8_t* data);
			bool isEightyByteHeader(uint8_t* data);
			bool isFourtyByteHeader(uint8_t* data);

		};
	}
}
#endif // _GUI_MBNTOOLWINDOW_H
