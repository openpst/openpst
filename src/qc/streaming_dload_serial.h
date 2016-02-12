/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload_serial.h
* @class StreamingDloadSerial
* @package OpenPST
* @brief Streaming DLOAD protocol serial port implementation
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _SERIAL_STREAMING_DLOAD_SERIAL_H
#define _SERIAL_STREAMING_DLOAD_SERIAL_H

#include <iostream>
#include <fstream>
#include "include/definitions.h"
#include "serial/serial.h"
#include "qc/hdlc_serial.h"
#include "util/hexdump.h"
#include "util/endian.h"
#include "qc/streaming_dload.h"
#include "qc/hdlc.h"

namespace OpenPST {
	namespace QC {
		struct StreamingDloadDeviceState {
			uint8_t openMode;
			uint8_t openMultiMode;
			StreamingDloadHelloResponse hello;
			StreamingDloadErrorResponse lastError;
			StreamingDloadLogResponse   lastLog;
		};

		enum StreamingDloadOperationResult {
			kStreamingDloadIOError = -1,
			kStreamingDloadError = 0,
			kStreamingDloadSuccess = 1
		};

		class StreamingDloadSerial : public HdlcSerial {

		public:
			/**
			* @brief holds the current device state data
			*/
			StreamingDloadDeviceState state;

			/**
			* @brief StreamingDloadSerial()
			*
			* @param std::string port - The device to connect to
			* @param int baudrate - Defaults to 115200
			* @param serial::Timeout - Timeout, defaults to 1000ms
			*/
			StreamingDloadSerial(std::string port, int baudrate = 115200, serial::Timeout timeout = serial::Timeout::simpleTimeout(1000));

			/**
			* @brief ~StreamingDloadSerial()
			*/
			~StreamingDloadSerial();

			/**
			* @brief sendHello - sends the initial handshake for the session
			*
			* @param std::string magic - The magic handhsake word. Should be QCOM FAST DOWNLOAD HOST
			* @param uint8_t version - The max version to be compatible with
			* @param uint8_t compatibleVersion - The lowest version to be compatible with
			* @param uint8_t a set of feature bits of what features to enable.
			*   @see qc/streaming_dload.h
			*
			* @return int
			*/
			int sendHello(std::string magic, uint8_t version, uint8_t compatibleVersion, uint8_t featureBits);

			/**
			* @brief sendUnlock - send the unlock command for implementations that require it
			*
			* @param std::string code - The unlock code
			*
			* @return int
			*/
			int sendUnlock(std::string code);

			/**
			* @brief setSecurityMode - set the session security mode
			*
			* @param uint8_t mode - The security mode to set to
			*
			* @return int
			*/
			int setSecurityMode(uint8_t mode);

			/**
			* @brief sendNop - send a NOP
			*
			* @return int
			*/
			int sendNop();

			/**
			* @brief sendReset - send a reset command
			*
			* @return int
			*/
			int sendReset();

			/**
			* @brief sendNop - send a power down command
			*
			* @return int
			*/
			int sendPowerOff();

			/**
			* @brief readEcc - read the current ECC state
			*
			* @param uint8_t& status - Will be set with the current status
			* @return int
			*/
			int readEcc(uint8_t& statusOut);

			/**
			* @brief setEcc - set the current ECC state
			*
			* @param uint8_t status - The status to set it to
			*
			* @return int
			*/
			int setEcc(uint8_t status);

			/**
			* @brief openMode - Open mode
			*
			* @param uint8_t mode - The mode to open
			*   @see qc/streaming_dload.h
			* @return int
			*/
			int openMode(uint8_t mode);

			/**
			* @brief closeMode - Close the current opened mode
			*
			* @return int
			*/
			int closeMode();

			/**
			* @brief openMultiImage - open mode for multi image devices
			*
			* @param uint8_t imageType - The image type to open
			*   @see qc/streaming_dload.h
			*
			* @return int
			*/
			int openMultiImage(uint8_t imageType);

			/**
			* @brief readAddress - Read x bytes from starting address
			*                      into a memory allocated array
			*
			* @param uint32_t address - The starting address
			* @param size_t length - The length to read from address
			* @param uint8_t** - The memory allocated array containing the read data until success or error encountered.
			* @param size_t& - The size of the memory allocated data
			* @param size_t chunkSize - The amount to request per read operation. The max size is 1024.
			*
			* @return int
			*/
			int readAddress(uint32_t address, size_t length, uint8_t** out, size_t& outSize, size_t chunkSize);

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
			int readAddress(uint32_t address, size_t length, std::vector<uint8_t> &out, size_t stepSize);

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
			int readAddress(uint32_t address, size_t length, std::ofstream& out, size_t &outSize, size_t stepSize);

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
			int writePartitionTable(std::string filePath, uint8_t& outStatus, bool overwrite = false);

			/**
			* @brief streamWrite - Stream write data starting at specified address. Writes hdlc encoded chunks
			*                   of max block size specified by device
			*
			* @param uint32_t address - The starting address to write to
			* @param uint8_t data - A pointer to the data to be written
			* @param size_t dataSize - The amount of data to write.
			* @param bool unframed - Write in unframed (non hdlc encoded) packets
			*
			* @return int
			*/
			int streamWrite(uint32_t address, uint8_t* data, size_t dataSize, bool unframed = false);

			/**
			* @brief readQfprom - Havent found a device or mode to use this in
			*/
			int readQfprom(uint32_t rowAddress, uint32_t addressType);

			/**
			* @brief getNamedError - Get a named error from an error code
			*
			* @param uint8_t code - The error code
			*
			* @return const char* msg
			*/
			const char* getNamedError(uint8_t code);

			/**
			* @brief getNamedOpenMode - Get a named mode from an open mode integer
			*
			* @param uint8_t mode - The mode
			*
			* @return const char* mode
			*/
			const char* getNamedOpenMode(uint8_t mode);

			/**
			* @brief getNamedMultiImage - Get a named image from an open multi image type
			*
			* @param uint8_t imageType - The image type id
			*
			* @return const char* name
			*/
			const char* getNamedMultiImage(uint8_t imageType);

		private:
			/**
			* @brief isValidResponse
			*
			* @param uint8_t expectedCommand,
			* @param uint8_t* response
			* @param size_t responseSize
			*/
			bool isValidResponse(uint8_t expectedCommand, uint8_t* response, size_t responseSize);

			/**
			* @brief isValidResponse
			*
			* @param uint8_t expectedCommand,
			* @param std::vector<uint8_t> response
			*/
			bool isValidResponse(uint8_t expectedCommand, std::vector<uint8_t> &response);

		};
	}
}

#endif // _SERIAL_STREAMING_DLOAD_SERIAL_H 
