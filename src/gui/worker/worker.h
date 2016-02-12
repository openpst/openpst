/**
* LICENSE PLACEHOLDER
*
* @file worker.h
* @class Worker
* @package OpenPST
* @brief Base class for all thread workers
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_WORKER_H
#define _GUI_WORKER_WORKER_H

#include <QThread>

namespace OpenPST {
	namespace GUI {
    
		class Worker : public QThread
		{
			
			public:
				Worker(QObject *parent = 0);
				~Worker();
				void cancel();
				bool shouldCancel();
			protected:
				bool cancelled = false;
		};
	}
}

#endif // _GUI_WORKER_WORKER_H