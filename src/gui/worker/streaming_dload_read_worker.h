/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_read_worker.h
* @class StreamingDloadReadWorker
* @package OpenPST
* @brief Handles background processing of open/open-multi mode reading
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_kStreamingDloadRead_WORKER_H
#define _GUI_WORKER_kStreamingDloadRead_WORKER_H

#include <QThread>
#include "qc/streaming_dload_serial.h"
#include "qc/streaming_dload.h"

using OpenPST::QC::StreamingDloadSerial;

namespace OpenPST {
    namespace GUI {
        struct StreamingDloadReadWorkerRequest {
            uint32_t        address;
            size_t          size;
            size_t          stepSize;
            size_t          outSize;
            std::string     outFilePath;        
        };
        
        class StreamingDloadReadWorker : public QThread
        {
            Q_OBJECT

            public:
                StreamingDloadReadWorker(StreamingDloadSerial& port, StreamingDloadReadWorkerRequest request, QObject *parent = 0);
                ~StreamingDloadReadWorker();
                void cancel();
            protected:
                StreamingDloadSerial&  port;
                StreamingDloadReadWorkerRequest request;

                void run() Q_DECL_OVERRIDE;         
                bool cancelled;
            signals:
                void chunkReady(StreamingDloadReadWorkerRequest request);
                void complete(StreamingDloadReadWorkerRequest request);
                void error(StreamingDloadReadWorkerRequest request, QString msg);
        };
    }
}

#endif // _GUI_WORKER_kStreamingDloadRead_WORKER_H