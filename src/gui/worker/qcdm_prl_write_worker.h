/**
* LICENSE PLACEHOLDER
*
* @file qcdm_prl_write_worker.h
* @class QcdmPrlWriteWorker
* @package OpenPST
* @brief Handles background processing of writing PRL 
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _GUI_WORKER_QCDM_PRL_WRITE_WORKER_H
#define _GUI_WORKER_QCDM_PRL_WRITE_WORKER_H

#include <QThread>
#include "serial/qcdm_serial.h"

using namespace serial;

namespace OpenPST {
    namespace GUI {
        struct QcdmPrlWriteWorkerRequest {
            int nam;
            std::string prlPath;
        };

        class QcdmPrlWriteWorker : public QThread
        {
    		Q_OBJECT

        public:
            QcdmPrlWriteWorker(QcdmSerial& port, QcdmPrlWriteWorkerRequest request, QObject *parent = 0);
            ~QcdmPrlWriteWorker();
            void cancel();
        protected:
            QcdmSerial&  port;
            QcdmPrlWriteWorkerRequest request;

            void run() Q_DECL_OVERRIDE;
            bool cancelled;
        signals:
            void update(QcdmPrlWriteWorkerRequest request);
            void complete(QcdmPrlWriteWorkerRequest request);
            void error(QcdmPrlWriteWorkerRequest request, QString msg);
        };
    }
}

#endif // _GUI_WORKER_QCDM_PRL_WRITE_WORKER_H