/**
* LICENSE PLACEHOLDER
*
* @file sahara_memory_read_worker.h
* @class SaharaMemoryReadWorker
* @package OpenPST
* @brief Handles background processing of memory reading in sahara mode
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#ifndef _GUI_WORKER_SAHARA_MEMORY_READ_WORKER_H
#define _GUI_WORKER_SAHARA_MEMORY_READ_WORKER_H

#include <QThread>
#include "gui/worker/worker.h"
#include "qc/sahara_serial.h"
#include "qc/sahara.h"

using OpenPST::QC::SaharaSerial;

namespace OpenPST {
	namespace GUI {	

		struct SaharaMemoryReadWorkerData {
			uint32_t        address;
			uint32_t        lastAddress;
			size_t          size;
			size_t          outSize;
			size_t          lastChunkSize;
			size_t          stepSize;
			std::string     outFilePath;
		};

		class SaharaMemoryReadWorker : public QThread
		{
			Q_OBJECT

			public:
				SaharaMemoryReadWorker(SaharaSerial& port, SaharaMemoryReadWorkerData data, QObject *parent = 0);
				~SaharaMemoryReadWorker();
			protected:
				SaharaSerial&  port;
				SaharaMemoryReadWorkerData data;

				void run() Q_DECL_OVERRIDE;         
			signals:
				void update(SaharaMemoryReadWorkerData data);
				void complete(SaharaMemoryReadWorkerData data);
				void error(SaharaMemoryReadWorkerData data, QString msg);
		};		
	}
}

#endif // _GUI_WORKER_SAHARA_MEMORY_READ_WORKER_H