/**
* LICENSE PLACEHOLDER
*
* @file sahara.h
* @package OpenPST
* @brief Sahara protocol definitions and structures
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _QC_SAHARA_H
#define _QC_SAHARA_H

#include "include/definitions.h"

#ifndef SAHARA_MAX_PACKET_SIZE
#define SAHARA_MAX_PACKET_SIZE 0x400
#endif

#ifndef SAHARA_LOG_LENGTH
#define SAHARA_LOG_LENGTH 0x64
#endif

#ifndef SAHARA_MAX_MEMORY_DATA_SIZE
#define SAHARA_MAX_MEMORY_DATA_SIZE 0x1000
#endif

#ifndef SAHARA_RAM_ZI_SIZE
#define SAHARA_RAM_ZI_SIZE 0x20000
#endif

#ifndef SAHARA_MAX_MEMORY_REQUEST_SIZE
#define SAHARA_MAX_MEMORY_REQUEST_SIZE 0x50000
#endif

/**
 * These are all known commands, both rx and tx.
 */
enum SaharaCommand {
    kSaharaCommandHello             = 0x01, // Initialize connection and protocol
    kSaharaCommandHelloResponse     = 0x02, // Acknowledge connection/protocol, mode of operation
    kSaharaCommandReadData          = 0x03, // Read specified number of bytes from host
    kSaharaCommandEndImageTransfer  = 0x04, // image transfer end / target transfer failure
    kSaharaCommandDone              = 0x05, // Acknowledgement: image transfer is complete
    kSaharaCommandDoneResponse      = 0x06, // Target is exiting protocol
    kSaharaCommandReset             = 0x07, // Instruct target to perform a reset
    kSaharaCommandResetResponse     = 0x08, // Indicate to host that target is about to reset
    kSaharaCommandMemoryDebug       = 0x09, // Indicate to host: target debug mode & ready to transfer memory content
    kSaharaCommandMemoryRead        = 0x0A, // Read number of bytes, starting from a specified address
    kSaharaCommandReady             = 0x0B, // Indicate to host: target ready to receive client commands,
    kSaharaCommandSwitchMode        = 0x0C, // Switch to a mode defined in enum SAHARA_MODE
    kSaharaCommandExecute           = 0x0D, // Indicate to host: to execute a given client command
    kSaharaCommandExecuteResponse	= 0x0E, // Indicate to host: target command execution status
    kSaharaCommandExecuteData		= 0x0F, // Indicate to target that host is ready to receive (more) data
    kSaharaCommandMemoryDebug64     = 0x10,
    kSaharaCommandMemoryRead64      = 0x11,
};

/**
 * SAHARA_MODE
 *
 * These are all known modes available
 * The initial hello lets you know which
 * mode the device is currently in.
 */
enum SaharaMode {
    kSaharaModeImageTxPending  = 0x00,
    kSaharaModeImageTxComplete = 0x01,  
    kSaharaModeMemoryDebug     = 0x02,
    kSaharaModeCommand         = 0x03
};

/**
 * SaharaClientCmd
 *
 * When in or switched to command mode, these are
 * all commands that are currently known
 */
enum SaharaClientCmd {
      kSaharaClientCmdNop                    = 0x00,
      kSaharaClientCmdSerialNumRead          = 0x01,
      kSaharaClientCmdMsmHWIDRead            = 0x02,
      kSaharaClientOemPkHashRead			 = 0x03,
      kSaharaClientCmdSwitchDMSS			 = 0x04, // haven't found a device/mode this works on
      kSaharaClientCmdSwitchToStreamingDload = 0x05, // haven't found a device/mode this works on
      kSaharaClientCmdReadDebugData          = 0x06,
      kSaharaClientCmdGetSblVersion			 = 0x07,
};

enum SaharaStatusCode {
    kSaharaStatusSuccess                      = 0x00,
    kSaharaStatusInvalidCmd                   = 0x01,
    kSaharaStatusProtocolMismatch             = 0x02,
    kSaharaStatusInvalidTargetProtocol        = 0x03,
    kSaharaStatusInvalidHostProtocol          = 0x04,
    kSaharaStatusInvalidPacketSize            = 0x05,
    kSaharaStatusUnexpectedImageId            = 0x06,
    kSaharaStatusInvalidHeaderSize            = 0x07,
    kSaharaStatusInvalidDataSize              = 0x08,
    kSaharaStatusInvalidImageType             = 0x09,
    kSaharaStatusInvalidTxLength              = 0x0A,
    kSaharaStatusInvalidRxLength              = 0x0B,
    kSaharaStatusTxRxError					  = 0x0C,
    kSaharaStatusReadDataError                = 0x0D,
    kSaharaStatusUnsupportedNumPhdrs          = 0x0E,
    kSaharaStatusInvalidPhdrSize              = 0x0F,
    kSaharaStatusMultipleSharedSeg            = 0x10,
    kSaharaStatusUninitPhdrLoc                = 0x11,
    kSaharaStatusInvalidDestAddress           = 0x12,
    kSaharaStatusInvalidImageHeaderSize       = 0x13,
    kSaharaStatusInvalidElfHeader             = 0x14,
    kSaharaStatusUnknownError				  = 0x15,
    kSaharaStatusTimeoutRx                    = 0x16,
    kSaharaStatusTimeoutTx                    = 0x17,
    kSaharaStatusInvalidMode				  = 0x18,
    kSaharaStatusInvalidMemoryRead            = 0x19,
    kSaharaStatusInvalidDataSizeRequest       = 0x1A,
    kSaharaStatusMemoryDebugNotSupported      = 0x1B,
    kSaharaStatusInvalidModeSwitch            = 0x1C,
    kSaharaStatusExecFailure				  = 0x1D,
    kSaharaStatusExecCmdInvalidParam          = 0x1E,
    kSaharaStatusExecCmdUnsupported           = 0x1F,
    kSaharaStatusExecDataInvalid			  = 0x20,
    kSaharaStatusHashTableAuthFailure         = 0x21,
    kSaharaStatusHashVerificationFailure      = 0x22,
    kSaharaStatusHashTableNotFound            = 0x23,
    kSaharaStatusTargetInitFailure            = 0x24,
    kSaharaStatusImageAuthFailure             = 0x25,
	kSaharaStatusInvalidImgHashTableSize	  = 0x26,
	kSaharaStatusMax
};

/**
* SaharaHeader
*
* The header all transmissions use
* with the exception of a few
*/
typedef struct SaharaHeader {
    uint32_t command;
    uint32_t size;
} SaharaHeader;

/**
* SaharaAbstractPacket
*/
typedef struct SaharaAbstractPacket {
    SaharaHeader header;
    uint32_t parameters[2];
} SaharaAbstractPacket;

/**
* SaharaHelloRequest
*
* The initial packet received from the device
* when first opening the COM port. If no data
* is available for read after the COM port is opened
* then the device may require a restart or it is not
* using sahara protocol
*/
typedef struct{ // 0x01
    SaharaHeader header;
    uint32_t version;
    uint32_t minVersion;
    uint32_t maxCommandPacketSize;
    uint32_t mode;
    uint32_t reserved[6];
} SaharaHelloRequest;

/**
* SaharaHelloResponse
*
* The initial packet sent to the device
* in response to the initial hello packet
*/
typedef struct { // 0x02
    SaharaHeader header;
    uint32_t version;
    uint32_t minVersion;
    uint32_t status; // ok or error
    uint32_t mode;
    uint32_t reserved[6];
} SaharaHelloResponse;

/**
* SaharaReadDataRequest
*
* When the device sends this packet
* it is requesting an image transfer and an initial
* chunk of the file for validation
*/
typedef struct { // 0x03
    SaharaHeader header;
    uint32_t imageId;
    uint32_t offset;
    uint32_t size;
} SaharaReadDataRequest;

/**
* SaharaEndImageTransferResponse
*
* When an error is encountered or an image transfer has
* ended.
*/
typedef struct { // 0x04
    SaharaHeader header;
    uint32_t file;
    uint32_t status;
} SaharaEndImageTransferResponse;

/*
* SaharaDoneRequest
*
* Sent to the device in response to a successful SaharaEndImageTransferResponse
*/
typedef struct { // 0x05
    SaharaHeader header;
} SaharaDoneRequest;

/*
* SaharaDoneResponse
*
* Received from the device after sending SaharaDoneRequest
*/
typedef struct { // 0x06
    SaharaHeader header;
    uint32_t imageTxStatus; // 0 pending, 1 complete
} SaharaDoneResponse;

/*
* SaharaResetRequest
*/
typedef struct { // 0x07
    SaharaHeader header;
} SaharaResetRequest;

/*
* SaharaResetResponse
*/
typedef struct { // 0x08
    SaharaHeader header;
} SaharaResetResponse;


typedef struct { // 0x09
    SaharaHeader header;
    uint32_t memoryTableAddress;
    uint32_t memoryTableLength;
} SaharaMemoryDebugRequest;

typedef struct { // 0x0a
  SaharaHeader header;
  uint32_t address;
  uint32_t size;
} SaharaMemoryReadRequest;

/**
* SaharaCommandReadyResponse
*
* Received from the device when in or switching to client
* command mode. ready to take a command (SaharaClientCommandRequest)
*/
typedef struct { // 0x0b
    SaharaHeader header;
    uint32_t imageTxStatus; // 0 pending, 1 complete
} SaharaCommandReadyResponse;

/**
* SaharaSwitchModeRequest
*
* Sent to the device to switch modes when in client command mode.
*
* Device should respond with SaharaHelloRequest
*/
typedef struct { // 0x0c
    SaharaHeader header;
    uint32_t mode;
} SaharaSwitchModeRequest;

/**
* SaharaClientCommandRequest
*
* Execute a client command
*
* If the command is invalid you will receive
* SaharaEndImageTransferResponse
*/
typedef struct { // 0x0d
    SaharaHeader header;
    uint32_t command;
} SaharaClientCommandRequest;


/**
* SaharaClientCommandResponse
*
* Received from the device in response to SaharaClientCommandRequest
* with the size of the data expected back from the command execution
*
*/
typedef struct { // 0x0e
    SaharaHeader header;
    uint32_t command;
    uint32_t size;
} SaharaClientCommandResponse;

/**
* SaharaClientCommandExecuteDataRequest
*
* Sent in response to SaharaClientCommandResponse
* indicating we are ready to receive the data of n size
* defined in SaharaClientCommandResponse
*
* After sent, read the size of data from the device
*/
typedef struct { // 0x0f
    SaharaHeader header;
    uint32_t command;
} SaharaClientCommandExecuteDataRequest;

/**
 *
 * SaharaMemoryDebug64Request
 */
typedef struct { // 0x10
    SaharaHeader header;
    uint32_t memoryTableAddress;
    uint32_t memoryTableLength;
} SaharaMemoryDebug64Request;

/**
 * SaharaMemoryRead64Request
 */
typedef struct { // 0x11
    SaharaHeader header;
    uint64_t address;
    uint64_t size;
} SaharaMemoryRead64Request;


/**
* SaharaMsmHwIdResponse
*
* represents a response from client command:
*	kSaharaClientCmdMsmHWIDRead
*/
typedef struct {
    uint16_t unknown1;
    uint16_t unknown2;
    uint16_t msmId;
} SaharaMsmHwIdResponse;

/**
* SaharaSerialNumberResponse
*
* represents a response from client command:
*	kSaharaClientCmdSerialNumRead
*/
typedef struct {
    uint32_t serial;
} SaharaSerialNumberResponse;

/**
* SaharaSblVersionResponse
*
* represents a response from client command:
*	kSaharaClientCmdGetSblVersion
*/
typedef struct {
    uint32_t version;
} SaharaSblVersionResponse;

/**
* SaharaSblVersionResponse
*
* represents a response from client command
* kSaharaClientOemPkHashRead
*/
typedef struct {
    uint8_t hash[32];
} SaharaOemPkHashResponse;

/**
* SaharaDebugLogEntry
*
* represents a single log entry from client command
* kSaharaClientCmdReadDebugData response
*/
typedef struct {
    uint8_t message[SAHARA_LOG_LENGTH];
} SaharaDebugLogEntry;

/**
* SaharaMemoryTableEntry
*
* This structure represents the memory table entry
* when reading the memory table from the specified address
* of a kSaharaCommandMemoryDebug response
* 
* The total number of entries would be response.size / sizeof(SaharaMemoryTableEntry)
*/
PACKED(typedef struct {
    uint32_t   unknown1;
    uint32_t   address;
    uint32_t   size;
    uint8_t    name[20];
    uint8_t    filename[20];
}) SaharaMemoryTableEntry;


#endif // _QC_SAHARA_H
