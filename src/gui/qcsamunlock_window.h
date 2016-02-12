/**
* LICENSE PLACEHOLDER
*
* @file qcdm_window.h
* @class QcdmWindow
* @package OpenPST
* @brief QCDM GUI interface class definitions
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#ifndef _GUI_QCSAMUNLOCK_WINDOW_H
#define _GUI_QCSAMUNLOCK_WINDOW_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <QMainWindow>
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QListWidget>
#include <QtXml>
#include "ui_qcsamunlock_window.h"
#include "serial/qcdm_serial.h"
#include "include/definitions.h"
#include "util/sleep.h"
#include "qc/dm.h"
#include "qc/dm_efs_manager.h"
#include "about_dialog.h"
#include "util/convert.h"

namespace Ui {
	class QcSamUnlockWindow;
}

namespace OpenPST {

	class QcSamUnlockWindow : public QMainWindow
	{
		Q_OBJECT

		enum LogType {
			kLogTypeError = 1,
			kLogTypeWarning = 2,
			kLogTypeInfo = 3,
			kLogTypeDebug = 4
		};

	public:
		Ui::QcSamUnlockWindow* ui;
		QcdmSerial port;
		serial::PortInfo currentPort;
		DmEfsManager efsManager;

		/**
		* @brief
		*/
		explicit QcSamUnlockWindow(QWidget *parent = 0);

		/**
		* @brief
		*/
		~QcSamUnlockWindow();

	public slots:
		/**
		* @brief
		*/
		void updatePortList();

		/**
		* @brief
		*/
		void connectPort();

		/**
		* @brief
		*/
		void disconnectPort();

		/**
		* @brief
		*/
		void clearLog();

		/**
		* @brief
		*/
		void saveLog();

		/**
		* @brief
		*/
		void About();

	private:
		AboutDialog* aboutDialog;

		QcdmEfsStatfsResponse        statResponse;
		QcdmEfsSyncResponse          syncResponse;
		QcdmEfsGetSyncStatusResponse syncStatusResponse;
		int syncMaxRetries = 10;
		int syncRetries = 0;
		int sequence = 0;

		/**
		* @brief
		*/
		bool processItem(int item, int sequence);

		/**
		* @brief
		*/
		void unlockSim();

		/**
		* @brief
		*/
		void log(const char* message);

		/**
		* @brief
		*/
		void log(int type, const char* message);

		/**
		* @brief
		*/
		void log(int type, std::string message);

		/**
		* @brief
		*/
		void log(int type, QString message);
	};

}
#endif // _GUI_QCSAMUNLOCK_WINDOW_H
