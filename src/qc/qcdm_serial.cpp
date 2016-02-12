/**
* LICENSE PLACEHOLDER
*
* @file qcdm_serial.h
* @class QcdmSerial
* @package OpenPST
* @brief QCDM serial port implementation
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#include "qcdm_serial.h"

using namespace OpenPST::QC;
using serial::IOException;

QcdmSerial::QcdmSerial(std::string port, int baudrate, serial::Timeout timeout) :
    HdlcSerial (port, baudrate, timeout)
{

}

QcdmSerial::~QcdmSerial()
{

}

QcdmVersionResponse QcdmSerial::getVersion()
{
	QcdmVersionResponse response;

	sendCommand(kDiagVerno);

    std::memcpy(&response, buffer, sizeof(response));

	return response;
} 

uint16_t QcdmSerial::getDiagVersion()
{
	sendCommand(kDiagDiagVer);

	return ((QcdmDiagVersionResponse*)&buffer)->version;
}

QcdmStatusResponse QcdmSerial::getStatus()
{
	QcdmStatusResponse response = {};

	sendCommand(kDiagStatus);

	std::memcpy(&response, buffer, sizeof(response));

	return response;
}

QcdmGuidResponse QcdmSerial::getGuid()
{
	QcdmGuidResponse response = {};

	sendCommand(kDiagGetGuid);

	std::memcpy(&response, buffer, sizeof(response));

	return response;
}

bool QcdmSerial::sendSpc(std::string spc)
{
	QcdmSpcRequest packet = {};
	QcdmSpcResponse* response;
	std::ostringstream message;

	if (spc.length() != DIAG_SPC_LENGTH) {
		message << "SPC must be " << DIAG_SPC_LENGTH << " digits";
		throw QcdmSerialInvalidArgument(message.str());
    }

	packet.command = kDiagSpc;
	std::memcpy(&packet.spc, spc.c_str(), DIAG_SPC_LENGTH);

	sendCommand(packet.command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

	response = (QcdmSpcResponse*)buffer;

	return response->status == 1;
}

bool QcdmSerial::sendPassword(std::string password)
{
	std::ostringstream message;
	
	if (password.length() != DIAG_PASSWORD_LENGTH) {
		message << "Password must be 16 " << DIAG_SPC_LENGTH << " characters";
		throw QcdmSerialInvalidArgument(message.str());
	}

	return false;
	/*
    long data = std::stoul(password, nullptr, 16);

    #ifdef _WIN32
        data = _byteswap_uint64(data);
    #else
        data = __builtin_bswap64(data);
    #endif

    QcdmPasswordRequest packet;
    memcpy(&packet.password, &data, sizeof(data));

    lastTxSize = write((uint8_t*)&packet, sizeof(packet));

    if (!lastTxSize) {
        printf("Attempted to write to device but 0 bytes were written\n");
        return DIAG_CMD_TX_FAIL;
    }

    lastRxSize = read(buffer, DIAG_MAX_PACKET_SIZE);

    if (!lastRxSize) {
        printf("Device did not respond\n");
        return DIAG_CMD_RX_FAIL;
    }

    QcdmPasswordResponse* rxPacket = (QcdmPasswordResponse*)buffer;

    return rxPacket->status;
	*/
}

void QcdmSerial::switchToDload()
{
	sendCommand(kDiagDload, false);
}

bool QcdmSerial::setPhoneMode(QcdmPhoneMode mode)
{
	QcdmPhoneModeResponse* response;
	QcdmPhoneModeRequest packet;

	packet.command = kDiagControl;
    packet.mode	   = mode;

	sendCommand(packet.command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

	response = (QcdmPhoneModeResponse*)buffer;

	return response->status ? true : false;
}

QcdmNvResponse QcdmSerial::readNV(uint16_t itemId)
{
	QcdmNvResponse response = {};
	QcdmNvRequest packet = {};

	packet.command	= kDiagNvRead;
    packet.nvItem	= itemId;

	sendCommand(packet.command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

	std::memcpy(&response, buffer, sizeof(QcdmNvResponse));

	return response;
}


bool QcdmSerial::writeNV(uint16_t itemId, uint8_t* data, size_t size)
{
	QcdmNvRequest packet;
	std::ostringstream message;
	if (size > DIAG_NV_ITEM_SIZE) {
		message << "Data is larger than max size of " << DIAG_NV_ITEM_SIZE << " bytes";
		throw QcdmSerialInvalidArgument(message.str());
	}

	// TODO: add support for short nv writes
    packet.command	= kDiagNvWrite;
    packet.nvItem	= itemId;
	memcpy(&packet.data, data, size);

	sendCommand(packet.command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

	return true;
}

QcdmNvPeekResponse QcdmSerial::peekNV(uint32_t address, uint8_t size)
{
	QcdmNvPeekRequest packet = {};
	QcdmNvPeekResponse response = {};

	if (size > DIAG_NV_PEEK_MAX_SIZE) {
		throw QcdmSerialInvalidArgument("NV peek request size is over the maximum");
	}

	packet.command = kDiagNvPeek;
	packet.address = address;
	packet.size	   = size;
	
	sendCommand(packet.command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
	
	memcpy(&response, buffer, sizeof(response));
		
	return response;
}

/**
* @brief sendHtcNvUnlock
* @param response -
* @return
*/
bool QcdmSerial::sendHtcNvUnlock()
{
	uint8_t packet[] = { kDiagSpc, 0x74, 0x64, 0x77, 0x61, 0x6F, 0x70 };

	sendCommand(NULL, packet, sizeof(packet));
	
    return true;
}

bool QcdmSerial::sendLgNvUnlock()
{

	uint8_t packet[] = { 0x33, 0x7D, 0x5F };

	sendCommand(NULL, packet, sizeof(packet));

	return true;
}


bool QcdmSerial::getLgSpc()
{
	unsigned char packet[] = { 0x11, 0x17, 0x00, 0x08 };

	sendCommand(NULL, packet, sizeof(packet));

	return true;
}

void QcdmSerial::sendCommand(uint8_t command, bool validate)
{
    QcdmGenericRequest packet = { command };

    return sendCommand(command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));
}

void QcdmSerial::sendCommand(uint8_t command, uint8_t* data, size_t size, bool validate)
{
	QcdmResponseHeader* response = (QcdmResponseHeader*)&buffer;
	size_t responseSize;

	write(data, size);

	if (!(responseSize = read(buffer, DIAG_MAX_PACKET_SIZE))) {
		throw QcdmSerialResponseError("Device did not respond");
	}

	if (validate) {
		if (command && response->command != command) {
			throw QcdmSerialResponseError("Received an unexpected response", response->command);
		} else if (!command) {
			if (isError(response->command)) {
				throw QcdmSerialResponseError("Invalid Response", response->command);
			}
		}
	}
}

bool QcdmSerial::isError(uint8_t responseCommand)
{
	switch (responseCommand) {
		case kDiagBadCmd:
		case kDiagBadParm:
		case kDiagBadLen:
		case kDiagBadMode:
		case kDiagBadSpcMode:
			return true;
	}

	return false;
}
/**
* @brief getErrorString - Returns a string with a known error
*								code stored in code
* @return string
*/
std::string QcdmSerial::getErrorString(uint8_t responseCommand)
{
	switch (responseCommand) {
		case kDiagBadCmd:		return "Bad Command";
		case kDiagBadParm:		return "Bad Parameter";
		case kDiagBadLen:		return "Bad Length";
		case kDiagBadMode:		return "Bad Mode";
		case kDiagBadSpcMode:	return "Bad SPC Mode";
		default:					return "Unknown Error";
	}

}

int QcdmSerial::getErrorLog()
{
    /*
    PACKED(typedef struct QcdmLogResponse{
        uint8_t command;
        uint8_t entries;
        uint16_t  length;
        uint8_t  logs[0];
    }) QcdmLogResponse;*/

    //kDiageatureQuery
    //kDiagLog
    QcdmGenericRequest packet = { kDiagLog };

    write(reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

    size_t rx = read(buffer, DIAG_MAX_PACKET_SIZE);
    hexdump(buffer, rx);


    QcdmLogResponse* response = (QcdmLogResponse*)buffer;

    LOGI("Cmd: %d\n", response->command);
    LOGI("Entries: %d\n", response->entries);
    LOGI("Size: %lu\n", response->length);

    //hexdump(&response->logs, response->length);

    return kQcdmSuccess;
}
