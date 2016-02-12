/**
* LICENSE PLACEHOLDER
*
* @file qcdm_efs_directory_tree_worker.cpp
* @class QcdmEfsDirectoryTreeWorker
* @package OpenPST
* @brief Handles background processing of directory reading
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "qcdm_efs_directory_tree_worker.h"

using namespace OpenPST;

QcdmEfsDirectoryTreeWorker::QcdmEfsDirectoryTreeWorker(DmEfsManager& efsManager, QcdmEfsDirectoryTreeWorkerRequest request, QObject *parent) :
	efsManager(efsManager),
    request(request),
    QThread(parent),
	cancelled(false)
{

}

QcdmEfsDirectoryTreeWorker::~QcdmEfsDirectoryTreeWorker()
{

}

void QcdmEfsDirectoryTreeWorker::cancel()
{
    cancelled = true;
}

void QcdmEfsDirectoryTreeWorker::run()
{
	return;
}