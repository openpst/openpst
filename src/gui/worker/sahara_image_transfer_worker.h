/**
* LICENSE PLACEHOLDER
*
* @file sahara_image_transfer_worker.h
* @class SaharaImageTransferWorker
* @package OpenPST
* @brief Handles background processing of image transfers in sahara mode
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_SAHARA_IMAGE_TRANSFER_WORKER_H
#define _GUI_WORKER_SAHARA_IMAGE_TRANSFER_WORKER_H

#include <QThread>
#include "gui/worker/worker.h"
#include "qc/sahara_serial.h"
#include "qc/sahara.h"

using namespace serial;

using OpenPST::QC::SaharaSerial;
using OpenPST::GUI::Worker;

namespace OpenPST {
    namespace GUI {
        
        struct SaharaImageTransferWorkerData {
            size_t          fileSize;
            size_t          chunkSize;
            uint32_t        offset;
            size_t          sent;
            uint32_t        imageType;
            std::string     imagePath;
        };
        
        class SaharaImageTransferWorker : public QThread
        {
			Q_OBJECT

            public:
                SaharaImageTransferWorker(SaharaSerial& port, SaharaImageTransferWorkerData data, QObject *parent = 0);
                ~SaharaImageTransferWorker();
            protected:
                SaharaSerial& port;
				SaharaImageTransferWorkerData data;

                void run() Q_DECL_OVERRIDE;
            signals:
				void update(SaharaImageTransferWorkerData data);
				void complete(SaharaImageTransferWorkerData data);
				void error(SaharaImageTransferWorkerData data, QString msg);
        };
    }
}

#endif // _GUI_WORKER_SAHARA_IMAGE_TRANSFER_WORKER_H