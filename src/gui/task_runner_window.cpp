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

#include "gui/task_runner_window.h"

using namespace OpenPST::GUI;

TaskRunnerWindow::TaskRunnerWindow(QWidget *parent) :
	QMainWindow(parent),
	taskRunner(&cancelTask)
{
}

TaskRunnerWindow::~TaskRunnerWindow()
{

}

void TaskRunnerWindow::addTask(Task* task)
{
	connect(task, &Task::started,   this, &TaskRunnerWindow::onTaskStarted);
	connect(task, &Task::complete,  this, &TaskRunnerWindow::onTaskComplete);
	connect(task, &Task::error,		this, &TaskRunnerWindow::onTaskError);
	connect(task, &Task::log,		this, &TaskRunnerWindow::onTaskLog);

	taskRunner.queue(task);
}

void TaskRunnerWindow::cancelCurrentTask()
{
	cancelTask = true;

}

void TaskRunnerWindow::cancelAllTasks()
{
	taskRunner.clearQueue();
	cancelTask = true;
}

void TaskRunnerWindow::onTaskStarted()
{
	printf("TaskRunnerWindow::onTaskStarted\n");

}

void TaskRunnerWindow::onTaskComplete()
{
	printf("TaskRunnerWindow::onTaskComplete\n");
}

void TaskRunnerWindow::onTaskError(QString msg)
{
	printf("TaskRunnerWindow::onTaskError\n");
}

void TaskRunnerWindow::onTaskLog(QString msg)
{
	printf("TaskRunnerWindow::onTaskLog\n");
}