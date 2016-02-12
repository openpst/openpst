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

#ifndef _GUI_TASK_TASK_RUNNER_H
#define _GUI_TASK_TASK_RUNNER_H

#include <QThreadPool>
#include "task.h"

class TaskRunner
{	
	
	private:
		QThreadPool pool;
		volatile bool* canceller;
	public:
		TaskRunner(volatile bool* canceller = 0);
		~TaskRunner();
		void clearQueue();
		void cancelCurrent();
		void cancelAll();
		bool waitForDone();
		int queue(Task* task, int priority = 0);
};



#endif // _GUI_TASK_TASK_RUNNER_H
