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

#include "qcdm_prl_write_worker.h"

using namespace OpenPST;

QcdmPrlWriteWorker::QcdmPrlWriteWorker(QcdmSerial& port, QcdmPrlWriteWorkerRequest request, QObject *parent) :
    port(port),
    request(request),
    QThread(parent),
    cancelled(false)
{

}

QcdmPrlWriteWorker::~QcdmPrlWriteWorker()
{

}

void QcdmPrlWriteWorker::cancel()
{
    cancelled = true;
}

void QcdmPrlWriteWorker::run()
{
    return;
}