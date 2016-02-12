/**
* LICENSE PLACEHOLDER
*
* @file qcdm_serial.cpp
* @class QcdmSerial
* @package OpenPST
* @brief QCDM serial port implementation
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/


#ifndef _SERIAL_QCDM_SERIAL_H
#define _SERIAL_QCDM_SERIAL_H

#include "include/definitions.h"
#include "serial/serial.h"
#include "qc/hdlc_serial.h"
#include "qc/dm.h"
#include "qc/dm_nv.h"
#include "util/hexdump.h"

namespace OpenPST {
	namespace QC {
		class QcdmSerial : public HdlcSerial {

			enum QcdmOpertationResult {
				kQcdmIOError            = -2,
				kQcdmRequiresSecurity   = -1,
				kQcdmError              = 0,
				kQcdmSuccess            = 1
			};

			public:
				/**
				* @brief QcdmSerial - Constructor
				*
				* @param std::string port
				* @param int baudrate
				* @param serial::Timeout - Timeout, defaults to 1000ms
				*/
				QcdmSerial(std::string port, int baudrate, serial::Timeout timeout = serial::Timeout::simpleTimeout(1000));

				/**
				* @brief ~QcdmSerial - Destructor
				*/
				~QcdmSerial();

				/**
				* @brief getVersion
				* @return QcdmVersionResponse
				*
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				QcdmVersionResponse getVersion();

				/**
				* @brief getDiagVersion
				* @return uint16_t
				*
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				uint16_t getDiagVersion();

				/**
				* @brief getStatus
				* @return QcdmStatusResponse
				*
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				QcdmStatusResponse getStatus();

				/**
				* @brief getGuid
				* @return QcdmGuidResponse
				*
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				QcdmGuidResponse getGuid();
			
				/**
				 * @brief sendSpc
				 * @param spc - a 6 digit SPC code to unlock service programming
				 * @return bool
				 *
				 * @throws QcdmSerialInvalidArgument
				 * @throws QcdmSerialResponseError
				 * @throws serial::PortNotOpenedException
				 * @throws serial::IOException
				 */
				bool sendSpc(std::string spc);

				/**
				* @brief sendPassword
				* @param password - a 16 digit password to unlock secure operations
				* @return bool
				*
				* @throws QcdmSerialInvalidArgument
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				bool sendPassword(std::string password);

				/**
				* @brief sendPhoneMode
				* @param QcdmPhoneMode mode
				* @return bool
				*
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				bool setPhoneMode(QcdmPhoneMode mode);

				/**
				* @brief readNV - Read an item from non volatile memory
				* @param uint16_t itemId - NV Item ID to read
				* @param response - Response structure to populate the read data with
				* @return QcdmNvResponse
				*
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				QcdmNvResponse readNV(uint16_t itemId);

				/**
				* @brief writeNV - Write an item to non volatile memory
				* @param itemId - NV Item ID
				* @param uint8_t* data - NV Item Data
				* @param size_t size - Cannot exceed DIAG_NV_ITEM_SIZE
				* @return bool
				*
				* @throws QcdmSerialInvalidArgument
				* @throws QcdmSerialResponseError
				* @throws serial::PortNotOpenedException
				* @throws serial::IOException
				*/
				bool writeNV(uint16_t itemId, uint8_t* data, size_t size);

				QcdmNvPeekResponse peekNV(uint32_t address, uint8_t size);

				void switchToDload();

				bool sendHtcNvUnlock();
			   
				bool sendLgNvUnlock();  

				bool getLgSpc();

            

				int getErrorLog();
				int clearErrorLog();


				std::string lastError;
				uint8_t buffer[DIAG_MAX_PACKET_SIZE];
				size_t lastRxSize,
					   lastTxSize;

            
				void sendCommand(uint8_t command, bool validate = true);
				void sendCommand(uint8_t command, uint8_t* data, size_t size, bool validate = true);
				static bool isError(uint8_t responseCommand);			
				static std::string getErrorString(uint8_t responseCommand);
			private:

		};

		/**
		* @brief QcdmSerialError - Generic QCDM Serial exception
		* @super std::exception
		*/
		class QcdmSerialError : public std::exception
		{
			const QcdmSerialError& operator=(QcdmSerialError);
			std::string _what;
			uint8_t code;
		public:
			QcdmSerialError(std::string message, uint8_t code = 0) : _what(message), code(code)  { }
			QcdmSerialError(const QcdmSerialError& second) : _what(second._what), code(second.code) {}
			virtual ~QcdmSerialError() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}

			uint32_t getCode()
			{
				return code;
			}

		};

		/**
		* @brief QcdmSerialResponseError - Exception thrown when a method encounters an error
		* @super QcdmSerialError
		*/
		class QcdmSerialResponseError : public QcdmSerialError
		{
			const QcdmSerialResponseError& operator=(QcdmSerialResponseError);
			std::string _what;
			uint8_t code;
		public:
			QcdmSerialResponseError(std::string message, uint8_t code = 0) : QcdmSerialError(message), _what(message), code(code)  {}
			QcdmSerialResponseError(const QcdmSerialResponseError& second) : QcdmSerialError(second), _what(second._what), code(code) {}
			virtual ~QcdmSerialResponseError() throw() {}
			virtual const char* what() const throw () {		
				return _what.c_str();
			}
		};

		/**
		* @brief QcdmSerialInvalidArgument - Exception thrown when a method has an invalid argument
		* @super QcdmSerialError, std::invalid_argument
		*/
		class QcdmSerialInvalidArgument : public QcdmSerialError, std::invalid_argument
		{
			const QcdmSerialInvalidArgument& operator=(QcdmSerialInvalidArgument);
			std::string _what;
		public:
			QcdmSerialInvalidArgument(std::string message) : QcdmSerialError(message), invalid_argument(message), _what(message)  {}
			QcdmSerialInvalidArgument(const QcdmSerialInvalidArgument& second) : QcdmSerialError(second), invalid_argument(second), _what(second._what) {}
			virtual ~QcdmSerialInvalidArgument() throw() {}
			virtual const char* what() const throw () {
				return _what.c_str();
			}
		};
	}
}

#endif /* _SERIAL_QCDM_SERIAL_H */
