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

#ifndef _GUI_TASK_RUNNER_WINDOW_WINDOW_H
#define _GUI_TASK_RUNNER_WINDOW_WINDOW_H

#include <QMainWindow>
#include "gui/task/task.h"
#include "gui/task/task_runner.h"

namespace OpenPST {
	namespace GUI {
    
		class TaskRunnerWindow
		{
			
			Q_OBJECT

			protected:
				TaskRunner taskRunner;
				volatile bool cancelTask = false;

			protected:
				virtual void addTask(Task* task) = 0;

			public slots:
				virtual void cancelCurrentTask() = 0;
				virtual void cancelAllTasks() = 0;
				virtual void onTaskStarted() = 0;
				virtual void onTaskComplete() = 0;
				virtual void onTaskError(QString msg) = 0;
				virtual void onTaskLog(QString msg) = 0;
		};
	}
}

#endif // _GUI_TASK_RUNNER_WINDOW_WINDOW_H