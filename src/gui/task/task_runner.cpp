/**
* LICENSE PLACEHOLDER
*
* @file
* @class
* @package OpenPST
* @brief
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "task_runner.h"

TaskRunner::TaskRunner(volatile bool* canceller) : canceller(canceller)
{
	pool.setExpiryTimeout(-1);
	pool.setMaxThreadCount(1);
}

TaskRunner::~TaskRunner()
{
	cancelAll();
	if (pool.activeThreadCount() > 0) {
		pool.waitForDone();
	}
}

int TaskRunner::queue(Task* task, int priority)
{
	if (canceller) {
		task->setCanceller(canceller);
	}

	pool.start(task, priority);

	return 0;
}


void TaskRunner::clearQueue()
{
	pool.clear();
}

void TaskRunner::cancelCurrent()
{
	if (canceller) {
		*canceller = true;
	}
}

void TaskRunner::cancelAll()
{
	clearQueue();
	cancelCurrent();
}

bool TaskRunner::waitForDone()
{
	if (pool.activeThreadCount()) {
		return pool.waitForDone();
	}
	return true;
}