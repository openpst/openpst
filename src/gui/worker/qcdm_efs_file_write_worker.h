/**
* LICENSE PLACEHOLDER
*
* @file qcdm_efs_file_write_worker.h
* @class QcdmEfsFileWriteWorker
* @package OpenPST
* @brief Handles background processing of file writing
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_QCDM_EFS_FILE_WRITE_WORKER_H
#define _GUI_WORKER_QCDM_EFS_FILE_WRITE_WORKER_H

#include <QThread>
#include "qc/dm_efs_manager.h"

using namespace serial;

namespace OpenPST {
    namespace GUI {
        struct QcdmEfsFileWriteWorkerRequest {

        };

        class QcdmEfsFileWriteWorker : public QThread
        {
    		Q_OBJECT

        public:
            QcdmEfsFileWriteWorker(DmEfsManager& efsManager, QcdmEfsFileWriteWorkerRequest request, QObject *parent = 0);
            ~QcdmEfsFileWriteWorker();
            void cancel();
        protected:
            DmEfsManager&  efsManager;
            QcdmEfsFileWriteWorkerRequest request;

            void run() Q_DECL_OVERRIDE;
            bool cancelled;
        signals:
            void update(QcdmEfsFileWriteWorkerRequest request);
            void complete(QcdmEfsFileWriteWorkerRequest request);
            void error(QcdmEfsFileWriteWorkerRequest request, QString msg);
        };
    }
}

#endif // _GUI_WORKER_QCDM_EFS_FILE_WRITE_WORKER_H