/**
* LICENSE PLACEHOLDER
*
* @file worker.cpp
* @class Worker
* @package OpenPST
* @brief Base class for all thread workers
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "gui/worker/worker.h"

using namespace OpenPST::GUI;

Worker::Worker(QObject *parent) :
    QThread(parent),
	cancelled(false)
{

}

Worker::~Worker()
{

}

bool Worker::shouldCancel()
{
	return cancelled;
}

void Worker::cancel()
{
	cancelled = true;
}
