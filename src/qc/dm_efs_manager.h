/**
* LICENSE PLACEHOLDER
*
* @file dm_efs_manager.h
* @class OpenPST::DmEfsManager
* @package OpenPST
* @brief EFS2 management over diagnostic monitor
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _QC_DM_EFS_MANAGER_H_
#define _QC_DM_EFS_MANAGER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fcntl.h>
#include "qc/dm.h"
#include "qc/dm_efs.h"
#include "qc/dm_efs_node.h"
#include "qc/qcdm_serial.h"

using OpenPST::QC::QcdmSerial;
using OpenPST::QC::QcdmSerialError;
using OpenPST::QC::QcdmSerialInvalidArgument;
using OpenPST::QC::QcdmSerialResponseError;

namespace OpenPST {
	namespace QC {
		/**
		* @brief OpenPST::DmEfsManager
		*/
		class DmEfsManager {
    
			protected:
				QcdmSerial& port;
				uint8_t buffer[DIAG_MAX_PACKET_SIZE];
			private:
				uint32_t subsystemCommand;
				uint32_t subsystemId;
			public:
            
				enum DmEfsOperationResult{
					kDmEfsIOError = -1,
					kDmEfsError = 0,
					kDmEfsSuccess = 1,
				}; 

				/**
				* @brief DmEfsManager - Constructor
				* @param QcdmSerial port
				*/
				DmEfsManager(QcdmSerial& port);

				/**
				* @brief ~DmEfsManager - Deconstructor
				*/
				~DmEfsManager();

				/**
				* @brief setSubsystemCommand - Set the command code used to access the DM subsystem
				*
				* @param uint32_t command
				* @return void
				*/
				void setSubsystemCommand(uint32_t command);

				/**
				* @brief setSubsystemCommand - Get the command code used to access the DM subsystem
				*
				* @return uint32_t
				*/
				uint32_t getSubsystemCommand();

				/**
				* @brief setSubsystemId - Set the id used to access the EFS subsystem
				*
				* @param uint32_t command
				* @return void
				*/
				void setSubsystemId(uint32_t code);

				/**
				* @brief getSubsystemId - Get the id used to access the EFS subsystem
				*
				* @return uint32_t
				*/
				uint32_t getSubsystemId();

				/**
				* @brief hello - Send the hello and recieve configuration parameters
				*
				* @param QcdmEfsHelloResponse &response - Populated with the results from the device on success
				* @return QcdmEfsHelloResponse
				*/
				QcdmEfsHelloResponse hello();

				/**
				* @brief getDeviceInfo - Get the device information
				*
				* @param QcdmEfsDeviceInfoResponse &response - Populated with the results from the device on success
				* @return QcdmEfsDeviceInfoResponse
				*/
				QcdmEfsDeviceInfoResponse getDeviceInfo();

				/**
				* @brief query - Query for file system settings
				*
				* @param QcdmEfsQueryResponse &response - Populated with the results from the device on success
				* @return QcdmEfsQueryResponse
				*/
				QcdmEfsQueryResponse query();

				/**
				* @brief open - Open a file for r/rw
				*
				* @param std::string path - full path to the file
				* @param int32_t mode - Mode to open file, standard open flags apply
				* @param int32_t& - fp of opened file on success
				*
				* @return int32_t - file pointer to opened file on device or 0 on error
				*/
				int32_t open(std::string path, int32_t flags, int32_t mode);

				/**
				* @brief close - Close a file
				*
				* @param int32_t - fp of file
				*
				* @return void
				*/
				void close(int32_t fp);

				/**
				* @brief read - Read from a file to a data buffer
				*
				* @param int32_t - fp of file
				* @param size_t size - amount of data to read
				* @param uint32_t offset - offset in file to start reading at
				*
				* @return std::vector<uint8_t> - Data read
				*/
				std::vector<uint8_t> read(int32_t fp, size_t size, uint32_t offset);
            
				/**
				* @brief read - Read from a file to a file
				*
				* @param std::string - Remote path to read
				* @param std::string - Local path to write
				*
				* @return size_t
				*/
				size_t read(std::string path, std::string outPath);

				/**
				* @brief write - Write to a file
				*
				* @param int32_t - fp of file
				* @param uint8_t* - data to write
				* @param size_t amount - amount of data to write
				* @paramuint32_t offset - offset in file to start writing at
				*
				* @return size_t - Amount written
				*/
				size_t write(int32_t fp, uint8_t* data, size_t amount, uint32_t offset);
            
				/**
				* @brief symlink - Create a symlink
				*
				* @param std::string path - Full path to file
				* @param std::string linkPath - Full path to the link
				*
				* @return void
				*/
				void symlink(std::string path, std::string linkPath);

				/**
				* @brief readSimlink - Read a symlink path
				*
				* @param std::string path - Full path to file
				*
				* @return std::string out - String with the links resolved location
				*/
				std::string readSimlink(std::string path);

				/**
				* @brief unlink - Delete a file
				*
				* @param std::string path - Full path to file
				*
				* @return void
				*/
				void unlink(std::string path);

				/**
				* @brief mkdir - Create a directory
				*
				* @param std::string path - Full path to directory to create
				* @param int16_t mode - Mode to create directory (chmod)
				*
				* @return void
				*/
				void mkdir(std::string path, int16_t mode);
            
				/**
				* @brief rmdir - Delete a directory
				*
				* @param std::string path - Full path to directory
				*
				* @return void
				*/
				void rmdir(std::string path);

				/**
				* @brief openDir - Open a directory for reading
				*
				* @param std::string path - Full path to file
				*
				* @return uint32_t - Directory pointer to opened directory or 0 on error
				*/
				uint32_t openDir(std::string path);
                
				/**
				* @brief readDir - Read a directory contents, optionally recursively
				*
				* @param std:;string path - Full path to file
				* @param bool - read recursively
				*
				* @return std::vector<DmEfsNode> - Read directory contents
				*/
				std::vector<DmEfsNode> readDir(std::string path, bool recursive = false);

				/**
				* @brief readDir - Read a directory contents, not recursive
				*
				* @param uint32_t dp - dp from openDir operation
				*
				* @return std::vector<DmEfsNode>
				*/
				std::vector<DmEfsNode> readDir(uint32_t dp);

				/**
				* @brief closeDir - Close a open directory.
				*
				* @param uint32_t dp - Directory pointer to opened directory
				* 
				* @return bool
				*/
				void closeDir(uint32_t dp);

				/**
				* @brief rename - Rename a file or directory
				*
				* @param std::string path
				* @param std::string newPath
				*
				* @return bool
				*/
				void rename(std::string path, std::string newPath);
            
				/**
				* @brief stat - Stat a file or directory by path
				*
				* @param std::string path
				*
				* @return QcdmEfsStatResponse
				*/
				QcdmEfsStatResponse stat(std::string path);
            
				/**
				* @brief lstat - Stat a link by path
				*
				* @param std::string path
				*
				* @return QcdmEfsLstatResponse
				*/
				QcdmEfsLstatResponse lstat(std::string path);
            
				/**
				* @brief fstat - Stat a file by fp
				*
				* @param std::string path
				*
				* @return QcdmEfsFstatResponse
				*/
				QcdmEfsFstatResponse fstat(int32_t fp);
            
				/**
				* @brief chmod - Change file permissions
				*
				* @param std::string path
				* @param int16_t mode
				*
				* @return bool
				*/
				void chmod(std::string path, int16_t mode);
            
				/**
				* @brief statfs - Stat the filesystem
				*
				* @param std::string path
				* @param QcdmEfsStatfsResponse& response
				*
				* @return int
				*/
				QcdmEfsStatfsResponse statfs(std::string path);

				/**
				* @brief chown - Change file or directory ownership
				*
				* @param std::string path
				* @param int32_t uid
				* @param int32_t gid
				*
				* @return bool
				*/
				void chown(std::string path, int32_t uid, int32_t gid);

				/**
				* @brief setQuota - Limit the size of a directory for a group
				*
				* @param std::string path
				* @param int32_t gid
				* @param size_t size
				*
				* @return bool
				*/
				void setQuota(std::string path, int32_t gid, size_t size);
            
				/**
				* @brief deltree - Delete a directory tree
				*
				* @param std::string path
				* @param int32_t sequence
				*
				* @return bool
				*/
				void deltree(std::string path, int32_t sequence = 1);
            
				/*
				int startBenchmarkTest();
				int getBenchmarkResults();
				int initBenchmark();
				int resetBenchmark();
				int setReservation();
				int put();
				int get();
				*/
            
				/**
				* @brief truncate - Truncate a file by path
				*
				* @param std::string path
				* @param size_t amount
				* @param int32_t sequence
				*
				* @return void
				*/
				void truncate(std::string path, size_t amount, int32_t sequence = 1);
            
				/**
				* @brief truncate - Truncate a file by fp
				*
				* @param int32_t fp
				* @param size_t amount
				* @param int32_t sequence
				*
				* @return void
				*/
				void ftruncate(int32_t fp, size_t amount, int32_t sequence = 1);
            
				/**
				* @brief statfsV2 - STATFS v2
				*
				* @param int32_t sequence
				*
				* @return QcdmEfsStatfsV2Response
				*/
				QcdmEfsStatfsV2Response statfsV2(std::string path, int sequence = 1);
            
				/**
				* @brief md5sum - Get MD5 Sum for a file.
				*
				* @param std::string path
				* @param int32_t sequence
				*
				* @return QcdmEfsMd5SumResponse
				*/
				QcdmEfsMd5SumResponse md5sum(std::string path, int32_t sequence = 1);
            
				/**
				* @brief formatHotplugDevice - Format a hot plug device
				*
				* @param std::string path
				* @param int32_t sequence
				*
				* @return void
				*/
				void formatHotplugDevice(std::string path, int32_t sequence = 1);
            
				/**
				* @brief shred - Erase obsolete data
				*
				* @param std::string path
				* @param int32_t sequence
				*
				* @return void
				*/
				void shred(std::string path, int32_t sequence = 1);
            

				/*
				int setIdleDevEventTime();
				int getHotplugDeviceInfo(std::string path, int32_t sequence = 1);
				*/


				QcdmEfsSyncResponse syncNoWait(std::string path, uint16_t sequence = 1);

				QcdmEfsGetSyncStatusResponse getSyncStatus(std::string path, uint32_t token, uint16_t sequence = 1);
            
				/*int truncate64();
				int ftruncate64(int32_t fp);
				int lseek64();
				*/
				int makeGoldenCopy(std::string path, int32_t sequence = 1);
				int32_t openFilesystemImage(std::string path, uint8_t imageType, int32_t sequence = 1);
				std::vector<uint8_t> readFilesystemImage(int32_t handle, int32_t sequence = 1);
				void closeFilesystemImage(int32_t handle, int32_t sequence = 1);

				/**
				* @brief getGroupInfo - Get information about a group for a path
				*
				* @param std::string path
				* @param int32_t gid
				*
				* @return int
				*/
				int getGroupInfo(std::string path, int32_t gid);
				/**
				* @brief access - Check access permissions for a path
				*
				* @param std::string path
				* @param char checkPermissionBits
				*
				* @return int
				*/
				int access(std::string path, char checkPermissionBits);

				/**
				* @brief getFactoryImage - Get EFS factory image
				*
				* @param std::ofstream& out
				*
				* @return int
				*/
				int getFactoryImage(std::ofstream& out);

				/**
				* @brief factoryImageStart - Start transfer of factory image
				*
				* @return int
				*/
				int factoryImageStart();

				/**
				* @brief factoryImageStart - Start transfer of factory image
				*
				* @return int
				*/
				int factoryImageRead(std::vector<uint8_t>& data);

				/**
				* @brief factoryImageEnd - End transfer of factory image
				*
				* @return int
				*/
				int factoryImageEnd();

				/**
				* @brief factoryImagePrepare - Prepare for transfer of factory image
				*
				* @return int
				*/
				int factoryImagePrepare();


			private:
            
				/**
				* @brief getHeader - Used internally to quickly assemble a packet header
				* 
				* @param uint16_t - The command to create the header data for
				*
				* @return QcdmSubsysHeader
				*/
				QcdmSubsysHeader getHeader(uint16_t command);
            
      	
				bool sendCommand(uint16_t command, uint8_t* responseOut, size_t responseOutSize, bool flush = true);
				bool sendCommand(uint16_t command, uint8_t* request, size_t requestSize, uint8_t* responseOut, size_t responseOutSize, bool flush = true);


				/**
				* @brief isValidResponse - used internally to validate responses
				*
				*
				* @param uint16_t - The expected response command
				* @param uint8_t* - Opaque data pointer to validate against
				* @param size_t - Size of the data
				*
				* @return bool
				*/
				bool isValidResponse(uint16_t command, uint8_t* data, size_t size);

				static bool isError(uint32_t error);
				static std::string getErrorString(uint32_t error);
		};

		/**
		* @brief DmEfsManagerResponseError - Generic EFS Response Error
		* @super QcdmSerialError
		*/
		class DmEfsManagerResponseError : public QcdmSerialError
		{
			const DmEfsManagerResponseError& operator=(DmEfsManagerResponseError);
			std::string _what;
			int32_t code;
		public:
			DmEfsManagerResponseError(std::string message, int32_t code) : QcdmSerialError(message, code), _what(message), code(code) { 

			}
			DmEfsManagerResponseError(const DmEfsManagerResponseError& second)
				: QcdmSerialError(second._what, code), _what(second._what), code(second.code) {}
			virtual ~DmEfsManagerResponseError() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}
		};

		/**
		* @brief DmEfsManagerInvalidArgument
		* @super QcdmSerialInvalidArgument
		*/
		class DmEfsManagerInvalidArgument : public QcdmSerialInvalidArgument
		{
			const DmEfsManagerInvalidArgument& operator=(DmEfsManagerInvalidArgument);
			std::string _what;
		public:
			DmEfsManagerInvalidArgument(std::string message) : QcdmSerialInvalidArgument(message), _what(message)  {}
			DmEfsManagerInvalidArgument(const QcdmSerialInvalidArgument& second) : QcdmSerialInvalidArgument(second) {}
			virtual ~DmEfsManagerInvalidArgument() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}
		};

	}
}

#endif // _QC_DM_EFS_MANAGER_H_