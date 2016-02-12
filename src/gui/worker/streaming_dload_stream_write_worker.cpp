/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_stream_write_worker.cpp
* @class StreamingDloadStreamWriteWorker
* @package OpenPST
* @brief Handles background processing of writing data to flash using streaming write 
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "streaming_dload_stream_write_worker.h"

using namespace OpenPST::GUI;
using namespace OpenPST::QC;

StreamingDloadStreamWriteWorker::StreamingDloadStreamWriteWorker(StreamingDloadSerial& port, StreamingDloadStreamWriteWorkerRequest request, QObject *parent) :
    port(port),
    request(request),
    QThread(parent)
{
    cancelled = false;
}

StreamingDloadStreamWriteWorker::~StreamingDloadStreamWriteWorker()
{

}

void StreamingDloadStreamWriteWorker::cancel()
{
    cancelled = true;
}

void StreamingDloadStreamWriteWorker::run()
{
    QString tmp;

    std::ifstream file(request.filePath.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        emit error(request, tmp.sprintf("Error opening %s for reading", request.filePath.c_str()));
        return;
    }

    file.seekg(0, file.end);

    size_t fileSize = file.tellg();

    file.seekg(0, file.beg);

    size_t writeSize = port.state.hello.maxPreferredBlockSize;

    request.outSize = 0;

    uint8_t fileBuffer[STREAMING_DLOAD_MAX_TX_SIZE];

    if (fileSize <= writeSize) {

        file.read((char*)fileBuffer, fileSize);

        if (port.streamWrite(request.address, fileBuffer, fileSize, request.unframed) != kStreamingDloadSuccess) {
            file.close();
            emit error(request, tmp.sprintf("Error writing %lu bytes starting at address 0x%04X", fileSize, request.address));
            return;
        }

        emit chunkComplete(request);
    } else {

        uint32_t address = request.address;

        do {

            file.read((char*)fileBuffer, writeSize);

            if (port.streamWrite((address + request.outSize), fileBuffer, writeSize, request.unframed) != kStreamingDloadSuccess) {
                file.close();
                emit error(request, tmp.sprintf("Error writing %lu bytes starting at address 0x%04X", writeSize, (address + request.outSize)));
                return;
            }
            
            request.outSize += writeSize;

            emit chunkComplete(request);

        } while (request.outSize < fileSize && !cancelled);

    }       

    file.close();

    emit complete(request);
}

