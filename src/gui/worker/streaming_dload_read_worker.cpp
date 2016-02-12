/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_read_worker.cpp
* @class StreamingDloadReadWorker
* @package OpenPST
* @brief Handles background processing of open/open-multi mode reading
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "streaming_dload_read_worker.h"

using namespace OpenPST::GUI;
using namespace OpenPST::QC;

StreamingDloadReadWorker::StreamingDloadReadWorker(StreamingDloadSerial& port, StreamingDloadReadWorkerRequest request, QObject *parent) :
    port(port),
    request(request),
    QThread(parent)
{
    cancelled = false;
}

StreamingDloadReadWorker::~StreamingDloadReadWorker()
{

}

void StreamingDloadReadWorker::cancel()
{
    cancelled = true;
}

void StreamingDloadReadWorker::run()
{
    QString tmp;

    std::ofstream file(request.outFilePath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    
    if (!file.is_open()) {
        emit error(request, tmp.sprintf("Error opening %s for writing", request.outFilePath.c_str()));
        return;
    }

    if (request.stepSize > port.state.hello.maxPreferredBlockSize) {
        request.stepSize = port.state.hello.maxPreferredBlockSize;
    }

    request.outSize = 0;

    if (request.size <= request.stepSize) {

        if (port.readAddress(request.address, request.stepSize, file, request.outSize, request.stepSize) != kStreamingDloadSuccess) {
            file.close();
            emit error(request, tmp.sprintf("Error reading %lu bytes from address 0x%08X", request.stepSize, request.address));
            return;
        }

        emit chunkReady(request);
    } else {

        
        uint32_t address = request.address;

        do {
            size_t chunkOutSize = 0;

            if (port.readAddress((address + request.outSize), request.stepSize, file, chunkOutSize, request.stepSize) != kStreamingDloadSuccess) {
                file.close();
                emit error(request, tmp.sprintf("Error reading %lu bytes from address 0x%08X", request.stepSize, (address + request.outSize)));
                return;
            }
            
            request.outSize += chunkOutSize;

            emit chunkReady(request);

        } while (request.outSize < request.size && !cancelled);

    }       

    file.close();

    emit complete(request);
}