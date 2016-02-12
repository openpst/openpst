/**
* LICENSE PLACEHOLDER
*
* @file streaming_dload.h
* @package OpenPST
* @brief Streaming DLOAD definitions and structures
* @see DCN 80-V5348-1 J
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

/*
 * 
 */
#ifndef _QC_STREAMING_DLOAD_H
#define _QC_STREAMING_DLOAD_H

#include "include/definitions.h"

#define STREAMING_DLOAD_FEATURE_BIT_UNCOMPRESSED_DOWNLOAD   0x00000001
#define STREAMING_DLOAD_FEATURE_BIT_NAND_BOOTABLE_IMAGE     0x00000002
#define STREAMING_DLOAD_FEATURE_BIT_NAND_BOOT_LOADER        0x00000004
#define STREAMING_DLOAD_FEATURE_BIT_MULTI_IMAGE             0x00000008
#define STREAMING_DLOAD_FEATURE_BIT_SECTOR_ADDRESSES        0x00000010

#define kStreamingDloadSecurityMode_UNTRUSTED             0x00
#define kStreamingDloadSecurityMode_TRUSTED               0x01

#define kStreamingDloadPartitionTable_ACCEPTED                    0x00
#define kStreamingDloadPartitionTable_DIFFERS_OVERRIDE_ACCEPTED   0x01
#define kStreamingDloadPartitionTable_FORMAT_NOT_RECOGNIZED       0x02
#define kStreamingDloadPartitionTable_ERASE_FAILED                0x03

#define kStreamingDloadOpen_MULTI_OPEN_SUCCESS                     0x00
#define kStreamingDloadOpen_MULTI_PAYLOAD_LENGTH_EXCEEDED          0x01
#define kStreamingDloadOpen_MULTI_NO_PAYLOAD_EXPECTED              0x02
#define kStreamingDloadOpen_MULTI_PAYLOAD_REQUIRED_FAIL            0x03
#define kStreamingDloadOpen_MULTI_BLOCK_WRITE_PROTECTED            0x04

#define STREAMING_DLOAD_MAGIC_SIZE 32
#define STREAMING_DLOAD_FLASH_ID_MAX_SIZE    32
#define STREAMING_DLOAD_MESSAGE_SIZE  64
#define STREAMING_DLOAD_MAX_SECTORS 32

#define STREAMING_DLOAD_MAX_DATA_SIZE 1024

#define STREAMING_DLOAD_MAX_TX_SIZE (STREAMING_DLOAD_MAX_DATA_SIZE * 2)
#define STREAMING_DLOAD_MAX_RX_SIZE (STREAMING_DLOAD_MAX_DATA_SIZE * 2)

enum StreamingDloadCommand {
    // 0x00 - Illegal
    kStreamingDloadHello                        = 0x01,
    kStreamingDloadHelloResponse               = 0x02,
    kStreamingDloadRead                         = 0x03,
    kStreamingDloadReadData                    = 0x04,
    kStreamingDloadSimpleWrite                 = 0x05,
    kStreamingDloadWriteOk                     = 0x06,
    kStreamingDloadStreamWrite                 = 0x07,
    kStreamingDloadBlockWritten                = 0x08,
    kStreamingDloadNop                          = 0x09,
    kStreamingDloadNopResponse                 = 0x0A,
    kStreamingDloadReset                        = 0x0B,
    kStreamingDloadReset_ACK                    = 0x0C,
    kStreamingDloadError                        = 0x0D,
    kStreamingDloadLog                          = 0x0E,
    kStreamingDloadUnlock                       = 0x0F,
    kStreamingDloadUnlocked                     = 0x10,
    kStreamingDloadPowerOff                    = 0x11,
    kStreamingDloadPoweringDown                = 0x12,
    kStreamingDloadOpen                         = 0x13,
    kStreamingDloadOpened                       = 0x14,
    kStreamingDloadClose                        = 0x15,
    kStreamingDloadClosed                       = 0x16,
    kStreamingDloadSecurityMode                = 0x17,
    kStreamingDloadSecurityModeReceived       = 0x18,
    kStreamingDloadPartitionTable              = 0x19,
    kStreamingDloadPartitionTableReceived     = 0x1A,
    kStreamingDloadOpenMultiImage             = 0x1B,
    kStreamingDloadOpenedMultiImage           = 0x1C,
    kStreamingDloadEraseFlash                  = 0x1D,
    kStreamingDloadFlashErased                 = 0x1E,
    kStreamingDloadGetEccState                = 0x1F,
    kStreamingDloadCurrentEccState            = 0x20,
    kStreamingDloadSetEcc                      = 0x21,
    kStreamingDloadSetEccResponse             = 0x22,
    kStreamingDloadCalculateSha1          = 0x23, // depreciated - doesnt appear to be used
    kStreamingDloadCalculateSha1Response = 0x24, // depreciated - doesnt appear to be used
    kStreamingDloadInfoResponse                = 0x24, 

    //25-2F commands described in [Q2]

    kStreamingDloadUnframedStreamWrite           = 0x30,
    kStreamingDloadUnframedStreamWriteResponse  = 0x31,
    kStreamingDloadWfpromWrite                    = 0x32,
    kStreamingDloadWfpromWriteResponse           = 0x33,
    kStreamingDloadQfpromRead                     = 0x34,
    kStreamingDloadQfpromReadResponse            = 0x35

    //36-FE Reserved
};

enum kStreamingDloadErrorCode {
    kStreamingDloadErrorInvalidDestinationAddress     = 0x02,
    kStreamingDloadErrorInvalidLength                        = 0x03,
    kStreamingDloadErrorUnexpectedPacketEnd              = 0x04,
    kStreamingDloadErrorInvalidCommand                       = 0x05,
    kStreamingDloadErrorWrongFlashIntelligentId            = 0x08,
    kStreamingDloadErrorBadProgrammingVoltage               = 0x09,
    kStreamingDloadErrorWriteVerifyFailed                   = 0x0A,
    kStreamingDloadErrorIncorrectSecurityCode               = 0x0C,
    kStreamingDloadErrorCannotPowerDown                     = 0x0D,
    kStreamingDloadErrorNandProgrammingUnsupported  = 0x0E,
    kStreamingDloadErrorCommandOutOfSequence                = 0x0F,
    kStreamingDloadErrorCloseDidNotSucceed                 = 0x10,
    kStreamingDloadErrorIncompatibleFeatureBits            = 0x11,
    kStreamingDloadErrorOutOfSpace                          = 0x12,
    kStreamingDloadErrorInvalidSecurityMode                 = 0x13,
    kStreamingDloadErrorMultiImageNandUnsupported        = 0x14,
    kStreamingDloadErrorPowerOffCmdUnsupported       = 0x15
    // 0x16 - 0x1A RESERVED FOR ERROR REASONS IN [Q2]
};

enum StreamingDloadOpenMode {
    kStreamingDloadOpenModeBootloader       = 0x01,
    kStreamingDloadOpenModeBootableImage   = 0x02,
    kStreamingDloadOpenModeCefs       = 0x03,
    kStreamingDloadOpenModeFactory                   = 0x04
};

enum StreamingDloadOpenMultiMode {
    kStreamingDloadOpenModeMultiNone        = 0x00,  
    kStreamingDloadOpenModeMultiPbl         = 0x01,  // Primary boot loader
    kStreamingDloadOpenModeMultiQcsblhHdCfg  = 0x02,  // Qualcomm secondary boot loader header and config data 
    kStreamingDloadOpenModeMultiQcsbl       = 0x03,  // Qualcomm secondary boot loader
    kStreamingDloadOpenModeMultiOemsbl      = 0x04,  // OEM secondary boot loader (max payload: 512 octets)
    kStreamingDloadOpenModeMultiAmss        = 0x05,  // AMSS modem executable (max payload: 512 octets)
    kStreamingDloadOpenModeMultiApps        = 0x06,  // AMSS applications executable (max payload: 512 octets)
    kStreamingDloadOpenModeMultiObl         = 0x07,  // MSM6250 OTP boot loader 
    kStreamingDloadOpenModeMultiFotaui      = 0x08,  // FOTA UI binary 
    kStreamingDloadOpenModeMultiCefs        = 0x09,  // Compact EFS2 image
    kStreamingDloadOpenModeMultiAppsBL      = 0x0A,  // AMSS applications boot loader (max payload: 512 octets)
    kStreamingDloadOpenModeMultiApps_CEFS   = 0x0B,  // Apps CEFS image
    kStreamingDloadOpenModeMultiFlashBin   = 0x0C,  // Flash.bin for Windows Mobile
    kStreamingDloadOpenModeMultiDsp1        = 0x0D,  // DSP1 runtime image
    kStreamingDloadOpenModeMultiCustom      = 0x0E,  // Image for user-defined partition User (max payload: 512 octets)
    kStreamingDloadOpenModeMultiDbl         = 0x0F,  // DBL image for Secure  Boot 2.0 architecture 
    kStreamingDloadOpenModeMultiOsbl        = 0x10,  // OSBL image for Secure  Boot 2.0 architecture 
    kStreamingDloadOpenModeMultiFsbl        = 0x11,  // FSBL image for Secure  Boot 2.0 architecture 
    kStreamingDloadOpenModeMultiDsp2        = 0x12,  // DSP2 executable 
    kStreamingDloadOpenModeMultiRaw         = 0x13,  // Apps EFS2 raw image 
    kStreamingDloadOpenModeMultiRofs1       = 0x14,  // Symbian
    kStreamingDloadOpenModeMultiRofs2       = 0x15,  // Symbian
    kStreamingDloadOpenModeMultiRofs3       = 0x16,  // Symbian
                                                         // 0x17-0x1F - RESERVED
    kStreamingDloadOpenModeMultiEmmcUser   = 0x21,  // EMMC card USER partition image 
    kStreamingDloadOpenModeMultiEmmcBoot0  = 0x22,  // EMMC card BOOT0 partition image 
    kStreamingDloadOpenModeMultiEmmcBoot1  = 0x23,  // EMMC card BOOT1 partition image 
    kStreamingDloadOpenModeMultiRpmb   = 0x24,  // partition1
    kStreamingDloadOpenModeMultiGpp1   = 0x25,  // partition1-4
    kStreamingDloadOpenModeMultiGpp2   = 0x26,   
    kStreamingDloadOpenModeMultiGpp3   = 0x27,   
    kStreamingDloadOpenModeMultiGpp4   = 0x28    
};

PACKED(typedef struct { // 0x01
    uint8_t command;
    uint8_t magic[STREAMING_DLOAD_MAGIC_SIZE];
    uint8_t version;
    uint8_t compatibleVersion;
    uint8_t featureBits;
}) StreamingDloadHelloRequest;

PACKED(typedef struct { // 0x02
    uint8_t  command;
    uint8_t  magic[STREAMING_DLOAD_MAGIC_SIZE];
    uint8_t  version;
    uint8_t  compatibleVersion;
    uint32_t maxPreferredBlockSize;
    uint32_t baseFlashAddress;
    uint8_t  flashIdLength;
}) StreamingDloadHelloResponseHeader;

PACKED(typedef struct { // 0x02
    uint8_t  command;
    uint8_t  magic[STREAMING_DLOAD_MAGIC_SIZE];
    uint8_t  version;
    uint8_t  compatibleVersion;
    uint32_t maxPreferredBlockSize;
    uint32_t baseFlashAddress;
    uint8_t  flashIdLength;
    uint8_t  flashIdenfier[STREAMING_DLOAD_FLASH_ID_MAX_SIZE];
    uint16_t windowSize;
    uint16_t numberOfSectors;
    uint16_t sectorSizes[STREAMING_DLOAD_MAX_SECTORS * 4];
    uint16_t featureBits;
}) StreamingDloadHelloResponse;

PACKED(typedef struct { // 0x03
    uint8_t command;
    uint32_t address;
    uint16_t length;
}) StreamingDloadReadRequest;

PACKED(typedef struct { // 0x04
    uint8_t command;
    uint32_t address;
    uint8_t data[1]; // varies
}) StreamingDloadReadResponse;

PACKED(typedef struct {// 0x05
    uint8_t command;
    uint32_t address;
    uint8_t* data;
}) StreamingDloadSimpleWriteRequest;

PACKED(typedef struct { // 0x06
    uint8_t command;
    uint32_t address;
}) StreamingDloadSimpleWriteResponse;

PACKED(typedef struct {// 0x07
    uint8_t command;
    uint32_t address;
    uint8_t data[1]; // varies
}) StreamingDloadStreamWriteRequest;

PACKED(typedef struct { // 0x08
    uint8_t command;
    uint32_t address;
}) StreamingDloadStreamWriteResponse;

PACKED(typedef struct {// 0x09
    uint8_t command;
    uint32_t identifier; // this can be any amount of data. its just sent back to us in the response to validate the NOP call
}) StreamingDloadNopRequest;

PACKED(typedef struct { // 0x0A
    uint8_t command;
    uint32_t identifier; // response should have same as in transmitted packet
}) StreamingDloadNopResponse;

PACKED(typedef struct {// 0x0B
    uint8_t command;
}) StreamingDloadResetRequest;

PACKED(typedef struct { // 0x0C
    uint8_t command;
}) StreamingDloadResetResponse;

PACKED(typedef struct { // 0x0D
    uint8_t command;
    uint32_t code;
    uint8_t text[STREAMING_DLOAD_MESSAGE_SIZE];
}) StreamingDloadErrorResponse;

PACKED(typedef struct { // 0x0E
    uint8_t command;
    uint8_t text[STREAMING_DLOAD_MESSAGE_SIZE];
}) StreamingDloadLogResponse;

PACKED(typedef struct { // 0x0F
    uint8_t command;
    uint64_t code;
}) StreamingDloadUnlockRequest;

PACKED(typedef struct { // 0x10
    uint8_t command;
}) StreamingDloadUnlockResponse;

PACKED(typedef struct { // 0x11
    uint8_t command;
}) StreamingDloadPowerOffRequest;

PACKED(typedef struct { // 0x12
    uint8_t command;
}) StreamingDloadPowerOffResponse;

PACKED(typedef struct { // 0x13
    uint8_t command;
    uint8_t mode;
}) StreamingDloadOpenRequest;

PACKED(typedef struct { // 0x14
    uint8_t command;
}) StreamingDloadOpenResponse;

PACKED(typedef struct { // 0x15
    uint8_t command;
}) StreamingDloadCloseRequest;

PACKED(typedef struct { // 0x16
    uint8_t command;
}) StreamingDloadCloseResponse;

PACKED(typedef struct { // 0x17
    uint8_t command;
    uint8_t mode; // 0x00 - Untrusted, 0x01 - Trusted
}) StreamingDloadSecurityModeRequest;

PACKED(typedef struct { // 0x18
    uint8_t command;
}) StreamingDloadSecurityModeResponse;

PACKED(typedef struct { // 0x19
    uint8_t command;
    uint8_t overrideExisting; // 0x00 no override, 0x01 override existing table
    uint8_t data[512]; // max 512 bytes
}) StreamingDloadPartitionTableRequest;


PACKED(typedef struct { // 0x1A
    uint8_t command;
    uint8_t status; // 0x0 – Partition table accepted
                    // 0x1 – Partition table differs, override is accepted
                    // 0x2 – Partition table format not recognized, does not accept override
                    // 0x3 – Erase operation failed
}) StreamingDloadPartitionTableResponse;

PACKED(typedef struct  { // 0x1B
    uint8_t command;
    uint8_t type;
}) StreamingDloadOpenMultiImageRequest;

PACKED(typedef struct { // 0x1B
    uint8_t command;
    uint8_t type;
}) StreamingDloadOpenMultiImageWithPayloadRequest;

PACKED(typedef struct { // 0x1C
    uint8_t command;
    uint8_t status; // 0x00 Open successful
                    // 0x01 Payload length exceeded, fail
                    // 0x02 No payload expected, fail
                    // 0x03 Payload required, fail
                    // 0x04 Block 0 write protected, fail
}) StreamingDloadOpenMultiImageResponse;

/*
* This packet will erase the whole flash. If the operation fails, you may
* have to restore the device via jtag. Use with caution
*/
PACKED(typedef struct { // 0x1D
    uint8_t command;
}) StreamingDloadEraseFlashRequest;

PACKED(typedef struct { // 0x1E
    uint8_t command;
}) StreamingDloadEraseFlashResponse;


PACKED(typedef struct { // 0x1F
    uint8_t command;
}) StreamingDloadGetEccStateRequest;

PACKED(typedef struct { // 0x20
    uint8_t command;
    uint8_t status; // 0x00 ECC generation/check disabled
                    // 0x01 ECC generation/check enabled
}) StreamingDloadGetEccStateResponse;

PACKED(typedef struct { // 0x21
    uint8_t command;
    uint8_t status; // 0x00 disable, 0x01 enabled
}) StreamingDloadSetEccStateRequest;

PACKED(typedef struct { // 0x22
    uint8_t command;
}) StreamingDloadSetEccStateResponse;


PACKED(typedef struct { // 0x30
    uint8_t command;
    uint8_t padding[2]; // should be set to 0x000000
    uint32_t address;
    uint32_t length;
    uint8_t* data;
}) StreamingDloadUnframedStreamWriteRequest;

PACKED(typedef struct { // 0x31
    uint8_t command;
    uint32_t address;
}) StreamingDloadUnframedStreamWriteResponse;

PACKED(typedef struct { // 0x32
    uint8_t command;
    uint32_t rowAddress;
    uint32_t lsb;
    uint32_t msb;
}) StreamingDloadQfpromWriteRequest;

PACKED(typedef struct { // 0x33
    uint8_t command;
    uint32_t rowAddress;
}) StreamingDloadQfpromWriteResponse;


PACKED(typedef struct { // 0x34
    uint8_t command;
    uint32_t rowAddress;
    uint32_t addressType; // 0x00 - Corrected Reads, 0x01 - Raw Reads
    uint32_t msb;
}) StreamingDloadQfpromReadRequest;


PACKED(typedef struct { // 0x35
    uint8_t command;
    uint32_t rowAddress;
    uint32_t lsb;
    uint32_t msb;
}) StreamingDloadQfpromReadResponse;



#endif // _QC_STREAMING_DLOAD_H

