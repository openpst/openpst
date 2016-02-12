/**
* LICENSE PLACEHOLDER
*
* @file qcdm_efs_file_write_worker.cpp
* @class QcdmEfsFileWriteWorker
* @package OpenPST
* @brief Handles background processing of file writing
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "qcdm_efs_file_write_worker.h"

using namespace OpenPST;

QcdmEfsFileWriteWorker::QcdmEfsFileWriteWorker(DmEfsManager& efsManager, QcdmEfsFileWriteWorkerRequest request, QObject *parent) :
    efsManager(efsManager),
    request(request),
    QThread(parent),
    cancelled(false)
{

}

QcdmEfsFileWriteWorker::~QcdmEfsFileWriteWorker()
{

}

void QcdmEfsFileWriteWorker::cancel()
{
    cancelled = true;
}

void QcdmEfsFileWriteWorker::run()
{
    return;
}