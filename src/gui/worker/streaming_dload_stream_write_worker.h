/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_stream_write_worker.h
* @class StreamingDloadStreamWriteWorker
* @package OpenPST
* @brief Handles background processing of writing data to flash using streaming write 
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_kStreamingDloadStreamWrite_WORKER_H
#define _GUI_WORKER_kStreamingDloadStreamWrite_WORKER_H

#include <QThread>
#include "qc/streaming_dload_serial.h"
#include "qc/streaming_dload.h"

using OpenPST::QC::StreamingDloadSerial;

namespace OpenPST {
	namespace GUI {
		struct StreamingDloadStreamWriteWorkerRequest {
			uint32_t        address;
			std::string     filePath;
			size_t          outSize;
			bool            unframed;
		};

		class StreamingDloadStreamWriteWorker : public QThread
		{
			Q_OBJECT

		public:
			StreamingDloadStreamWriteWorker(StreamingDloadSerial& port, StreamingDloadStreamWriteWorkerRequest request, QObject *parent = 0);
			~StreamingDloadStreamWriteWorker();
			void cancel();
		protected:
			StreamingDloadSerial&  port;
			StreamingDloadStreamWriteWorkerRequest request;

			void run() Q_DECL_OVERRIDE;
			bool cancelled;
		signals:
			void chunkComplete(StreamingDloadStreamWriteWorkerRequest request);
			void complete(StreamingDloadStreamWriteWorkerRequest request);
			void error(StreamingDloadStreamWriteWorkerRequest request, QString msg);
		};
	}
}

#endif // _GUI_WORKER_kStreamingDloadStreamWrite_WORKER_H