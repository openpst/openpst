/**
* LICENSE PLACEHOLDER
*
* @file qcdm_memory_read_worker.h
* @class QcdmMemoryReadWorker
* @package OpenPST
* @brief Handles background processing of memory reading
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _GUI_WORKER_QCDM_MEMORY_READ_WORKER_H
#define _GUI_WORKER_QCDM_MEMORY_READ_WORKER_H

#include <QThread>
#include "serial/qcdm_serial.h"

using namespace serial;

namespace OpenPST {

    namespace GUI {
        struct QcdmMemoryReadWorkerRequest {

        };

        class QcdmMemoryReadWorker : public QThread
        {
            Q_OBJECT

        public:
            QcdmMemoryReadWorker(QcdmSerial& port, QcdmMemoryReadWorkerRequest request, QObject *parent = 0);
            ~QcdmMemoryReadWorker();
            void cancel();
        protected:
            QcdmSerial&  port;
            QcdmMemoryReadWorkerRequest request;

            void run() Q_DECL_OVERRIDE;
            bool cancelled;
        signals:
            void update(QcdmMemoryReadWorkerRequest request);
            void complete(QcdmMemoryReadWorkerRequest request);
            void error(QcdmMemoryReadWorkerRequest request, QString msg);
        };
    }
}

#endif // _GUI_WORKER_QCDM_MEMORY_READ_WORKER_H