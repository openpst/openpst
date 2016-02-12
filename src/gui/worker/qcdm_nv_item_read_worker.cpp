/**
* LICENSE PLACEHOLDER
*
* @file qcdm_nv_item_read_worker.cpp
* @class QcdmNvItemReadWorker
* @package OpenPST
* @brief Handles background processing of NV item reading
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "qcdm_nv_item_read_worker.h"

using namespace OpenPST;

QcdmNvItemReadWorker::QcdmNvItemReadWorker(QcdmSerial& port, QcdmNvItemReadWorkerRequest request, QObject *parent) :
	port(port),
    request(request),
    QThread(parent),
    cancelled(false)
{

}

QcdmNvItemReadWorker::~QcdmNvItemReadWorker()
{

}

void QcdmNvItemReadWorker::cancel()
{
    cancelled = true;
}

void QcdmNvItemReadWorker::run()
{
	if ((request.type != QcdmNvItemReadWorkerRequestTypeLog && request.outFilePath.length())) {
		return doFileRun();
	} else {
		return doLogRun();
	}
}

void QcdmNvItemReadWorker::doFileRun()
{
	QString nvHexHeading;
	QString nvHex; 
	QcdmNvResponse nvItem;
	QcdmNvItemReadWorkerResponse response = {};

	std::ofstream file(request.outFilePath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

	if (!file.is_open()) {
		emit error(request, "Error opening file for writing");
		return;
	}

	response.type = request.type;

	for (auto item : request.items) {

		if (cancelled) {
			return;
		}

		request.current = item;
		response.item	= item;

		try {
			nvItem = port.readNV(item);
		} catch (std::exception e) {
			emit error(request, e.what());
			
			if (request.type == QcdmNvItemReadWorkerRequestTypeText) {
				nvHexHeading.sprintf("========\nError Reading Item %d\n========\n", nvItem.nvItem);			
				file.write(nvHexHeading.toStdString().c_str(), nvHexHeading.size());
				continue;
			}
		}

		std::memcpy(response.data, nvItem.data, sizeof(nvItem.data));

		if (request.type == QcdmNvItemReadWorkerRequestTypeText) {
			nvHexHeading.sprintf("========\nItem %d\n========\n", nvItem.nvItem);
			hexdump(nvItem.data, sizeof(nvItem.data), nvHex, false);
			file.write(nvHexHeading.toStdString().c_str(), nvHexHeading.size());
			file.write(nvHex.toStdString().c_str(), nvHex.size());
		} else {
			file.write((char*)&nvItem, sizeof(nvItem));
		}

		emit update(response);
	}

	file.close();

	emit complete(request);


		
}

void QcdmNvItemReadWorker::doLogRun()
{
	QcdmNvResponse nvItem;
	QcdmNvItemReadWorkerResponse response = {};

	for (auto item : request.items) {

		if (cancelled) {
			return;
		}

		request.current = item;
		response.item = item;

		try {
			nvItem = port.readNV(item);
		} catch (std::exception e) {
			emit error(request, e.what());
			continue;
		}

		std::memcpy(response.data, nvItem.data, sizeof(nvItem.data));

		emit update(response);
	}

	emit complete(request);
}