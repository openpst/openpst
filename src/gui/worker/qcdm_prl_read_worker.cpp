/**
* LICENSE PLACEHOLDER
*
* @file qcdm_prl_read_worker.cpp
* @class QcdmPrlReadWorker
* @package OpenPST
* @brief Handles background processing of reading PRL
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "qcdm_prl_read_worker.h"

using namespace OpenPST;

QcdmPrlReadWorker::QcdmPrlReadWorker(QcdmSerial& port, QcdmPrlReadWorkerRequest request, QObject *parent) :
    port(port),
    request(request),
    QThread(parent),
    cancelled(false)
{

}

QcdmPrlReadWorker::~QcdmPrlReadWorker()
{

}

void QcdmPrlReadWorker::cancel()
{
    cancelled = true;
}

void QcdmPrlReadWorker::run()
{
    return;
}