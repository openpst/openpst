/**
* LICENSE PLACEHOLDER
*
* @file sahara_serial.h
* @class SaharaSerial
* @package OpenPST
* @brief Sahara protocol serial port implementation
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _SERIAL_SAHARA_SERIAL_H_
#define _SERIAL_SAHARA_SERIAL_H_

#include "include/definitions.h"
#include "serial/serial.h"
#include "qc/sahara.h"
#include "qc/mbn.h"
#include "util/hexdump.h"
#include "util/sleep.h"
#include <iostream>
#include <fstream>

/**
* SaharaSerial
*
* Keep in mind when working with this library that sahara protocol dictates actions for
* most things. So when a request is mentioned its actually a request from the device not the
* host. So replys are actually responses. It is opposite from other QC protocols like QCDM, or 
* Streaming DLOAD.
* 
* Guest - You
* Host - Phone
* A general session works like this:
*  Guest -> Connect
*  Host -> Guest | Command: Hello
*  Guest -> Host | Command: Hello - Dictating version info and mode to work on
*  Host -> ACK or NCK | Branch depending on mode
*/
namespace OpenPST {
	namespace QC {

		typedef struct SaharaState {
			uint32_t mode; // see enum SaharaMode
			union {
				SaharaMemoryDebugRequest		memoryDebug;
				SaharaCommandReadyResponse		clientCommand;
				SaharaReadDataRequest			imageTransfer;
				SaharaEndImageTransferResponse  errorOrDone;
			};
		};

		class SaharaSerial : public serial::Serial {

			public:          
				/**
				* @brief SaharaSerial
				*
				* @param std::string port
				* @param int baudrate
				* @param serial::Timeout - Timeout, defaults to 1000ms
				*/
				SaharaSerial(std::string port, int baudrate = 115200, serial::Timeout timeout = serial::Timeout::simpleTimeout(1000));
            
				/**
				* @brief ~SaharaSerial
				*/
				~SaharaSerial();

				/**
				* @brief write
				* @overload Serial:: write(uint8_t *data, size_t size)
				* @return size_t
				*/
				size_t write(uint8_t *data, size_t size);

				/**
				* @brief read
				* @overload Serial::read(uint8_t *buf, size_t size)
				* @return size_t
				*/
				size_t read(uint8_t *buf, size_t size);

				/**
				* @brief write
				* @overload Serial::write(std::vector<uint8_t> &data)
				* @return size_t
				*/
				size_t write(std::vector<uint8_t> &data);

				/**
				* @brief read
				* @overload Serial::read(std::vector<uint8_t> &buffer, size_t size)
				* @return size_t
				*/
				size_t read(std::vector<uint8_t> &buffer, size_t size);

				/**
				* @brief readHello - Always start a session by reading hello
				* @return SaharaHelloRequest
				*/
				SaharaHelloRequest readHello();

				/**
				 * @brief sendHello
				 * @param uint32_t mode - @see enum SaharaMode
				 * @param uint32_t version -The version of sahara protocol to request, defaults to 2
				 * @param uint32_t minVersion - The minimum version we can support, defaults to 1
				 * @param uint32_t status - indicate to device our status, if set to anything other than 0x00, sahara protocol will abort
				 * @return SaharaState
				 */
				SaharaState sendHello(uint32_t mode, uint32_t version = 0x02, uint32_t minVersion = 0x01, uint32_t status = 0x00);

				/**
				 * @brief switchMode -  Switch the device mode out of command mode. You will manually
				 *						have to re-establish the hello handshake, and gives the option
				 *						to change modes yet again if needed. Alternately, you can use 
				 *						SaharaSerial::switchModeAndHello to automatically do this for you.
				 * @param uint32_t mode - @see enum SaharaMode
				 * @return int
				 */ 
				void switchMode(uint32_t mode);

				/**
				* @brief switchModeAndHello - Switch the device mode out of command mode and automatically
				*								establish the hello handshake.
				* @param uint32_t mode - @see enum SaharaMode
				* @return int
				*/
				SaharaState switchModeAndHello(uint32_t mode);

				/**
				 * @brief sendClientCommand
				 * @param uint32_t command - @see enum SaharaClientCommand
				 * @param responseData - A pointer to the memory allocated block with the command
										 response data if the client command returns some sort of
										 data like debug data, sn, chip info.
										 You will need to free this.
				 * @param responseDataSize - The size of the response data
				 * @return int
				 */
				std::vector<uint8_t> sendClientCommand(uint32_t command);

				/**
				 * @brief sendImage - Send a whole image file to the device. 
				 * @param std::string filePath
				 * @param size_t initialRequestSize - The amount to send, initially received from a SaharaReadDataRequest
				 * @return size_t - The total amount of bytes sent
				 */
				size_t sendImage(std::string filePath, size_t initialRequestSize);

				/**
				* @brief readNextImageChunkSize
				* @return SaharaReadDataRequest
				*/
				SaharaReadDataRequest readNextImageOffset();

				/**
				* @brief readMemory - Read size starting from address and 
				*                     store it in a memory allocated buffer
				*
				* @param uint32_t address - The starting address to read from
				* @param size_t size - If the size is over SAHARA_MAX_MEMORY_REQUEST_SIZE, it will read in chunks of SAHARA_MAX_MEMORY_REQUEST_SIZE
				* @param uint8_t** out - Pointer to the memory allocated buffer with the read data
				* @param size_t outSize - Total size of the read data
				* @return int
				*/
				size_t readMemory(uint32_t address, size_t size, std::vector<uint8_t>&out);
            
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
				size_t readMemory(uint32_t address, size_t size, std::string outFilePath);
            
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
				size_t readMemory(uint32_t address, size_t size, std::ofstream& out);


				/**
				* @brief sendDone - Sends the done command. In emergency mode this will
				*                    transition to the uploaded programmer
				* @return int
				*/
				void sendDone();
            
				/**
				 * @brief sendReset
				 * @return int
				 */
				void sendReset();

				/**
				 * @brief close
				 * @overload Serial::close
				 * @return void
				 */
				void close();

				/**
				 * @brief getNamedMode
				 * @param mode
				 * @return const char*
				 */
				std::string getNamedMode(uint32_t mode);

				/**
				 * @brief getNamedClientCommand
				 * @param command
				 * @return const char*
				 */
				std::string getNamedClientCommand(uint32_t command);

				/**
				 * @brief getNamedErrorStatus
				 *
				 * @param uint32_t status
				 * @return std::string
				 */
				std::string getNamedErrorStatus(uint32_t status);

				/**
				 * @brief getNamedRequestedImage
				 * @param uint32_t imageId
				 * @return const char*
				 */

				std::string getNamedRequestedImage(uint32_t imageId);


				void validateResponse(uint32_t expectedResponseCommand, SaharaHeader* data, size_t dataSize);

				/**
				* @brief isValidResponse - Check a response is the expected response by command
				* @param uint32_t expectedResponseCommand
				* @param uint8_t* data
				* @param size_t dataSize
				* @return bool
				*/
				bool isValidResponse(uint32_t expectedResponseCommand, SaharaHeader* data, size_t dataSize);

				bool isErrorResponse(SaharaHeader* data, size_t dataSize);
		};



		/**
		* @brief SaharaSerialError - Generic Sahara Serial exception
		* @super std::exception
		*/
		class SaharaSerialError : public std::exception
		{
			const SaharaSerialError& operator=(SaharaSerialError);
			std::string _what;
			uint8_t code;
		public:
			SaharaSerialError(std::string message, uint8_t code = 0) : _what(message), code(code)  { }
			SaharaSerialError(const SaharaSerialError& second) : _what(second._what), code(second.code) {}
			virtual ~SaharaSerialError() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}

			uint32_t getCode()
			{
				return code;
			}

		};

		/**
		* @brief SaharaSerialResponseError - Exception thrown when a method encounters an error
		* @super SaharaSerialError
		*/
		class SaharaSerialResponseError : public SaharaSerialError
		{
			const SaharaSerialResponseError& operator=(SaharaSerialResponseError);
			std::string _what;
			uint8_t code;
		public:
			SaharaSerialResponseError(std::string message, uint8_t code = 0) : SaharaSerialError(message), _what(message), code(code)  {}
			SaharaSerialResponseError(const SaharaSerialResponseError& second) : SaharaSerialError(second), _what(second._what), code(code) {}
			virtual ~SaharaSerialResponseError() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}
		};

		/**
		* @brief SaharaSerialInvalidArgument - Exception thrown when a method has an invalid argument
		* @super SaharaSerialError, std::invalid_argument
		*/
		class SaharaSerialInvalidArgument : public SaharaSerialError, std::invalid_argument
		{
			const SaharaSerialInvalidArgument& operator=(SaharaSerialInvalidArgument);
			std::string _what;
		public:
			SaharaSerialInvalidArgument(std::string message) : SaharaSerialError(message), invalid_argument(message), _what(message)  {}
			SaharaSerialInvalidArgument(const SaharaSerialInvalidArgument& second) : SaharaSerialError(second), invalid_argument(second), _what(second._what) {}
			virtual ~SaharaSerialInvalidArgument() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}
		};
	}
}

#endif /* _SERIAL_SAHARA_SERIAL_H_ */
