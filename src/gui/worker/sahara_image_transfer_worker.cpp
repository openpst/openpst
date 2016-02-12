/**
* LICENSE PLACEHOLDER
*
* @file sahara_image_transfer_worker.cpp
* @class SaharaImageTransferWorker
* @package OpenPST
* @brief Handles background processing of image transfers in sahara mode
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "sahara_image_transfer_worker.h"

using namespace OpenPST::GUI;

SaharaImageTransferWorker::SaharaImageTransferWorker(SaharaSerial& port, SaharaImageTransferWorkerData data, QObject *parent) :
    port(port),
	data(data),
    QThread(parent)
{
}

SaharaImageTransferWorker::~SaharaImageTransferWorker()
{

}

void SaharaImageTransferWorker::run() 
{
	
    /*QString tmp; 
    request.sent = 0;

#ifdef _WIN32
    FILE* fp;
    fopen_s(&fp, request.imagePath.c_str(), "rb");
#else
    FILE* fp = fopen(request.imagePath.c_str(), "rb");
#endif

    if (!fp) {
        emit error(request, tmp.sprintf("Error opening image %s for reading", request.imagePath.c_str()));
        return;
    }
    
    size_t fileReadResult;

    printf("Loaded File %s With Size %lu\n", request.imagePath.c_str(), request.fileSize);

    uint8_t* fileBuffer = new uint8_t[request.chunkSize];
    size_t fileBufferSize = request.chunkSize;

    while (request.sent != request.fileSize && !cancelled) {
        if (request.chunkSize > fileBufferSize) {
            fileBuffer = (uint8_t*)realloc(fileBuffer, request.chunkSize);
            if (fileBuffer == NULL) {
                free(fileBuffer);
                fclose(fp);
                emit error(request, tmp.sprintf("Could Not Allocate %lu Bytes For File Buffer\n", request.chunkSize));
                return;
            }
        }

        if (request.fileSize < (request.sent + request.chunkSize)) {
            request.chunkSize = request.fileSize - request.sent;
        }

        fileReadResult = fread(fileBuffer, sizeof(uint8_t), request.chunkSize, fp);

        request.chunkSize = port.write(fileBuffer, request.chunkSize);

        if (!request.chunkSize) {
            free(fileBuffer);
            fclose(fp);
            emit error(request, "Error sending next chunk of image");
            return;
        }

        hexdump_tx(fileBuffer, request.chunkSize);

        request.sent += request.chunkSize;
        
		emit update(request);

        if (!port.readNextImageOffset(request.offset, request.chunkSize)) {
            free(fileBuffer);
            fclose(fp);
            emit error(request, "Error getting next image offset and size");
            return;
        }

        if (request.offset == 0x00 && request.chunkSize == 0x00) {
            break;
        }
    }

    free(fileBuffer);
    fclose(fp);
	*/
	emit complete(data);
}

