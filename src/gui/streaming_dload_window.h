/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_window.h
* @class StreamingDloadWindow
* @package OpenPST
* @brief Streaming DLOAD GUI interface class definitions
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_STREAMING_DLOAD_WINDOW_H
#define _GUI_STREAMING_DLOAD_WINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_streaming_dload_window.h"
#include "qc/streaming_dload.h"
#include "qc/streaming_dload_serial.h"
#include "gui/worker/streaming_dload_read_worker.h"
#include "gui/worker/streaming_dload_stream_write_worker.h"
#include <iostream>
#include <fstream>

namespace Ui {
	class StreamingDloadWindow;
}

namespace OpenPST{
	namespace GUI {
		class StreamingDloadWindow : public QMainWindow
		{

			Q_OBJECT

			public:
				explicit StreamingDloadWindow(QWidget *parent = 0);
				~StreamingDloadWindow();


			public slots:

				/**
				 * @brief updatePortList
				 */
				void updatePortList();

				/**
				 * @brief connectToPort
				 */
				void connectToPort();

				/**
				 * @brief disconnectPort
				 */
				void disconnectPort();

				/**
				 * @brief sendHello
				 */
				void sendHello();

				/**
				* @brief sendUnlock
				*/
				void sendUnlock();

				/**
				* @brief setSecurityMode
				*/
				void setSecurityMode();

				/**
				* @brief sendNop
				*/
				void sendNop();

				/**
				* @brief sendReset
				*/
				void sendReset();

				/**
				* @brief sendPowerDown
				*/
				void sendPowerDown();

				/**
				* @brief openMode
				*/
				void openMode();

				/**
				* @brief closeMode
				*/
				void closeMode();

				/**
				* @brief openMultiMode
				*/
				void openMultiMode();

				/**
				* @brief readEccState
				*/
				void readEccState();

				/**
				* @brief setEccState
				*/
				void setEccState();

				/**
				* @brief read - Read address and size
				*/
				void read();

				/**
				* @brief streamWrite - Stream write file starting at spcified address
				*/
				void streamWrite();

				/**
				* @brief eraseFlash
				*/
				void eraseFlash();

				/**
				* @brief clearLog
				*/
				void clearLog();

				/**
				 * @brief SaveLog
				 */
				void saveLog();

				/**
				* @brief browseForPartitionTable
				*/
				void browseForParitionTable();

				/**
				* @brief writePartitionTable
				*/
				void writePartitionTable();

				/**
				* @brief browseForWriteFile
				*/
				void browseForWriteFile();
            
				/**
				* @brief readChunkReadyHandler - callback function to update UI when a read chunk is ready. 
				*                                used to increment the progress bar
				*/
				void readChunkReadyHandler(StreamingDloadReadWorkerRequest request);

				/**
				* @brief readCompleteHandler - callback function to update UI when the read worker completes the request
				*/
				void readCompleteHandler(StreamingDloadReadWorkerRequest request);

				/**
				* @brief readChunkErrorHandler - callback function to update UI when the read worker encounters an error
				*/
				void readChunkErrorHandler(StreamingDloadReadWorkerRequest request, QString msg);
            
				/**
				* @brief streamWriteChunkCompleteHandler - callback function to update UI when a stream write chunk has been written. 
				*                                       used to increment the progress bar
				*/
				void streamWriteChunkCompleteHandler(StreamingDloadStreamWriteWorkerRequest request);

				/**
				* @brief streamWriteCompleteHandler - callback function to update UI when a stream write worker completes the request
				*/
				void streamWriteCompleteHandler(StreamingDloadStreamWriteWorkerRequest request);

				/**
				* @brief streamWriteErrorHandler - callback function to update UI when a stream write worker encounters an error
				*/
				void streamWriteErrorHandler(StreamingDloadStreamWriteWorkerRequest request, QString msg);

				/**
				* @brief cancelOperation - Cancels any currently running workers
				*/          
				void cancelOperation();

				/**
				* @brief enableControls
				*/
				void enableControls();

				/**
				* @brief disableControls
				*/
				void disableControls();

				/**
				* @brief log
				*/
				void log(const char* message);

				/**
				* @brief SaveLog
				*/
				void log(QString message);

			private:
				Ui::StreamingDloadWindow *ui;
				StreamingDloadSerial port;
				serial::PortInfo currentPort;
				StreamingDloadReadWorker* readWorker;
				StreamingDloadStreamWriteWorker* streamWriteWorker;
			};
	}
}
#endif // _GUI_STREAMING_DLOAD_WINDOW_H
