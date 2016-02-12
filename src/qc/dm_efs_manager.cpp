
/**
* LICENSE PLACEHOLDER
*
* @file dm_efs_manager.cpp
* @class OpenPST::DmEfsManager
* @package OpenPST
* @brief EFS2 management over diagnostic monitor
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#include "dm_efs_manager.h"

using namespace OpenPST::QC;

/**
* @brief DmEfsManager - Constructor
* @param QcdmSerial port
*/
DmEfsManager::DmEfsManager(QcdmSerial& port) : 
    port(port),
    subsystemCommand(kDiagSubsysCmd),
    subsystemId(kDiagSubsyss)
{

}
/**
* @brief ~DmEfsManager - Deconstructor
*/
DmEfsManager::~DmEfsManager()
{

}
/**
* @brief setSubsystemCommand - Set the command code used to access the DM subsystem
*
* @param uint32_t command
* @return void
*/
void DmEfsManager::setSubsystemCommand(uint32_t command)
{
    subsystemCommand = command;
}

/**
* @brief setSubsystemCommand - Get the command code used to access the DM subsystem
*
* @return uint32_t
*/
uint32_t DmEfsManager::getSubsystemCommand()
{
    return subsystemCommand;
}

/**
* @brief setSubsystemId - Set the id used to access the EFS subsystem
*
* @param uint32_t command
* @return void
*/
void DmEfsManager::setSubsystemId(uint32_t id)
{
    subsystemId = id;
}

/**
* @brief getSubsystemId - Get the id used to access the EFS subsystem
*
* @return uint32_t
*/
uint32_t DmEfsManager::getSubsystemId()
{
    return subsystemId;
}

QcdmEfsHelloResponse DmEfsManager::hello()
{
	QcdmEfsHelloResponse response = {};

    QcdmEfsHelloRequest packet = {
        getHeader(kDiagEfsHello),
        DIAG_EFS_DEFAULT_WINDOW_SIZE,
        DIAG_EFS_DEFAULT_WINDOW_BYTE_SIZE,
        DIAG_EFS_DEFAULT_WINDOW_SIZE,
        DIAG_EFS_DEFAULT_WINDOW_BYTE_SIZE,
        DIAG_EFS_DEFAULT_WINDOW_SIZE,
        DIAG_EFS_DEFAULT_WINDOW_BYTE_SIZE,
        DIAG_EFS_VERSION,
        DIAG_EFS_MIN_VERSION,
        DIAG_EFS_MAX_VERSION,
        0x000000
    };
	
	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet), 
		sizeof(packet), 
		reinterpret_cast<uint8_t*>(&response), 
		sizeof(response)
	);

	return response;
}

QcdmEfsDeviceInfoResponse DmEfsManager::getDeviceInfo()
{
	QcdmEfsDeviceInfoResponse response = {};
    
    sendCommand(
		kDiagEfsDevInfo,
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);
	
	return response;
}

QcdmEfsQueryResponse DmEfsManager::query()
{
	QcdmEfsQueryResponse response = {};

	sendCommand(
		kDiagEfsQuery,
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);

	return response;
}


int32_t DmEfsManager::open(std::string path, int32_t flags, int32_t mode)
{
	size_t packetSize;
	QcdmEfsOpenFileRequest* packet;
	QcdmEfsOpenFileResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize = sizeof(QcdmEfsOpenFileRequest) + path.size();
    packet = (QcdmEfsOpenFileRequest*) new uint8_t[packetSize]();

    packet->header  = getHeader(kDiagEfsOpen);
    packet->flags	= flags;
    packet->mode	= mode;    
    std::memcpy(packet->path, path.c_str(), path.size());

	try {
		
		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet), 
			packetSize, reinterpret_cast<uint8_t*>(&response), 
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;

	if (response.error) {
		throw DmEfsManagerResponseError("Response returned an error", response.error);
	}

    return response.fp;
}

void DmEfsManager::close(int32_t fp)
{
	QcdmEfsCloseFileRequest packet   = {};
	QcdmEfsCloseFileReponse response = {};

	if (!fp) {
		throw DmEfsManagerInvalidArgument("Invalid file pointer");
	}

	packet.header	= getHeader(kDiagEfsClose);
	packet.fp		= fp;

	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet),
		sizeof(packet),
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

/**
* @brief DmEfsManager::read - Read from a file to a data buffer
*
* @param int32_t - fp of file
* @param size_t size - amount of data to read
* @param uint32_t offset - offset in file to start reading at
* @param std::vector<uint8_t>& - Data read
*
* @return int
*/
std::vector<uint8_t> DmEfsManager::read(int32_t fp, size_t size, uint32_t offset)
{
	QcdmEfsReadFileRequest packet = {};
	QcdmEfsReadFileResponse* response;
	std::vector<uint8_t> data;

	if (!fp) {
		throw DmEfsManagerInvalidArgument("Invalid file pointer");
	} else if (size <= 0) {
		throw DmEfsManagerInvalidArgument("Invalid size");
	} else if (size < 0) {
		throw DmEfsManagerInvalidArgument("Invalid offset");
	}
	
	response = (QcdmEfsReadFileResponse*) new uint8_t[DIAG_MAX_PACKET_SIZE]();

	packet.header	= getHeader(kDiagEfsRead);
	packet.fp		= fp;
	packet.offset	= offset;
	packet.size		= size;

	try {

		sendCommand(
			packet.header.subsysCommand,
			reinterpret_cast<uint8_t*>(&packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(response),
			DIAG_MAX_PACKET_SIZE
		);

	} catch (...) {
		delete response;
		throw;
	}

	if (isError(response->error)) {
		DmEfsManagerResponseError e = DmEfsManagerResponseError(getErrorString(response->error), response->error);
		delete response;
		throw e;
	}

	data.insert(data.end(), response->data, response->data + response->bytesRead);

    return data;
}


size_t DmEfsManager::read(std::string path, std::string outPath)
{
	QcdmEfsFstatResponse fileInfo;
	std::vector<uint8_t> data; 
	int32_t fp;

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	} else if (!outPath.length()) {
		throw DmEfsManagerInvalidArgument("Invalid out path");
	}

    fp = open(path, O_RDONLY, 0x00);

	if (!fp) {
		return 0;
	}

    fileInfo = fstat(fp);

	if (fileInfo.error) {
        return 0;
    }

    if (fileInfo.size <= 0) {
        return 0;
    }

	data = read(fp, fileInfo.size, 0);

    close(fp);

    std::ofstream out(outPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if (!out.is_open()) {
        return 0;
    }

    out.write(reinterpret_cast<char*>(&data[0]), data.size());

	return data.size();
}

size_t DmEfsManager::write(int32_t fp, uint8_t* data, size_t amount, uint32_t offset)
{
    return 0;
}

void DmEfsManager::symlink(std::string path, std::string linkPath)
{
	size_t packetSize;
	QcdmEfsCreateLinkRequest* packet;
	QcdmEfsCreateLinkResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	} else if (!linkPath.length()) {
		throw DmEfsManagerInvalidArgument("Invalid link path");
	}

	packetSize  = sizeof(QcdmEfsCreateLinkRequest) + path.size() + linkPath.size() + 1;
	packet		= (QcdmEfsCreateLinkRequest*) new uint8_t[packetSize]();

	packet->header = getHeader(kDiagEfsSymlink);
	std::memcpy(packet->path, path.c_str(), path.size());
	std::memcpy((packet->path + path.size() + 1), linkPath.c_str(), linkPath.size());;

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			packetSize, reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

std::string DmEfsManager::readSimlink(std::string path)
{
	size_t packetSize;
	QcdmEfsReadLinkRequest* packet;
	QcdmEfsReadLinkResponse* response;
	std::string ret;

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}
	
	packetSize	= sizeof(QcdmEfsReadLinkRequest) + path.size() + 1;
	packet		= (QcdmEfsReadLinkRequest*) new uint8_t[packetSize]();
	response	= (QcdmEfsReadLinkResponse*) new uint8_t[DIAG_MAX_PACKET_SIZE]();
	
	packet->header = getHeader(kDiagEfsReadLINK);
	std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			packetSize, 
			reinterpret_cast<uint8_t*>(response),
			DIAG_MAX_PACKET_SIZE
		);

	} catch (...) {
		delete packet, response;
		throw;
	}

	if (isError(response->error)) {
		DmEfsManagerResponseError e = DmEfsManagerResponseError(getErrorString(response->error), response->error);
		delete packet, response;
		throw e;
	}
	
	ret.append(response->path, strlen(response->path));

	return ret;
}

void DmEfsManager::unlink(std::string path)
{
	size_t packetSize;
	QcdmEfsUnlinkRequest* packet;
	QcdmEfsUnlinkResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsUnlinkRequest) + path.size() + 1;
    packet		= (QcdmEfsUnlinkRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsUnlink);
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			packetSize, 
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

/**
* @brief DmEfsManager::mkdir - Create a directory
*
* @param std::string path - Full path to directory to create
* @param int16_t mode - Mode to create directory (chmod)
*
* @return int
*/
void DmEfsManager::mkdir(std::string path, int16_t mode)
{
	size_t packetSize;
	QcdmEfsMkdirRequest* packet;
	QcdmEfsMkdirResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

	packetSize	= sizeof(QcdmEfsMkdirRequest) + path.size();
	packet		= (QcdmEfsMkdirRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsMkdir);
    packet->mode = mode;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			packetSize,
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}


void DmEfsManager::rmdir(std::string path)
{
	size_t packetSize;
	QcdmEfsRmdirRequest* packet;
	QcdmEfsRmdirResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsRmdirRequest) + path.size();
    packet		= (QcdmEfsRmdirRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsRmdir);
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			packetSize,
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

uint32_t DmEfsManager::openDir(std::string path)
{
	size_t packetSize;
	QcdmEfsOpenDirRequest* packet;
	QcdmEfsOpenDirResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsOpenDirRequest) + path.size();
    packet		= (QcdmEfsOpenDirRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsOpenDIR);
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			packetSize,
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}

	return response.dp;
}


std::vector<DmEfsNode> DmEfsManager::readDir(std::string path, bool recursive)
{
    uint32_t dp;
	std::string checkPath;
	std::vector<DmEfsNode> contents;
	QcdmEfsReadDirResponse* response;
	QcdmEfsReadDirRequest packet = {};
	std::ostringstream error;

	if (!path.length()) {
		error << "Invalid Path " << path;
		throw DmEfsManagerInvalidArgument(error.str());
	}

	if (!(dp = openDir(path))) {
		error << "Could not open directory " << path;
		throw DmEfsManagerResponseError(error.str(), 0);
	}

	response = (QcdmEfsReadDirResponse*) new uint8_t[DIAG_MAX_PACKET_SIZE]();

	packet.header = getHeader(kDiagEfsReadDIR);
    packet.dp = dp;
    packet.sequenceNumber = 1;

    do {

		try {

			sendCommand(
				packet.header.subsysCommand,
				reinterpret_cast<uint8_t*>(&packet),
				sizeof(packet),
				reinterpret_cast<uint8_t*>(response),
				DIAG_MAX_PACKET_SIZE
			);

		} catch (...) {
			try {
				closeDir(dp);
			} catch (...) {}
			delete response;
			throw;
		}

        if (response->sequenceNumber != packet.sequenceNumber) {
			LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
		}

        if (strlen(response->name) == 0) {
			break; // end
        }
    
		DmEfsNode node(path, response);

        contents.insert(contents.end(), node);
        packet.sequenceNumber++;
		

    } while (true);

	closeDir(dp);

	delete response;

    if (recursive) {
        for (auto &node : contents) {
            if (node.isDir()) {

                checkPath = path;

                checkPath.append(node.name).append("/");
				
				node.children = readDir(checkPath, recursive);
            }
        }
    }

	return contents;
}


std::vector<DmEfsNode> DmEfsManager::readDir(uint32_t dp)
{
	std::vector<DmEfsNode>	contents;
	QcdmEfsReadDirResponse* response;
	QcdmEfsReadDirRequest	packet = {};

	if (!dp) {
		throw DmEfsManagerInvalidArgument("Invalid directory pointer");
	}
	
	response = (QcdmEfsReadDirResponse*) new uint8_t[DIAG_MAX_PACKET_SIZE]();

    packet.header = getHeader(kDiagEfsReadDIR);
    packet.dp = dp;
    packet.sequenceNumber = 1;
            
    do {

		try {

			sendCommand(
				packet.header.subsysCommand,
				reinterpret_cast<uint8_t*>(&packet),
				sizeof(packet),
				reinterpret_cast<uint8_t*>(response),
				DIAG_MAX_PACKET_SIZE
			);

		} catch (...) {
			try {
				closeDir(dp);
			} catch (...) {}
			delete response;
			throw;
		}

        if (response->sequenceNumber != packet.sequenceNumber) {
			LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
		}

        if (strlen(response->name) == 0) {
            break; // end
        }

        DmEfsNode node("", response);

        contents.insert(contents.end(), node);

        packet.sequenceNumber++;

    } while (true);

	delete response;

	return contents;
}

void DmEfsManager::closeDir(uint32_t dp)
{
	QcdmEfsCloseDirRequest packet = {};
	QcdmEfsCloseDirResponse response = {};

	if (!dp) {
		throw DmEfsManagerInvalidArgument("Invalid directory pointer");
	}

	packet.header	= getHeader(kDiagEfsCloseDIR);
	packet.dp		= dp;
	
	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet),
		sizeof(packet),
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);


	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}


void DmEfsManager::rename(std::string path, std::string newPath)
{
	size_t packetSize;
	QcdmEfsRenameRequest* packet;
	QcdmEfsRenameResponse response;

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	} else if (!newPath.length()) {
		throw DmEfsManagerInvalidArgument("Invalid new path");
	}

    packetSize  = sizeof(QcdmEfsRenameRequest) + path.size() + newPath.size() + 1;
    packet		= (QcdmEfsRenameRequest*) new uint8_t[packetSize]();
    
	packet->header = getHeader(kDiagEfsRename);    
	std::memcpy(packet->path, path.c_str(), path.size());
    std::memcpy((packet->path + path.size() + 1), newPath.c_str(), newPath.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;


	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

QcdmEfsStatResponse DmEfsManager::stat(std::string path)
{
	size_t packetSize;
	QcdmEfsStatRequest* packet;
	QcdmEfsStatResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

	packetSize = sizeof(QcdmEfsStatRequest) + path.size() + 1;
    packet = (QcdmEfsStatRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsStat);
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;

	return response;
}


QcdmEfsLstatResponse DmEfsManager::lstat(std::string path)
{
	size_t packetSize;
	QcdmEfsLstatRequest* packet;
	QcdmEfsLstatResponse response = {}; 
	
	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsLstatRequest) + path.size() + 1;
    packet		= (QcdmEfsLstatRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsLstat);
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	return response;
}

QcdmEfsFstatResponse DmEfsManager::fstat(int32_t fp)
{
	QcdmEfsFstatRequest packet		= {};
	QcdmEfsFstatResponse response	= {};

	if (!fp) {
		throw DmEfsManagerInvalidArgument("Invalid file pointer");
	}

    packet.header	= getHeader(kDiagEfsFstat);
    packet.fp		= fp;

	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet),
		sizeof(packet),
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);

	return response;
}


void DmEfsManager::chmod(std::string path, int16_t mode)
{
	size_t packetSize;
	QcdmEfsChmodRequest* packet;
	QcdmEfsChmodResponse response = {};
	
	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsChmodRequest) + path.size() + 1;
    packet		= (QcdmEfsChmodRequest*) new uint8_t[packetSize]();

    packet->header	= getHeader(kDiagEfsChmod);
    packet->mode	= mode;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	delete packet;
	

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

QcdmEfsStatfsResponse DmEfsManager::statfs(std::string path)
{
	size_t packetSize;
	QcdmEfsStatfsRequest* packet;
	QcdmEfsStatfsResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsStatfsRequest) + path.size() + 1;
    packet		= (QcdmEfsStatfsRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsStatFS);
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

	return response;
}

void DmEfsManager::chown(std::string path, int32_t uid, int32_t gid)
{
	size_t packetSize;
	QcdmEfsChownRequest* packet;
	QcdmEfsChownResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsChownRequest) + path.size() + 1;
    packet		= (QcdmEfsChownRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsChown);
    packet->uid = uid;
    packet->gid = gid;
    std::memcpy(packet->path, path.c_str(), path.size());
    
	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}


	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

void DmEfsManager::setQuota(std::string path, int32_t gid, size_t size)
{
	size_t packetSize;
	QcdmEfsSetQuotaRequest* packet;
	QcdmEfsSetQuotaResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	} else if (size < 0) {
		throw DmEfsManagerInvalidArgument("Invalid size");
	}

    packetSize	= sizeof(QcdmEfsSetQuotaRequest) + path.size() + 1;
    packet		= (QcdmEfsSetQuotaRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsSetQuota);
    packet->gid = gid;
    packet->amount = size;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;


	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}


void DmEfsManager::deltree(std::string path, int sequence)
{
	size_t packetSize;
	QcdmEfsDeltreeRequest* packet;
	QcdmEfsDeltreeResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsDeltreeRequest) + path.size() + 1;
    packet		= (QcdmEfsDeltreeRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsDeltree);
    packet->sequence = sequence;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

    if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}


	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

void DmEfsManager::truncate(std::string path, size_t amount, int32_t sequence)
{
	size_t packetSize;
	QcdmEfsTruncateRequest* packet;
	QcdmEfsTruncateResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	} else if (amount <= 0) {
		throw DmEfsManagerInvalidArgument("Invalid amount");
	}

    packetSize = sizeof(QcdmEfsTruncateRequest) + path.size();
    packet = (QcdmEfsTruncateRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsTruncate);
    packet->sequence = sequence;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

    if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}


void DmEfsManager::ftruncate(int32_t fp, size_t amount, int32_t sequence)
{
	QcdmEfsFtncateRequest packet = {};
	QcdmEfsFtncateResponse response = {};

	if (!fp) {
		throw DmEfsManagerInvalidArgument("Invalid file pointer");
	} else if (amount <= 0) {
		throw DmEfsManagerInvalidArgument("Invalid amount");
	}

    packet.header	= getHeader(kDiagEfsFtruncate);
    packet.sequence = sequence;
    packet.fp		= fp;

	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet),
		sizeof(packet),
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);

	if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}


QcdmEfsStatfsV2Response DmEfsManager::statfsV2(std::string path, int sequence)
{
	size_t packetSize;
	QcdmEfsStatfsV2Request* packet;
	QcdmEfsStatfsV2Response response = {};


	packetSize = sizeof(QcdmEfsStatfsV2Request) + path.size() + 1;
	packet = (QcdmEfsStatfsV2Request*) new uint8_t[packetSize]();

	packet->header = getHeader(kDiagEfsStatVFS_V2);
	packet->sequence = sequence;
	std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}	

	return response;
}

QcdmEfsMd5SumResponse DmEfsManager::md5sum(std::string path, int32_t sequence)
{
	size_t packetSize;
	QcdmEfsMd5SumRequest* packet;
	QcdmEfsMd5SumResponse response = {};
	
	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

	packetSize  = sizeof(QcdmEfsMd5SumRequest) + path.size();
	packet		= (QcdmEfsMd5SumRequest*) new uint8_t[packetSize]();

	packet->header	 = getHeader(kDiagEfsMd5Sum);
	packet->sequence = sequence;

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

    if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}
	
	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}

	return response;
}


void DmEfsManager::formatHotplugDevice(std::string path, int32_t sequence)
{
	size_t packetSize;
	QcdmEfsHotplugFormatRequest* packet;
	QcdmEfsHotplugFormatResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize  = sizeof(QcdmEfsHotplugFormatRequest) + path.size();
    packet		= (QcdmEfsHotplugFormatRequest*) new uint8_t[packetSize]();

    packet->header	 = getHeader(kDiagEfsHotplugFormat);
    packet->sequence = sequence;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

    if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}

void DmEfsManager::shred(std::string path, int32_t sequence)
{
	size_t packetSize;
	QcdmEfsShredRequest* packet;
	QcdmEfsShredResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsShredRequest) + path.size() + 1;
    packet		= (QcdmEfsShredRequest*) new uint8_t[packetSize]();

    packet->header		= getHeader(kDiagEfsShred);
    packet->sequence	= sequence;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

    if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}
}


QcdmEfsSyncResponse DmEfsManager::syncNoWait(std::string path, uint16_t sequence)
{
	size_t packetSize;
	QcdmEfsSyncRequest* packet;
	QcdmEfsSyncResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}
	
	packetSize	= sizeof(QcdmEfsSyncRequest) + path.size() + 1;
	packet		= (QcdmEfsSyncRequest*) new uint8_t[packetSize]();

	packet->header	 = getHeader(kDiagEfsSyncNoWait);
	packet->sequence = sequence;
	std::memcpy(packet->path, path.c_str(), path.size() + 1);

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

    if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}

	return response;
}

QcdmEfsGetSyncStatusResponse DmEfsManager::getSyncStatus(std::string path, uint32_t token, uint16_t sequence)
{
	size_t packetSize;
	QcdmEfsGetSyncStatusRequest* packet;
	QcdmEfsGetSyncStatusResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	} else if (!token) {
		throw DmEfsManagerInvalidArgument("Invalid token");
	}
		
    packetSize	= sizeof(QcdmEfsGetSyncStatusRequest) + path.size() + 1;
    packet		= (QcdmEfsGetSyncStatusRequest*) new uint8_t[packetSize]();

    packet->header		= getHeader(kDiagEfsSyncGetStatus);
    packet->sequence	= sequence;
    packet->token		= token;
    std::memcpy(packet->path, path.c_str(), path.size() + 1);
	
	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

	if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}

	return response;
}


int DmEfsManager::makeGoldenCopy(std::string path, int32_t sequence)
{
	size_t packetSize;
	QcdmEfsMakeGoldenCopyRequest* packet;
	QcdmEfsMakeGoldenCopyResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize	= sizeof(QcdmEfsMakeGoldenCopyRequest) + path.size() + 1;
    packet		= (QcdmEfsMakeGoldenCopyRequest*) new uint8_t[packetSize]();

    packet->header	 = getHeader(kDiagEfsMakeGoldenCopy);
    packet->sequence = sequence;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

	if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (isError(response.error)) {
		throw DmEfsManagerResponseError(getErrorString(response.error), response.error);
	}

	return response.error ? false : true;
}


int32_t DmEfsManager::openFilesystemImage(std::string path, uint8_t imageType, int32_t sequence)
{
	size_t packetSize;
	QcdmEfsFsImageOpenRequest* packet;
	QcdmEfsFsImageOpenResponse response = {};

	if (!path.length()) {
		throw DmEfsManagerInvalidArgument("Invalid path");
	}

    packetSize  = sizeof(QcdmEfsFsImageOpenRequest) + path.size();
    packet		= (QcdmEfsFsImageOpenRequest*) new uint8_t[packetSize]();

    packet->header = getHeader(kDiagEfsFsImageOpen);
    packet->sequence = sequence;
    packet->imageType = imageType;
    std::memcpy(packet->path, path.c_str(), path.size());

	try {

		sendCommand(
			packet->header.subsysCommand,
			reinterpret_cast<uint8_t*>(packet),
			sizeof(packet),
			reinterpret_cast<uint8_t*>(&response),
			sizeof(response)
		);

	} catch (...) {
		delete packet;
		throw;
	}

    delete packet;

	if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	return response.error ? 0 : response.handle;
}

std::vector<uint8_t> DmEfsManager::readFilesystemImage(int32_t handle, int32_t sequence)
{
	QcdmEfsFsImageReadRequest packet = {};
	QcdmEfsFsImageReadResponse* response;
	std::vector<uint8_t> data;

	if (!handle) {
		throw DmEfsManagerInvalidArgument("Invalid handle");
	}

	packet.header = getHeader(kDiagEfsFsImageClose);
	packet.sequence = sequence;
	packet.handle	= handle;

	response = (QcdmEfsFsImageReadResponse*) new uint8_t[DIAG_MAX_PACKET_SIZE]();

	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet),
		sizeof(packet),
		reinterpret_cast<uint8_t*>(response),
		DIAG_MAX_PACKET_SIZE
	);

	if (response->error) {
		DmEfsManagerResponseError e = DmEfsManagerResponseError("Response returned an error", response->error);
		delete response;
		throw;
	}

	if (response->sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	data.insert(data.end(), response->data, response->data + response->length);

	delete response;

	return data;
}

void DmEfsManager::closeFilesystemImage(int32_t handle, int32_t sequence)
{
	QcdmEfsFsImageCloseRequest packet = {};
	QcdmEfsFsImageCloseResponse response = {};

	if (!handle) {
		throw DmEfsManagerInvalidArgument("Invalid handle");
	}

	packet.header	= getHeader(kDiagEfsFsImageClose);
	packet.sequence = sequence;
	packet.handle	= handle;

	sendCommand(
		packet.header.subsysCommand,
		reinterpret_cast<uint8_t*>(&packet),
		sizeof(packet),
		reinterpret_cast<uint8_t*>(&response),
		sizeof(response)
	);

	if (response.sequence != sequence) {
		LOGI("Invalid sequence received in %s response\n", __FUNCTION__);
	}

	if (response.error) {
		throw DmEfsManagerResponseError("Response returned an error", response.error);
	}
}

/**
* @brief access - Check access permissions for a path
*
* @param std::string path
* @param char checkPermissionBits
*
* @return int
*/
int DmEfsManager::access(std::string path, char checkPermissionBits)
{
	if (!port.isOpen()) {
		return kDmEfsIOError;
	}

	size_t packetSize = sizeof(QcdmEfsAccessRequest) + path.size();
	QcdmEfsAccessRequest* packet = (QcdmEfsAccessRequest*) new uint8_t[packetSize]();

	packet->header = getHeader(kDiagEfsAccess);
	packet->permissionBits = checkPermissionBits;
	std::memcpy(packet->path, path.c_str(), path.size());

	int commandResult = sendCommand(kDiagEfsAccess, reinterpret_cast<uint8_t*>(packet), packetSize);

	delete packet;

	if (commandResult != kDmEfsSuccess) {
		return commandResult;
	}

	QcdmEfsAccessResponse* response = (QcdmEfsAccessResponse*)buffer;

	if (response->error) {
		return kDmEfsError;
	}

	return kDmEfsSuccess;
}

/**
* @brief getFactoryImage - Get EFS factory image
*
* @param std::ofstream& out
*
* @return int
*/
int DmEfsManager::getFactoryImage(std::ofstream& out)
{
	return  kDmEfsError;
}

/**
* @brief factoryImageStart - Start transfer of factory image
*
* @return int
*/
int DmEfsManager::factoryImageStart()
{
	/*if (!port.isOpen()) {
		return kDmEfsIOError;
	}

	int commandResult = sendCommand(kDiagEfsFactoryImageStart);

	if (commandResult != kDmEfsSuccess) {
		return commandResult;
	}

	QcdmEfsFactoryImageStartResponse* response = (QcdmEfsFactoryImageStartResponse*)buffer;

	if (response->error) {
		return kDmEfsError;
	}*/
	return kDmEfsSuccess;
}

/**
* @brief factoryImageStart - Start transfer of factory image
*
* @return int
*/
int DmEfsManager::factoryImageRead(std::vector<uint8_t>& data)
{
	if (!port.isOpen()) {
		return kDmEfsIOError;
	}

	QcdmEfsFactoryImageReadRequest packet = {};
	packet.header				= getHeader(kDiagEfsFactoryImageStart);
	packet.streamState			= 0x00;
	packet.infoClusterSent		= 0x00;
	packet.clusterMapSequence	= 0x00;
	packet.clusterDataSequence	= 0x00;

	int commandResult = sendCommand(kDiagEfsFactoryImageStart, reinterpret_cast<uint8_t*>(&packet), sizeof(packet));

	if (commandResult != kDmEfsSuccess) {
		return commandResult;
	}

	QcdmEfsFactoryImageReadResponse* response = (QcdmEfsFactoryImageReadResponse*)buffer;

	if (response->error) {
		return kDmEfsError;
	}

	return kDmEfsSuccess;
}

/**
* @brief factoryImageEnd - End transfer of factory image
*
* @return int
*/
int DmEfsManager::factoryImageEnd()
{
	/*if (!port.isOpen()) {
		return kDmEfsIOError;
	}

	int commandResult = sendCommand(kDiagEfsFactoryImageStart);

	if (commandResult != kDmEfsSuccess) {
		return commandResult;
	}

	QcdmEfsFactoryImageEndResponse* response = (QcdmEfsFactoryImageEndResponse*)buffer;

	if (response->error) {
		return kDmEfsError;
	}*/
	return kDmEfsSuccess;
}

/**
* @brief factoryImagePrepare - Prepare for transfer of factory image
*
* @return int
*/
int DmEfsManager::factoryImagePrepare()
{
	/*if (!port.isOpen()) {
		return kDmEfsIOError;
	}

	int commandResult = sendCommand(kDiagEfsPrepareFactoryImage);

	if (commandResult != kDmEfsSuccess) {
		return commandResult;
	}

	QcdmEfsFactoryImagePrepareResponse* response = (QcdmEfsFactoryImagePrepareResponse*)buffer;

	if (response->error) {
		return kDmEfsError;
	}
	*/
	return kDmEfsSuccess;
}



/**
* @brief getGroupInfo - Get information about a group for a path
*
* @param std::string path
* @param int32_t gid
*
* @return int
*/
int DmEfsManager::getGroupInfo(std::string path, int32_t gid)
{
	if (!port.isOpen()) {
		return kDmEfsIOError;
	}

	size_t packetSize = sizeof(QcdmEfsGetGroupInfoRequest) + path.size();
	QcdmEfsGetGroupInfoRequest* packet = (QcdmEfsGetGroupInfoRequest*) new uint8_t[packetSize]();

	packet->header = getHeader(kDiagEfsGetGroupInfo);
	packet->gid = gid;
	std::memcpy(packet->path, path.c_str(), path.size());

	int commandResult = sendCommand(kDiagEfsGetGroupInfo, reinterpret_cast<uint8_t*>(packet), packetSize);

	delete packet;

	if (commandResult != kDmEfsSuccess) {
		return commandResult;
	}

	QcdmEfsGetGroupInfoResponse* response = (QcdmEfsGetGroupInfoResponse*)buffer;

	if (response->error) {
		return kDmEfsError;
	}

	return kDmEfsSuccess;
}



QcdmSubsysHeader DmEfsManager::getHeader(uint16_t command)
{
    QcdmSubsysHeader header = {
        getSubsystemCommand(),
        getSubsystemId(),
        command
    };

    return header;
}


bool DmEfsManager::sendCommand(uint16_t command, uint8_t* response, size_t responseSize, bool flush)
{
	QcdmSubsysHeader packet = getHeader(command);

	return sendCommand(command, reinterpret_cast<uint8_t*>(&packet), sizeof(packet), response, responseSize, flush);
}

bool DmEfsManager::sendCommand(uint16_t command, uint8_t* request, size_t requestSize, uint8_t* responseOut, size_t responseOutSize, bool flush)
{
	size_t rxSize;
	QcdmSubsysHeader* header = (QcdmSubsysHeader*)&buffer;

	port.write(request, requestSize);

	if (!(rxSize = port.read(buffer, DIAG_MAX_PACKET_SIZE))) {
		throw DmEfsManagerResponseError("Device did not respond", 0);
	} else if (port.isError(header->command)) {
		throw QcdmSerialResponseError(port.getErrorString(header->command), header->command);
	} else if (header->subsysCommand != command) {
		throw DmEfsManagerResponseError("Unexcpected command response", header->subsysCommand);
	} else if (responseOutSize < rxSize) {
		throw DmEfsManagerInvalidArgument("Respose buffer is not large enough to fit the response");
	}
	
	std::memcpy(responseOut, &buffer, rxSize);

	if (flush) {
		port.flush();
	}
	
	return true;
}

bool DmEfsManager::isValidResponse(uint16_t command, uint8_t* data, size_t size)
{
    if (size == 0) {
        return false;
    }

	QcdmSubsysHeader* header = (QcdmSubsysHeader*)data;

	if (port.isError(header->command) ||
        header->subsysCommand != command
    ) {     
		return false;
    }

    return true;
}


bool DmEfsManager::isError(uint32_t error)
{
	return error != 0;
}

std::string DmEfsManager::getErrorString(uint32_t error)
{
	switch (error) {
		case kDiagEfsErrorInconsistentState:		return "Inconsistent State";
		case kDiagEfsErrorInvalidSequence:			return "Invalid Sequence Number";
		case kDiagEfsErrorDirNotOpen:				return "Directory Not Open";
		case kDiagEfsErrorDirEntNotFound:			return "Directory Entry Not Found";
		case kDiagEfsErrorInvalidPath:				return "Invalid Path";
		case kDiagEfsErrorPathTooLong:			return "Path Too Long";
		case kDiagEfsErrorTooManyOpenDirs:		return "Too Many Open Directories";
		case kDiagEfsErrorInvalidDirEntry:		return "Invalid Directory Entry";
		case kDiagEfsErrorTooManyOpenFiles:		return "Too Many Open Files";
		case kDiagEfsErrorUnknmownFileType:			return "Unknown File Type";
		case kDiagEfsErrorNotNandFlash:			return "Not NAND Flash";
		case kDiagEfsErrorUnavailableInfo:			return "Information Unavailable";
		default:								return "";
	}
}