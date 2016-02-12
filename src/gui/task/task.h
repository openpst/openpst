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

#ifndef _GUI_TASK_TASK_H
#define _GUI_TASK_TASK_H

#include <QObject>
#include <QRunnable>

class Task : public QObject, public QRunnable
{
	Q_OBJECT

	public:
		~Task() {
			if (canceller) {
				*canceller = false;
			}
		}
		virtual void run() = 0;

		bool cancelled() {
			return canceller ? *canceller : false;
		}
		void setCanceller(volatile bool* cancelController) {
			canceller = cancelController;
		}

	protected:
		volatile bool* canceller = 0;

	signals:
		void started();
		void aborted();
		void complete();
		void error(QString msg);
		void log(QString msg);
};

#endif // _GUI_TASK_TASK_H