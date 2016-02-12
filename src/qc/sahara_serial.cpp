
/**
* LICENSE PLACEHOLDER
*
* @file sahara_serial.cpp
* @class SaharaSerial
* @package OpenPST
* @brief Sahara protocol serial port implementation
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "sahara_serial.h"

using namespace OpenPST::QC;

/**
* @brief SaharaSerial
*
* @param std::string port
* @param int baudrate
* @param serial::Timeout - Timeout, defaults to 1000ms
*/
SaharaSerial::SaharaSerial(std::string port, int baudrate, serial::Timeout timeout) :
    serial::Serial(port, baudrate, timeout)
{

}

/**
* @brief ~SaharaSerial
*/
SaharaSerial::~SaharaSerial()
{
}

/**
* @brief write
* @overload Serial:: write(uint8_t *data, size_t size)
* @return size_t
*/
size_t SaharaSerial::write(uint8_t *data, size_t size)
{
    size_t bytesWritten = Serial::write(data, size);
    hexdump_tx(data, bytesWritten);
    return bytesWritten;
}

/**
* @brief read
* @overload Serial::read(uint8_t *buf, size_t size)
* @return size_t
*/
size_t SaharaSerial::read(uint8_t *buf, size_t size)
{
    size_t bytesRead = Serial::read(buf, size);
    hexdump_rx(buf, bytesRead);
    return bytesRead;
}

/**
* @brief write
* @overload Serial::write(std::vector<uint8_t> &data)
* @return size_t
*/
size_t SaharaSerial::write(std::vector<uint8_t> &data)
{
    size_t bytesWritten = Serial::write(data);
    hexdump_tx(&data[0], bytesWritten);
    return bytesWritten;
}

/**
* @brief read
* @overload Serial::read(std::vector<uint8_t> &buffer, size_t size)
* @return size_t
*/
size_t SaharaSerial::read(std::vector<uint8_t> &buffer, size_t size)
{
    size_t bytesRead = Serial::read(buffer, size);
    hexdump_rx(&buffer[0], bytesRead);
    return bytesRead;
}


SaharaHelloRequest SaharaSerial::readHello()
{
	size_t rxSize;
	SaharaHelloRequest ret = {};

	if (!(rxSize = read(reinterpret_cast<uint8_t*>(&ret), sizeof(ret)))) {
		throw SaharaSerialError("Did not receive hello. Not in sahara mode, already in a session, or requires restart");
    }

	validateResponse(kSaharaCommandHello, reinterpret_cast<SaharaHeader*>(&ret), rxSize);

	flush();

	return ret;
}


SaharaState SaharaSerial::sendHello(uint32_t mode, uint32_t version, uint32_t minVersion, uint32_t status)
{
    size_t rxSize;
    SaharaHelloResponse packet = {};
	SaharaState ret = {};
	std::vector<uint8_t> buffer;
	
    packet.header.command	= kSaharaCommandHelloResponse;
	packet.header.size		= sizeof(packet);
    packet.status			= status;
	packet.mode				= mode;
    packet.version			= version;
    packet.minVersion		= minVersion;

	write((uint8_t*)&packet, sizeof(packet));

    try {
		rxSize = read(buffer, sizeof(ret));		
    }  catch (serial::IOException e) {
        // sometimes (at least in memory debug mode) the device
        // will reset itself and the hello needs to be re-read and
        // sent
        close();

        sleep(5000); // give it some time to come back

        open();

		readHello();

        write((uint8_t*)&packet, sizeof(packet)); // resend the hello response

		rxSize = read(buffer, SAHARA_MAX_PACKET_SIZE);	
    }
	
	flush();

	if (packet.mode == kSaharaModeCommand && isValidResponse(kSaharaCommandReady, reinterpret_cast<SaharaHeader*>(&buffer[0]), rxSize)) {
		ret.mode = packet.mode;
		memcpy(&ret.clientCommand, &buffer[0], sizeof(ret.clientCommand));
    } else if (packet.mode == kSaharaModeImageTxPending && isValidResponse(kSaharaCommandReadData, reinterpret_cast<SaharaHeader*>(&buffer[0]), rxSize)) {
		ret.mode = packet.mode;
		memcpy(&ret.imageTransfer, &buffer[0], sizeof(ret.imageTransfer));
    } else if (packet.mode == kSaharaModeMemoryDebug && isValidResponse(kSaharaCommandMemoryDebug, reinterpret_cast<SaharaHeader*>(&buffer[0]), rxSize)) {
		ret.mode = packet.mode;
		memcpy(&ret.memoryDebug, &buffer[0], sizeof(ret.memoryDebug));
    } else {
		throw SaharaSerialError("Unexpected or unsupported mode");
    }

	return ret;
}


void SaharaSerial::switchMode(uint32_t mode)
{
	size_t rxSize;
	SaharaSwitchModeRequest packet = {};
	SaharaState ret = {};
	std::vector<uint8_t> buffer;

	LOGD("Requesting Mode Switch to %s (0x%02x)\n", getNamedMode(mode).c_str(), mode);
	    
    packet.header.command	= kSaharaCommandSwitchMode;
    packet.header.size		= sizeof(packet);
    packet.mode				= mode;
	
	write(reinterpret_cast<uint8_t*>(&packet), packet.header.size);
}

SaharaState SaharaSerial::switchModeAndHello(uint32_t mode)
{
	switchMode(mode);
	
	readHello();
	
	return sendHello(mode);
}

std::vector<uint8_t> SaharaSerial::sendClientCommand(uint32_t command)
{
	size_t rxSize;
	size_t dataSize;
	size_t totalRead = 0;
	SaharaClientCommandRequest packet = {};
	SaharaClientCommandResponse resp = {};
	SaharaClientCommandExecuteDataRequest execDataPacket = {};
	std::vector<uint8_t> ret;

	LOGD("Sending Client Command: 0x%02x - %s\n", command, getNamedClientCommand(command).c_str());
	    
    packet.header.command	= kSaharaCommandExecute;
    packet.header.size		= sizeof(packet);
    packet.command			= command;

	write((uint8_t*)&packet, sizeof(packet));

	if (!(rxSize = read(reinterpret_cast<uint8_t*>(&resp), sizeof(resp)))) {
		throw SaharaSerialError("No response from device");
    }

	flush();

	validateResponse(kSaharaCommandExecuteResponse, reinterpret_cast<SaharaHeader*>(&resp), rxSize);

	dataSize = resp.size;

	execDataPacket.header.command = kSaharaCommandExecuteData;
	execDataPacket.header.size	  = sizeof(SaharaClientCommandExecuteDataRequest);
	execDataPacket.command		  = command;

	write((uint8_t*)&execDataPacket, sizeof(execDataPacket));

    do {

		if (!(rxSize = read(ret, SAHARA_MAX_PACKET_SIZE))) {
			throw SaharaSerialError("No response from device");
        }

		validateResponse(NULL, reinterpret_cast<SaharaHeader*>(&ret), rxSize);

	} while (ret.size() < dataSize);

#ifdef DEBUG
    LOGD("========\nDumping Data For Command: %s (%02X) - %lu bytes of expected %lu\n========\n\n",
		getNamedClientCommand(command).c_str(), command,
		ret.size(), dataSize
    );
	hexdump(reinterpret_cast<uint8_t*>(&ret[0]), ret.size());
#endif

	flush();

	return ret;
}


size_t SaharaSerial::sendImage(std::string filePath, size_t initialRequestSize)
{
	size_t txSize;
	size_t fileSize;
	size_t totalSent = 0;
	size_t totalRequestLength = 0;
	std::vector<char> fileBuffer;
	SaharaReadDataRequest nextOffset = {};
	std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
	
    if (!file.is_open()) {
		std::stringstream ss;
		ss << "Could Not Open File " << filePath;
		throw SaharaSerialError(ss.str());
    }

    file.seekg(0, file.end);
    fileSize = (size_t)file.tellg();
    file.seekg(0, file.beg);

    LOGD("Loaded File %s With Size %lu\n", filePath.c_str(), fileSize);

	file.read(&fileBuffer[0], initialRequestSize);

	nextOffset.size = initialRequestSize;

    while (totalSent != fileSize) {

		totalRequestLength += nextOffset.size;

		fileBuffer.resize(nextOffset.size);

		if (fileSize < (totalSent + nextOffset.size)) {
			nextOffset.size = fileSize - totalSent;
        }

		file.read(&fileBuffer[0], nextOffset.size);

		txSize = write(reinterpret_cast<uint8_t*>(&fileBuffer), nextOffset.size);

        totalSent += txSize;

		try {
			nextOffset = readNextImageOffset();
		} catch (...) {
			file.close();
			throw;
		}

		if (nextOffset.offset == 0x00 && nextOffset.size == 0x00) {
            break;
        }
    }

    file.close();

	return totalSent;
}

/**
* @brief readNextImageChunkSize
* @return std::pair<uint32_t, size_t> - The next offset and size to send
*/
SaharaReadDataRequest SaharaSerial::readNextImageOffset()
{
	size_t rxSize;
	SaharaReadDataRequest ret = {};
	
	if (!(rxSize = read(reinterpret_cast<uint8_t*>(&ret), sizeof(ret)))) {
		 throw SaharaSerialError("No response from device");
    }

	flush();

	validateResponse(kSaharaCommandReadData, reinterpret_cast<SaharaHeader*>(&ret), rxSize);

	if (ret.header.command == kSaharaCommandEndImageTransfer) {
		ret.header.command = kSaharaCommandReadData; 
		ret.size   = sizeof(ret);
		ret.offset = 0x00;
		ret.size   = 0x00;
	}

    return ret;
}


size_t SaharaSerial::readMemory(uint32_t address, size_t size, std::vector<uint8_t>&out)
{
	SaharaMemoryReadRequest packet;
	size_t rxSize;
	size_t readSoFar = 0;
	std::vector<uint8_t> buffer;

	out.reserve(size);
	buffer.reserve(size > SAHARA_MAX_MEMORY_REQUEST_SIZE ? SAHARA_MAX_MEMORY_REQUEST_SIZE : size);
	
	packet.header.command = kSaharaCommandMemoryRead;
    packet.header.size	= sizeof(packet);

	while (readSoFar < size) {
		
		packet.address = address + readSoFar; 
		
		if (size > SAHARA_MAX_MEMORY_REQUEST_SIZE) {
			packet.size = SAHARA_MAX_MEMORY_REQUEST_SIZE; 
			
			if ((size - readSoFar) < SAHARA_MAX_MEMORY_REQUEST_SIZE) {
				packet.size = size - readSoFar;
			}

		} else {
			packet.size = size;
		}

		write(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
		
		while (available()) {
			if (!(rxSize = read(buffer, packet.size))) {
				break;
			}

			readSoFar += rxSize;

			if (isErrorResponse(reinterpret_cast<SaharaHeader*>(&buffer[0]), rxSize)) {
				throw SaharaSerialError(getNamedErrorStatus(reinterpret_cast<SaharaEndImageTransferResponse*>(&buffer[0])->status));
			}

			std::copy(buffer.begin(), buffer.end(), std::back_inserter(out));

			buffer.empty();
		}				
	}

	return readSoFar;
}

/**
* @brief readMemory - Read size starting from address and
*                     save the result into the specified outFilePath
*
* @param uint32_t address - The starting address to read from
* @param size_t size - If the size is over SAHARA_MAX_MEMORY_REQUEST_SIZE, it will read in chunks of SAHARA_MAX_MEMORY_REQUEST_SIZE
* @param const char* outFilePath - Path to the file to create and store the read data
* @param size_t outSize - Total size of the read data
* @return int
*/
size_t SaharaSerial::readMemory(uint32_t address, size_t size, std::string outFilePath)
{
	std::ofstream file(outFilePath, std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open()) {
		throw SaharaSerialError("Could not open file for writing");
    }

    size_t read = readMemory(address, size, file);

    file.close();

	return read;
}

/**
* @brief readMemory - Read size starting from address and
*                     save the result into an existing file pointer.
*
* @note - Will not close the file pointer handle
*
* @param uint32_t address
* @param size_t size - If the size is over SAHARA_MAX_MEMORY_REQUEST_SIZE, it will read in chunks of SAHARA_MAX_MEMORY_REQUEST_SIZE
* @param std::ofstream out - out file stream to write to
* @param size_t outSize
* @return int
*/
size_t SaharaSerial::readMemory(uint32_t address, size_t size, std::ofstream& out)
{
	std::vector<uint8_t> buffer;

	readMemory(address, size, buffer);

	out.write(reinterpret_cast<char*>(&buffer[0]), buffer.size());

	return buffer.size();
}


void SaharaSerial::sendDone()
{
	size_t rxSize;
	SaharaDoneRequest packet = {};
	SaharaDoneResponse resp = {};

    packet.header.command = kSaharaCommandDone;
    packet.header.size = sizeof(packet);

    write((uint8_t*)&packet, sizeof(packet));
	
	if (!(rxSize = read(reinterpret_cast<uint8_t*>(&resp), sizeof(resp)))) {
		 throw SaharaSerialError("No response from device");
    }

	flush();

	validateResponse(kSaharaCommandDoneResponse, reinterpret_cast<SaharaHeader*>(&resp), rxSize);
}

void SaharaSerial::sendReset()
{
    size_t rxSize;
	SaharaResetRequest packet = {};
	SaharaResetResponse resp = {};

    packet.header.command = kSaharaCommandReset;
    packet.header.size = sizeof(packet);

    try {
		write(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
		rxSize = read(reinterpret_cast<uint8_t*>(&resp), sizeof(resp));
    } catch (...) {
		close();
		return;
    }

	validateResponse(kSaharaCommandResetResponse, reinterpret_cast<SaharaHeader*>(&resp), rxSize);

    close();
}

void SaharaSerial::close()
{
    serial::Serial::close();
}


void SaharaSerial::validateResponse(uint32_t expectedResponseCommand, SaharaHeader* data, size_t dataSize)
{
	std::stringstream ss; 
	SaharaEndImageTransferResponse* error;

	if (!isValidResponse(expectedResponseCommand, data, dataSize)) {
		ss << "Devices responded with an error: " << 
			getNamedErrorStatus(reinterpret_cast<SaharaEndImageTransferResponse*>(data)->status);

		throw SaharaSerialError(ss.str());
	}
}

bool SaharaSerial::isValidResponse(uint32_t expectedResponseCommand, SaharaHeader* data, size_t dataSize)
{

	if (dataSize == 0) {
        LOGD("No data to validate");
        return false;
    } else if (isErrorResponse(data, dataSize)) {
		LOGD("Error Response");
		return false;
	} else if (NULL == expectedResponseCommand || data->command == expectedResponseCommand) {
		return true;
	}

    return false; 
}

bool SaharaSerial::isErrorResponse(SaharaHeader* data, size_t dataSize)
{
	return data->command == kSaharaCommandEndImageTransfer &&
		dataSize == sizeof(SaharaEndImageTransferResponse)/* &&
		reinterpret_cast<SaharaEndImageTransferResponse*>(data)->status != kSaharaStatusSuccess &&
		reinterpret_cast<SaharaEndImageTransferResponse*>(data)->status < kSaharaStatusMax*/;
}

std::string SaharaSerial::getNamedMode(uint32_t mode)
{
    switch (mode) {
        case kSaharaModeImageTxPending:  return "Image Transfer Pending";
        case kSaharaModeImageTxComplete: return "Image Transfer Complete";
        case kSaharaModeMemoryDebug:      return "Memory Debug";
        case kSaharaModeCommand:           return "Command Mode";
        default:
            return "Unknown";
    }
}

std::string SaharaSerial::getNamedClientCommand(uint32_t command)
{
    switch (command) {
        case kSaharaClientCmdNop:						return "NOP";
        case kSaharaClientCmdSerialNumRead:				return "Read Serial Number";
        case kSaharaClientCmdMsmHWIDRead:				return "Read MSM HW ID";
        case kSaharaClientOemPkHashRead:				return "Read OEM PK Hash";
        case kSaharaClientCmdSwitchDMSS:				return "Switch To DMSS DLOAD";
        case kSaharaClientCmdSwitchToStreamingDload:    return "Switch To Streaming DLOAD";
        case kSaharaClientCmdReadDebugData:				return "Read Debug Data";
        case kSaharaClientCmdGetSblVersion:				return "Read SBL SW Version";
        default:										return "Unknown";
    }
}

std::string SaharaSerial::getNamedErrorStatus(uint32_t status)
{
    switch (status) {
        case kSaharaStatusSuccess:                     return "Success";
        case kSaharaStatusInvalidCmd:                  return "Invalid Command";
        case kSaharaStatusProtocolMismatch:            return "Protocol Mismatch";
        case kSaharaStatusInvalidTargetProtocol:       return "Invalid Target Protocol";
        case kSaharaStatusInvalidHostProtocol:         return "Invalid Host Protocol";
        case kSaharaStatusInvalidPacketSize:           return "Invalid Packet Size";
        case kSaharaStatusUnexpectedImageId:           return "Unexpected Image ID";
        case kSaharaStatusInvalidHeaderSize:           return "Invalid Header Size";
        case kSaharaStatusInvalidDataSize:             return "Invalid Data Size";
        case kSaharaStatusInvalidImageType:            return "Invalid Image Type";
        case kSaharaStatusInvalidTxLength:             return "Invalid TX Length";
        case kSaharaStatusInvalidRxLength:             return "Invalid RX Length";
        case kSaharaStatusTxRxError:				   return "General TX RX Error";
        case kSaharaStatusUnsupportedNumPhdrs:         return "Cannot receive specified number of program headers";
        case kSaharaStatusInvalidPhdrSize:             return "Invalid data length received for program headers";
        case kSaharaStatusMultipleSharedSeg:           return "Multiple shared segments found in ELF image";
        case kSaharaStatusUninitPhdrLoc:               return "Uninitialized program header location";
        case kSaharaStatusInvalidDestAddress:          return "Invalid destination address";
        case kSaharaStatusInvalidImageHeaderSize:		return "Invalid data size receieved in image header";
        case kSaharaStatusInvalidElfHeader:             return "Invalid ELF header received";
        case kSaharaStatusUnknownError:					return "Unknown Host Error";
        case kSaharaStatusTimeoutRx:                    return "RX Timeout";
        case kSaharaStatusTimeoutTx:                    return "TX Timeout";
        case kSaharaStatusInvalidMode:					return "Invalid Host Mode";
        case kSaharaStatusInvalidMemoryRead:           return "Invalid Memory Read";
        case kSaharaStatusInvalidDataSizeRequest:     return "Invalid Data Size Request";
        case kSaharaStatusMemoryDebugNotSupported:    return "Memory Debug Not Supported";
        case kSaharaStatusInvalidModeSwitch:           return "Invalid Mode Switch";
        case kSaharaStatusExecFailure:              return "Command Execute Failure";
        case kSaharaStatusExecCmdInvalidParam:        return "Invalid Command Parameter";
        case kSaharaStatusExecCmdUnsupported:          return "Command Unsupported";
        case kSaharaStatusExecDataInvalid:  return "Invalid Client Command";
        case kSaharaStatusHashTableAuthFailure:       return "Hash Table Auth Failure";
        case kSaharaStatusHashVerificationFailure:     return "Hash Table Verification Failure";
        case kSaharaStatusHashTableNotFound:          return "Hash Table Not Found";
        case kSaharaStatusTargetInitFailure:           return "Target Init Failure";
        case kSaharaStatusImageAuthFailure:            return "Image Auth Failure";
        case kSaharaStatusInvalidImgHashTableSize:   return "Invalid ELF Hash Table Size";
        default:
            return "Unknown";
    }
}

std::string SaharaSerial::getNamedRequestedImage(uint32_t imageId)
{
    switch (imageId) {
        case kMbnImageNone:          return "None";
        case kMbnImageOemSbl:   return "OEM SBL";
        case kMbnImageAmss:      return "AMSS";
        case kMbnImageOcbl:     return "QCSBL";
        case kMbnImageHash:      return "Hash";
        case kMbnImageAppbl:    return "APPSBL";
        case kMbnImageHostDl:        return "HOSTDL";
        case kMbnImageDsp1:          return "DSP1";
        case kMbnImageFsbl:          return "FSBL";
        case kMbnImageDbl:           return "DBL";
        case kMbnImageOsbl:          return "OSBL";
        case kMbnImageDsp2:          return "DSP2";
        case kMbnImageEhostdl:       return "EHOSTDL";
        case kMbnImageNandprg:       return "NANDPRG";
        case kMbnImageNorprg:        return "NORPRG";
        case kMbnImageRamfs1:        return "RAMFS1";
        case kMbnImageRamfs2:        return "RAMFS2";
        case kMbnImageAdspQ5:       return "ADSP Q5";
        case kMbnImageAppsKernel:   return "APPS Kernel";
        case kMbnImageBackupRamfs:   return "Backup RAMFS";
        case kMbnImageSbl1:          return "SBL1";
        case kMbnImageSbl2:          return "SBL2";
        case kMbnImageRpm:           return "RPM";
        case kMbnImageSbl3:          return "SBL3";
        case kMbnImageTz:            return "TZ";
        case kMbnImageSsdKeys:       return "SSD Keys";
        case kMbnImageGen:           return "GEN";
        case kMbnImageDsp3:          return "DSP3";
        case kMbnImageAcdb:          return "ACDB";
        case kMbnImageWdt:           return "WDT";
        case kMbnImageMba:           return "MBA";
        default:					 return "Unknown";
    }
}
