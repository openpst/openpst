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

#ifndef _GUI_QCDM_WINDOW_H
#define _GUI_QCDM_WINDOW_H

#include <QMainWindow>
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QClipboard>
#include <QListWidget>
#include <QtXml>
#include "ui_qcdm_window.h"
#include "about_dialog.h"
#include "qc/dm.h"
#include "qc/dm_nv.h"
#include "qc/dm_efs.h"
#include "qc/dm_efs_manager.h"
#include "serial/qcdm_serial.h"
#include "worker/qcdm_efs_directory_tree_worker.h"
#include "worker/qcdm_efs_file_read_worker.h"
#include "worker/qcdm_efs_file_write_worker.h"
#include "worker/qcdm_memory_read_worker.h"
#include "worker/qcdm_prl_write_worker.h"
#include "worker/qcdm_prl_read_worker.h"
#include "worker/qcdm_nv_item_read_worker.h"
#include "util/convert.h"

namespace Ui {
	class QcdmWindow;
}

namespace OpenPST {
	namespace GUI {
		class QcdmWindow : public QMainWindow
		{
			Q_OBJECT

			enum LogType {
				kLogTypeError = 1,
				kLogTypeWarning = 2,
				kLogTypeInfo = 3,
				kLogTypeDebug = 4
			};

			enum EfsBrowserColumns {
				kEfsBrowserColumnName = 0,
				kEfsBrowserColumnType,
				kEfsBrowserColumnSize,
				kEfsBrowserColumnMode,
				kEfsBrowserColumnATime,
				kEfsBrowserColumnMTime,
				kEfsBrowserColumnCTime,
				kEfsBrowserColumnFullPath
			};

			enum SpcReadType {
				kSpcReadTypeNv = 0,
				kSpcReadTypeEfs = 1,
				kSpcReadTypeHtc = 2,
				kSpcReadTypeLg = 3,
				kSpcReadTypeSamsung = 3,
			};

			std::vector<QThread*> workers;

		public:
			Ui::QcdmWindow* ui;
			QcdmSerial port;
			serial::PortInfo currentPort;
			DmEfsManager efsManager;
			AboutDialog* aboutWindow;
			QcdmNvItemReadWorker* nvItemReadWorker;

			/**
			* @brief
			*/
			explicit QcdmWindow(QWidget *parent = 0);

			/**
			* @brief
			*/
			~QcdmWindow();

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
			void disableUI();

			/**
			* @brief
			*/
			void enableUI();

			/**
			* @brief
			*/
			void getVersionInfo();

			/**
			* @brief
			*/
			void getGuid();
		
			/**
			* @brief
			*/
			void getStatus();
				
			/**
			* @brief
			*/
			void sendSpc();

			/**
			* @brief
			*/
			void sendPassword();

			/**
			* @brief
			*/
			void readImei();

			/**
			* @brief
			*/
			void readMeid();

			/**
			* @brief
			*/
			void writeMeid();

			/**
			* @brief
			*/
			void readNam();

			/**
			* @brief
			*/
			void writeNam();

			/**
			* @brief
			*/
			void nvReadSelectionToLog();

			/**
			* @brief
			*/
			void nvReadSelectionToText();

			/**
			* @brief
			*/
			void nvReadSelectionToBinary();

			/**
			* @brief
			*/
			void nvReadSelectionSelectAll();

			/**
			* @brief
			*/
			void nvReadSelectionClearSelection();

			/**
			* @brief
			*/
			void nvReadRangeToLog();

			/**
			* @brief
			*/
			void nvReadRangeToText();

			/**
			* @brief
			*/
			void nvReadRangeToBinary();

			/**
			* @brief
			*/
			void nvReadSingleToLog();

			/**
			* @brief
			*/
			void readSpc();

			/**
			* @brief
			*/
			void writeSpc();

			/**
			* @brief
			*/
			void readSubscription();

			/**
			* @brief
			*/
			void writeSubscription();

			/**
			* @brief
			*/
			void setPhoneMode();

			/**
			* @brief
			*/
			void switchToDload();

		
			/**
			* @brief
			*/
			void onSpcTextChanged(QString value);

			/**
			* @brief
			*/
			void probeCommands();

			/**
			* @brief
			*/
			void efsHello();

			/**
			* @brief
			*/
			void efsGetDeviceInfo();

			/**
			* @brief
			*/
			void efsListDirectories();

			/**
			* @brief
			*/
			void efsQuery();

			/**
			* @brief
			*/
			void efsExtractFactoryImage();

			/**
			* @brief
			*/
			void efsMakeGoldenCopy();

			/**
			* @brief
			*/
			void efsFilesystemImage();

			/**
			* @brief
			*/
			void efsGetFileInfo();

			/**
			* @brief
			*/
			void efsGetFile();

			/**
			* @brief
			*/
			void efsDeleteFile();

			/**
			* @brief
			*/
			void onEfsTreeNodeRightClick(const QPoint& point);

			/**
			* @brief
			*/
			void onEfsSubsysCommandCodeChange();

			/**
			* @brief
			*/
			void onEfsSubsysIdChange();

			/**
			* @brief
			*/
			void efsReadAll();

			/**
			* @brief
			*/
			void efsContextMenuSaveFile();

			/**
			* @brief
			*/
			void efsContextMenuSaveDirectory();

			/**
			* @brief
			*/
			void efsContextMenuSaveDirectoryCompressed();

			/**
			* @brief
			*/
			void efsContextMenuUploadFile();

			/**
			* @brief
			*/
			void efsContextMenuCopyPathToClipboard();

			/**
			* @brief
			*/
			void efsContextMenuDeleteDirectory();

			/**
			* @brief
			*/
			void efsContextMenuDeleteFile();

			/**
			* @brief
			*/
			void efsContextMenuCreateDirectory();


			/**
			* @brief
			*/
			void efsContextMenuCreateLink();

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

			void readSome();

			void sendCommand();

			void nvReadRequest(QcdmNvItemReadWorkerRequest& request);

			void nvItemReadUpdate(QcdmNvItemReadWorkerResponse response);

			void nvItemReadComplete(QcdmNvItemReadWorkerRequest request);

			void nvItemReadError(QcdmNvItemReadWorkerRequest request, QString msg);

			void nvItemWriteSingle();

		private:
			/**
			* @brief
			*/
			void ReadMdn();

			/**
			* @brief
			*/
			void WriteMdn();

			/**
			* @brief
			*/
			void ReadMin();

			/**
			* @brief
			*/
			void ReadSid();

			/**
			* @brief
			*/
			void ReadSystemPref();

			/**
			* @brief
			*/
			void WriteSystemPref();

			/**
			* @brief
			*/
			void ReadPrefMode();

			/**
			* @brief
			*/
			void ReadPrefServ();

			/**
			* @brief
			*/
			void WritePrefServ();

			/**
			* @brief
			*/
			void ReadRoamPref();

			/**
			* @brief
			*/
			void WriteRoamPref();


			/**
			* @brief
			*/
			void ReadPapUserId();

			/**
			* @brief
			*/
			void ReadPapPassword();

			/**
			* @brief
			*/
			void ReadPppUserId();

			/**
			* @brief
			*/
			void ReadPppPassword();

			/**
			* @brief
			*/
			void ReadHdrAnUserId();

			/**
			* @brief
			*/
			void ReadHdrAnPassword();

			/**
			* @brief
			*/
			void ReadHdrAnLongUserId();

			/**
			* @brief
			*/
			void ReadHdrAnLongPassword();

			/**
			* @brief
			*/
			void ReadHdrAnPppUserId();

			/**
			* @brief
			*/
			void ReadHdrAnPppPassword();


			/**
			* @brief
			*/
			void EfsAddTreeNodes(QTreeWidgetItem* parent, std::vector<DmEfsNode>& entries);



			/**
			* @brief
			*/
			QString FixedEmptyTrim(QString input);


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
}
#endif // _GUI_QCDM_WINDOW_H
