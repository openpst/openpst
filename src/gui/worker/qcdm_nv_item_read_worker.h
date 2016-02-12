/**
* LICENSE PLACEHOLDER
*
* @file qcdm_nv_item_read_worker.h
* @class QcdmNvItemReadWorker
* @package OpenPST
* @brief Handles background processing of NV item reading
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_QCDM_NV_ITEM_READ_WORKER_H
#define _GUI_WORKER_QCDM_NV_ITEM_READ_WORKER_H

#include <QThread>
#include "serial/qcdm_serial.h"
#include <iostream>
#include <fstream>

using namespace serial;

namespace OpenPST {
	namespace GUI {
	    struct QcdmNvItemReadWorkerRequest {
			int type;
			std::vector<uint16_t> items;
			std::string outFilePath;
			uint16_t current;
	    };

		struct QcdmNvItemReadWorkerResponse {
			int type;
			uint16_t item;
			uint8_t data[DIAG_NV_ITEM_SIZE];
		};

		enum QcdmNvItemReadWorkerRequestType {
			QcdmNvItemReadWorkerRequestTypeLog    = 0,
			QcdmNvItemReadWorkerRequestTypeText   = 1,
			QcdmNvItemReadWorkerRequestTypeBinary = 2,
		};

	    class QcdmNvItemReadWorker : public QThread
	    {
			Q_OBJECT

			public:
				QcdmNvItemReadWorker(QcdmSerial& port, QcdmNvItemReadWorkerRequest request, QObject *parent = 0);
				~QcdmNvItemReadWorker();
				void cancel();
			protected:
				QcdmSerial&  port;
				QcdmNvItemReadWorkerRequest request;

				void run() Q_DECL_OVERRIDE;
				bool cancelled;
				void doFileRun();
				void doLogRun();
			signals:
				void update(QcdmNvItemReadWorkerResponse request);
				void complete(QcdmNvItemReadWorkerRequest request);
				void error(QcdmNvItemReadWorkerRequest request, QString msg);
	    };
	}
}

#endif // _GUI_WORKER_QCDM_NV_ITEM_READ_WORKER_H