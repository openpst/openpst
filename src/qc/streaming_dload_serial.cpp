/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_serial.cpp
* @class StreamingDloadSerial
* @package OpenPST
* @brief Streaming DLOAD protocol serial port implementation
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "streaming_dload_serial.h"

using namespace OpenPST::QC;

/**
* @brief StreamingDloadSerial() - Constructor
*
* @param std::string port - The device to connect to
* @param int baudrate - Defaults to 115200
* @param serial::Timeout - Timeout, defaults to 1000ms
*/
StreamingDloadSerial::StreamingDloadSerial(std::string port, int baudrate, serial::Timeout timeout) :
    HdlcSerial(port, baudrate, timeout), 
    state({})
{
    state.hello.maxPreferredBlockSize = STREAMING_DLOAD_MAX_DATA_SIZE;
}

/**
* @brief ~StreamingDloadSerial() - Deconstructor
*/
StreamingDloadSerial::~StreamingDloadSerial()
{

}


/**
* @brief sendHello - sends the initial handshake for the session
*
* @param std::string magic - The magic handhsake word. Should be QCOM FAST DOWNLOAD HOST
* @param uint8_t version - The max version to be compatible with
* @param uint8_t compatibleVersion - The lowest version to be compatible with
* @param uint8_t featureBits - a set of feature bits of what features to enable.
*   @see qc/streaming_dload.h
*
* @return int
*/
int StreamingDloadSerial::sendHello(std::string magic, uint8_t version, uint8_t compatibleVersion, uint8_t featureBits)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    size_t rxSize, txSize; 
    std::vector<uint8_t> buffer; 
    StreamingDloadHelloRequest hello = {};
    
    hello.command = kStreamingDloadHello;
    memcpy(hello.magic, magic.c_str(), magic.size());
    hello.version = version;
    hello.compatibleVersion = compatibleVersion;
    hello.featureBits = featureBits;

    txSize = write((uint8_t*)&hello, sizeof(hello));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);
    
    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadHelloResponse, buffer)) {
        return kStreamingDloadError;
    }
            
    memcpy(&state.hello, &buffer[0], sizeof(StreamingDloadHelloResponseHeader));
    
    int dataStartIndex = sizeof(StreamingDloadHelloResponseHeader);

    // parse the packet and get the things that are not obvious without calculation
    // flashIdenfier, windowSize, numberOfSectors, sectorSizes, featureBits
    memcpy(&state.hello.flashIdenfier, &buffer[dataStartIndex], state.hello.flashIdLength);
    memcpy(&state.hello.windowSize, &buffer[dataStartIndex + state.hello.flashIdLength], sizeof(state.hello.windowSize));
    memcpy(&state.hello.numberOfSectors, &buffer[dataStartIndex + state.hello.flashIdLength + sizeof(state.hello.windowSize)], sizeof(state.hello.numberOfSectors));

    int sectorSize = 4 * state.hello.numberOfSectors;
    memcpy(&state.hello.sectorSizes, &buffer[dataStartIndex + state.hello.flashIdLength + sizeof(state.hello.windowSize) + sizeof(state.hello.numberOfSectors)], sectorSize-1);
    memcpy(&state.hello.featureBits, &buffer[dataStartIndex + state.hello.flashIdLength + sizeof(state.hello.windowSize) + sizeof(state.hello.numberOfSectors) + sectorSize-1], sizeof(state.hello.featureBits));
    state.hello.featureBits = flip_endian16(state.hello.featureBits);

    return kStreamingDloadSuccess;
}

/**
* @brief sendUnlock - send the unlock command for implementations that require it
*
* @param std::string code - The unlock code
*
* @return int
*/
int StreamingDloadSerial::sendUnlock(std::string code)
{
    if (!isOpen()) {
        LOGD("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer; 
    StreamingDloadUnlockRequest packet;
    
    packet.command = kStreamingDloadUnlock;
    packet.code = std::stoul(code.c_str(), nullptr, 16);
    
    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadUnlocked, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief setSecurityMode - set the session security mode
*
* @param uint8_t mode - The security mode to set to
*
* @return int
*/
int StreamingDloadSerial::setSecurityMode(uint8_t mode)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;

    StreamingDloadSecurityModeRequest packet;
    packet.command = kStreamingDloadSecurityMode;
    packet.mode = mode;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadSecurityModeReceived, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}


/**
* @brief sendNop - send a NOP
*
* @return int
*/
int StreamingDloadSerial::sendNop()
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;
    StreamingDloadNopRequest packet;
    packet.command = kStreamingDloadNop;
    packet.identifier = std::rand();

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadNopResponse, buffer)) {
        return kStreamingDloadError;
    }

    StreamingDloadNopResponse* resp = (StreamingDloadNopResponse*)&buffer[0];

    if (resp->identifier != packet.identifier) {
        LOGD("Received NOP response but identifier did not match\n");
    }

    return kStreamingDloadSuccess;
}

/**
* @brief sendReset - send a reset command
*
* @return int
*/
int StreamingDloadSerial::sendReset()
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;
    
    StreamingDloadResetRequest packet;
    packet.command = kStreamingDloadReset;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadReset_ACK, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief sendNop - send a power down command
*
* @return int
*/
int StreamingDloadSerial::sendPowerOff()
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;
    StreamingDloadResetRequest packet;
    packet.command = kStreamingDloadPowerOff;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadPoweringDown, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief readEcc - read the current ECC state
*
* @param uint8_t& status - Will be set with the current status
* @return int
*/
int StreamingDloadSerial::readEcc(uint8_t& statusOut)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    size_t txSize, rxSize;
    std::vector<uint8_t> buffer; 
    StreamingDloadGetEccStateRequest packet;
    packet.command = kStreamingDloadGetEccState;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadCurrentEccState, buffer)) {
        return kStreamingDloadError;
    }

    StreamingDloadGetEccStateResponse* resp = (StreamingDloadGetEccStateResponse*)&buffer[0];
    
    statusOut = resp->status;

    return kStreamingDloadSuccess;
}

/**
* @brief setEcc - set the current ECC state
*
* @param uint8_t status - The status to set it to
*
* @return int
*/
int StreamingDloadSerial::setEcc(uint8_t status)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    size_t txSize, rxSize;
    std::vector<uint8_t> buffer; 
    StreamingDloadSetEccStateRequest packet;
    packet.command = kStreamingDloadSetEcc;
    packet.status = status;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadSetEccResponse, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief openMode - Open mode
*
* @param uint8_t mode - The mode to open
*   @see qc/streaming_dload.h
* @return int
*/
int StreamingDloadSerial::openMode(uint8_t mode)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;    
    StreamingDloadOpenRequest packet;
    packet.command = kStreamingDloadOpen;
    packet.mode = mode;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadOpened, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief closeMode - Close the current opened mode
*
* @return int
*/
int StreamingDloadSerial::closeMode()
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer; 
    StreamingDloadCloseRequest packet;
    packet.command = kStreamingDloadClose;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadClosed, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief openMultiImage - open mode for multi image devices
*
* @param uint8_t imageType - The image type to open
*   @see qc/streaming_dload.h
*
* @return int
*/
int StreamingDloadSerial::openMultiImage(uint8_t imageType)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;
    StreamingDloadOpenMultiImageRequest packet;
    packet.command = kStreamingDloadOpenMultiImage;
    packet.type = imageType;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadOpenedMultiImage, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief readAddress - Read x bytes from starting address
*                      into a memory allocated array
*
* @param uint32_t address - The starting address
* @param size_t length - The length to read from address
* @param uint8_t** - The memory allocated array containing the read data until success or error encountered.
* @param size_t& - The size of the memory allocated data
* @param size_t stepSize - The amount to request per read operation. The max size is 1024.
*
* @return int
*/
int StreamingDloadSerial::readAddress(uint32_t address, size_t length, uint8_t** data, size_t& dataSize, size_t stepSize)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;

    uint8_t* out = new uint8_t[length];
    size_t outSize = length;

    uint8_t buffer[STREAMING_DLOAD_MAX_RX_SIZE];

    dataSize = 0;

    StreamingDloadReadRequest packet;
    packet.command = kStreamingDloadRead;

    StreamingDloadReadResponse* readRx;

    if (stepSize > state.hello.maxPreferredBlockSize) {
        stepSize = state.hello.maxPreferredBlockSize;
    }

    do {
        packet.address = address + dataSize;
        packet.length = length <= stepSize ? length : stepSize;

        LOGE("Requesting %lu bytes from %08X\n", packet.length, packet.address);

        txSize = write((uint8_t*)&packet, sizeof(packet));

        if (!txSize) {
            LOGE("Wrote 0 bytes requesting to read %lu bytes from 0x%08X\n", packet.length, packet.address);
            return kStreamingDloadIOError;
        }

        rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

        if (!rxSize) {
            LOGE("Device did not respond\n");
            delete out;
            return kStreamingDloadIOError;
        }

        if (!isValidResponse(kStreamingDloadReadData, buffer, rxSize)) {
            LOGE("Invalid Response\n");
            delete out;
            return kStreamingDloadError;
        }
        
        size_t newSize = dataSize + rxSize;
        if (newSize > outSize) {             
            out = (uint8_t*)realloc(out, newSize);
            outSize = newSize;
        }

        readRx = (StreamingDloadReadResponse*)&buffer[0];

        if (readRx->address != packet.address) {
            LOGE("Packet address and response address differ\n");
            return kStreamingDloadError;
        }
        
        memcpy(&out[dataSize], readRx->data, rxSize - sizeof(packet));
        dataSize += (rxSize - sizeof(packet));
        
        if (length <= dataSize) {
            break; //done
        }
            
    } while (true);

    *data = out;
    
    LOGI("\n\n\nFinal Data Size: %lu bytes\n", dataSize);
    hexdump(out, dataSize);

    return kStreamingDloadSuccess;
}

/**
* @brief readAddress - Read x bytes from starting address
*                      into a std::vector<uint8_t> container
*
* @param uint32_t address - The starting address
* @param size_t length - The length to read from address
* @param std::vector<uint8_t> &out - The populated vector containing the read data until success or error encountered.
* @param size_t stepSize - The amount to request per read operation. The max size is 1024.
*
* @return int
*/
int StreamingDloadSerial::readAddress(uint32_t address, size_t length, std::vector<uint8_t> &out, size_t stepSize)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;

    StreamingDloadReadRequest packet = {};
    packet.command = kStreamingDloadRead;
    packet.address = address;

    StreamingDloadReadResponse* readRx;

    if (out.size()) {
        out.clear();
    }

    out.reserve(length);

    std::vector<uint8_t> tmp;
    tmp.reserve(packet.length + sizeof(packet));

    if (stepSize > state.hello.maxPreferredBlockSize) {
        stepSize = state.hello.maxPreferredBlockSize;
    }

    do {
        packet.address = packet.address + out.size();       
        packet.length = length <= stepSize ? length : stepSize;
        
        LOGE("Requesting %lu bytes from %08X\n", packet.length, packet.address);

        txSize = write((uint8_t*)&packet, sizeof(packet));
        
        if (!txSize) {
            LOGE("Wrote 0 bytes requesting to read %lu bytes from 0x%08X\n", packet.length, packet.address);
            return kStreamingDloadIOError;
        }

        // read accounting for additional room for the data to be read
        // and the extra packet data in the header of the response
        // and possibly escaped content
        rxSize = read(tmp, STREAMING_DLOAD_MAX_RX_SIZE);

        if (!rxSize) {
            LOGE("Device did not respond to request to read %lu bytes from 0x%08X\n", packet.length, packet.address);
            return kStreamingDloadIOError;
        }
        
        if (!isValidResponse(kStreamingDloadReadData, tmp)) {
            LOGD("Invalid response in request to read %lu bytes from 0x%08X. Data read so far is %lu bytes.\n", packet.length, packet.address, out.size());
            return kStreamingDloadError;
        }

        readRx = (StreamingDloadReadResponse*)&tmp[0];

        if (readRx->address != packet.address) {
            LOGE("Packet address and response address differ\n");
            return kStreamingDloadError;
        }

        // remove the command code, address, and length to only
        // keep the real data
        tmp.erase(tmp.end() - rxSize, (tmp.end() - rxSize) + sizeof(packet));

        out.reserve(out.size() + tmp.size());
        std::copy(tmp.begin(), tmp.end(), std::back_inserter(out));

        if (length <= out.size()) {
            LOGE("Final read size is %lu bytes\n", out.size());
            break; //done
        }

        // clear out the tmp buffer
        // for a possible next read
        tmp.clear();

    } while (true);

    return kStreamingDloadSuccess;
}

/**
* @brief readAddress - Read x bytes from starting address
*                      into a file pointer
*
* @param uint32_t address - The starting address
* @param size_t length - The length to read from address
* @param FILE* out - The file pointer to write the data to
* @param size_t& outSize - The amount of bytes written to the file until success or error encountered.
* @param size_t stepSize - The amount to request per read operation. The max size is 1024.
*
* @return int
*/
int StreamingDloadSerial::readAddress(uint32_t address, size_t length, std::ofstream& out, size_t &outSize, size_t stepSize)
{
    if (!isOpen() || !out.is_open()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;

    StreamingDloadReadRequest packet = {};
    packet.command = kStreamingDloadRead;
    packet.address = address;

    outSize = 0;

    uint8_t tmp[STREAMING_DLOAD_MAX_RX_SIZE];

    if (stepSize > state.hello.maxPreferredBlockSize) {
        stepSize = state.hello.maxPreferredBlockSize;
    }

    do {
        packet.address = packet.address + outSize;
        packet.length = length <= stepSize ? length : stepSize;

        LOGE("Requesting %lu bytes from %08X\n", packet.length, packet.address);

        txSize = write((uint8_t*)&packet, sizeof(packet));

        if (!txSize) {
            LOGE("Wrote 0 bytes requesting to read %lu bytes from 0x%08X\n", packet.length, packet.address);
            return kStreamingDloadIOError;
        }

        rxSize = read(tmp, STREAMING_DLOAD_MAX_RX_SIZE);

        if (!rxSize) {
            LOGE("Device did not respond to request to read %lu bytes from 0x%08X\n", packet.length, packet.address);
            return kStreamingDloadIOError;
        }

        if (!isValidResponse(kStreamingDloadReadData, tmp, rxSize)) {
            LOGE("Invalid response in request to read %lu bytes from 0x%08X. Data read so far is %lu bytes.\n", packet.length, packet.address, outSize);
            return kStreamingDloadError;
        }

        StreamingDloadReadResponse* resp = (StreamingDloadReadResponse*)&tmp;
    
        if (resp->address != packet.address) {
            LOGE("Packet address and response address differ\n");
            return kStreamingDloadError;
        }

        size_t dataSize = rxSize - (sizeof(resp->command) + sizeof(resp->address));

        out.write((char *)resp->data, dataSize);

        if (packet.length != dataSize) {
            LOGE("Data Written Does Not Match Requested Length. Requested %lu bytes wrote %lu bytes\n", packet.length, dataSize);
        }

        outSize += dataSize;

    } while (outSize < length);

    LOGD("Final read size is %lu bytes\n", outSize);

    return kStreamingDloadSuccess;
}

/**
* @brief writePartitionTable - Writes partition table for sessions that require it.
*
* You should send this request with overwrite set to false first and check the outStatus
* If it is ok the write will take place. If it is not ok then to write the file you will
* need to force it with overwrite set to true.
*
* @param std::string filePath - The path to the partition table (max 512 bytes)
* @param uint8_t& - The status reported back by the device.
*   @see qc/streaming_dload.h
* @param bool overwritte - Defaults to false. If true will overwrite even if error is reported.
*/
int StreamingDloadSerial::writePartitionTable(std::string fileName, uint8_t& outStatus, bool overwrite)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }
    
    std::ifstream file(fileName.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        LOGE("Could Not Open File %s\n", fileName.c_str());
        return kStreamingDloadIOError;
    }

    file.seekg(0, file.end);

    size_t fileSize = (size_t)file.tellg();
    
    file.seekg(0, file.beg);
    
    if (fileSize > 512) {
        LOGE("File can\'t exceed 512 bytes");
        file.close();
        return kStreamingDloadError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer;
    StreamingDloadPartitionTableRequest packet = {};

    packet.command = kStreamingDloadPartitionTable;
    packet.overrideExisting = overwrite;

    file.read((char *)&packet.data, fileSize);
    file.close();

    txSize = write((uint8_t*)&packet, sizeof(packet));
    
    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadPartitionTableReceived, &buffer[0], rxSize)) {
        return kStreamingDloadError;
    }

    StreamingDloadPartitionTableResponse* resp = (StreamingDloadPartitionTableResponse*)&buffer[0];

    outStatus = resp->status;

    if (resp->status > 1) {
        LOGE("Received Error Response %02X\n", resp->status);
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
    
}

int StreamingDloadSerial::streamWrite(uint32_t address, uint8_t* data, size_t dataSize, bool unframed)
{
    uint8_t packetBuffer[STREAMING_DLOAD_MAX_TX_SIZE] = {};
    uint8_t responseBuffer[STREAMING_DLOAD_MAX_RX_SIZE] = {};

    StreamingDloadStreamWriteRequest* packet = (StreamingDloadStreamWriteRequest*)packetBuffer;

    if (dataSize > state.hello.maxPreferredBlockSize) {

        size_t bytesWritten = 0;
        size_t dataSegmentSize = state.hello.maxPreferredBlockSize;
        

        do {
            packet->command = unframed ? kStreamingDloadUnframedStreamWrite : kStreamingDloadStreamWrite;
            packet->address = address + bytesWritten;

            if (dataSegmentSize > (dataSize - bytesWritten)) {
                dataSegmentSize = dataSize - bytesWritten;
            }

            memcpy(packet->data, &data[bytesWritten], dataSegmentSize);

            size_t txSize = write((uint8_t*)packet, sizeof(packet->command) + sizeof(packet->address) + dataSize, unframed);

            if (!txSize) {
                LOGE("Wrote 0 bytes\n");
                return kStreamingDloadIOError;
            }

            size_t rxSize = read(responseBuffer, STREAMING_DLOAD_MAX_RX_SIZE, unframed);

            if (!rxSize) {
                LOGE("Device did not respond\n");
                return kStreamingDloadIOError;
            }

            StreamingDloadStreamWriteResponse* response = (StreamingDloadStreamWriteResponse*) responseBuffer;

            if (response->address != packet->address) {
                LOGE("Response address %04X differs from requeasted write address %04X\n", response->address, packet->address);
                return kStreamingDloadError;
            }

            bytesWritten += dataSegmentSize;

        } while (bytesWritten < dataSize);

    } else {
        packet->command = unframed ? kStreamingDloadUnframedStreamWrite : kStreamingDloadStreamWrite;;
        packet->address = address;
        memcpy(packet->data, data, dataSize);

        size_t bytesWritten = write((uint8_t*)packet, sizeof(packet->command) + sizeof(packet->address) + dataSize, (!unframed));

        if (!bytesWritten) {
            LOGE("Wrote 0 bytes\n");
            return kStreamingDloadIOError;
        }

        size_t rxSize = read(responseBuffer, STREAMING_DLOAD_MAX_RX_SIZE, (!unframed));

        if (!rxSize) {
            LOGE("Device did not respond\n");
            return kStreamingDloadIOError;
        }

        if (!isValidResponse(unframed ? kStreamingDloadUnframedStreamWriteResponse : kStreamingDloadBlockWritten, responseBuffer, rxSize)) {
            return kStreamingDloadError;
        }

        StreamingDloadStreamWriteResponse* response = (StreamingDloadStreamWriteResponse*)responseBuffer;

        if (response->address != packet->address) {
            LOGE("Response address %04X differs from requeasted write address %04X\n", response->address, packet->address);
            return kStreamingDloadError;
        }

    }

    return kStreamingDloadSuccess;
}

/**
* @brief readQfprom - Havent found a device or mode to use this in
*/
int StreamingDloadSerial::readQfprom(uint32_t rowAddress, uint32_t addressType)
{
    if (!isOpen()) {
        LOGE("Port Not Open\n");
        return kStreamingDloadIOError;
    }

    size_t txSize, rxSize;
    std::vector<uint8_t> buffer; 
    StreamingDloadQfpromReadRequest packet;
    packet.command = kStreamingDloadQfpromRead;
    packet.addressType = addressType;
    packet.rowAddress = rowAddress;

    txSize = write((uint8_t*)&packet, sizeof(packet));

    if (!txSize) {
        LOGE("Wrote 0 bytes\n");
        return kStreamingDloadIOError;
    }

    rxSize = read(buffer, STREAMING_DLOAD_MAX_RX_SIZE);

    if (!rxSize) {
        LOGE("Device did not respond\n");
        return kStreamingDloadIOError;
    }

    if (!isValidResponse(kStreamingDloadQfpromReadResponse, buffer)) {
        return kStreamingDloadError;
    }

    return kStreamingDloadSuccess;
}

/**
* @brief isValidResponse
*
* @param uint8_t expectedCommand,
* @param uint8_t* response
* @param size_t responseSize
*/
bool StreamingDloadSerial::isValidResponse(uint8_t expectedCommand, uint8_t* response, size_t responseSize)
{
    if (response[0] != expectedCommand) {
        if (response[0] == kStreamingDloadLog) {
            StreamingDloadLogResponse* packet = (StreamingDloadLogResponse*)response;
            LOGE("Received Log Response\n");
            memcpy((uint8_t*)&state.lastLog, packet, responseSize);
        } else if (response[0] == kStreamingDloadError) {
            StreamingDloadErrorResponse* packet = (StreamingDloadErrorResponse*)response;
            LOGE("Received Error Response %02X - %s\n", packet->code, getNamedError(packet->code));
            memcpy((uint8_t*)&state.lastError, packet, responseSize);
        } else {
            LOGE("Unexpected Response\n");
        }
        return false;
    }

    return true;
}

/**
* @brief isValidResponse
*
* @param uint8_t expectedCommand,
* @param std::vector<uint8_t> response
*/
bool StreamingDloadSerial::isValidResponse(uint8_t expectedCommand, std::vector<uint8_t> &data)
{
    return isValidResponse(expectedCommand, &data[0], data.size());
}

/**
* @brief getNamedError - Get a named error from an error code
*
* @param uint8_t code - The error code
*
* @return const char* msg
*/
const char* StreamingDloadSerial::getNamedError(uint8_t code)
{
    switch (code) {
        case kStreamingDloadErrorInvalidDestinationAddress:       return "Invalid Destination Address";
        case kStreamingDloadErrorInvalidLength:                          return "Invalid Length";
        case kStreamingDloadErrorUnexpectedPacketEnd:                return "Unexpected End of Packet";
        case kStreamingDloadErrorInvalidCommand:                         return "Invalid Command";
        case kStreamingDloadErrorWrongFlashIntelligentId:              return "Wrong Flash Intelligent ID";
        case kStreamingDloadErrorBadProgrammingVoltage:                 return "Bad Programming Voltage";
        case kStreamingDloadErrorWriteVerifyFailed:                     return "Write Verify Failed";
        case kStreamingDloadErrorIncorrectSecurityCode:                 return "Incorrect Security Code";
        case kStreamingDloadErrorCannotPowerDown:                       return "Cannot Power Down";
        case kStreamingDloadErrorNandProgrammingUnsupported:    return "NAND Flash Programming Not Supported";
        case kStreamingDloadErrorCommandOutOfSequence:                  return "Command Out of Sequence";
        case kStreamingDloadErrorCloseDidNotSucceed:                   return "Close Did Not Succeed";
        case kStreamingDloadErrorIncompatibleFeatureBits:              return "Incompatible Features Bits";
        case kStreamingDloadErrorOutOfSpace:                            return "Out of Space";
        case kStreamingDloadErrorInvalidSecurityMode:                   return "Invalid Security Mode";
        case kStreamingDloadErrorMultiImageNandUnsupported:          return "Multi Image NAND Not Supported";
        case kStreamingDloadErrorPowerOffCmdUnsupported:         return "Power Off Command Not Supported";
        default: 
            return "Unknown";
    }
}

/**
* @brief getNamedOpenMode - Get a named mode from an open mode integer
*
* @param uint8_t mode - The mode
*
* @return const char* mode
*/
const char* StreamingDloadSerial::getNamedOpenMode(uint8_t mode)
{
    switch (mode) {
        case kStreamingDloadOpenModeBootloader:     return "Bootloader Download";
        case kStreamingDloadOpenModeBootableImage: return "Bootable Image Download";
        case kStreamingDloadOpenModeCefs:     return "CEFS Image Download";
        default:
            return "Unknown";
    }
}

/**
* @brief getNamedMultiImage - Get a named image from an open multi image type
*
* @param uint8_t imageType - The image type id
*
* @return const char* name
*/
const char* StreamingDloadSerial::getNamedMultiImage(uint8_t imageType)
{
    switch (imageType) {
        case kStreamingDloadOpenModeMultiNone:          return "None";
        case kStreamingDloadOpenModeMultiPbl:           return "Primary boot loader";
        case kStreamingDloadOpenModeMultiQcsblhHdCfg:    return "Qualcomm secondary boot loader header and config data";
        case kStreamingDloadOpenModeMultiQcsbl:         return "Qualcomm secondary boot loader";
        case kStreamingDloadOpenModeMultiOemsbl:        return "OEM secondary boot loader";
        case kStreamingDloadOpenModeMultiAmss:          return "AMSS modem executable";
        case kStreamingDloadOpenModeMultiApps:          return "AMSS applications executable";
        case kStreamingDloadOpenModeMultiObl:           return "MSM6250 OTP boot loader";
        case kStreamingDloadOpenModeMultiFotaui:        return "FOTA UI binary";
        case kStreamingDloadOpenModeMultiCefs:          return "Compact EFS2 image";
        case kStreamingDloadOpenModeMultiAppsBL:        return "AMSS applications boot loader";
        case kStreamingDloadOpenModeMultiApps_CEFS:     return "Apps CEFS image";
        case kStreamingDloadOpenModeMultiFlashBin:     return "Flash.bin for Windows Mobile";
        case kStreamingDloadOpenModeMultiDsp1:          return "DSP1 runtime image";
        case kStreamingDloadOpenModeMultiCustom:        return "User-defined partition";
        case kStreamingDloadOpenModeMultiDbl:           return "DBL image for Secure Boot 2.0";
        case kStreamingDloadOpenModeMultiOsbl:          return "OSBL image for Secure Boot 2.0";
        case kStreamingDloadOpenModeMultiFsbl:          return "FSBL image for Secure Boot 2.0";
        case kStreamingDloadOpenModeMultiDsp2:          return "DSP2 executable";
        case kStreamingDloadOpenModeMultiRaw:           return "Apps EFS2 raw image ";
        case kStreamingDloadOpenModeMultiRofs1:         return "ROFS1 - Symbian";
        case kStreamingDloadOpenModeMultiRofs2:         return "ROFS2 - Symbian";
        case kStreamingDloadOpenModeMultiRofs3:         return "ROFS3 - Symbian";
        case kStreamingDloadOpenModeMultiEmmcUser:     return "EMMC USER partition";
        case kStreamingDloadOpenModeMultiEmmcBoot0:    return "EMMC BOOT0 partition";
        case kStreamingDloadOpenModeMultiEmmcBoot1:    return "EMMC BOOT1 partition";
        case kStreamingDloadOpenModeMultiRpmb:     return "EMMC RPMB";
        case kStreamingDloadOpenModeMultiGpp1:     return "EMMC GPP1";
        case kStreamingDloadOpenModeMultiGpp2:     return "EMMC GPP2";
        case kStreamingDloadOpenModeMultiGpp3:     return "EMMC GPP3";
        case kStreamingDloadOpenModeMultiGpp4:     return "EMMC GPP4";
        default:
            return "Unknown";
    }
}