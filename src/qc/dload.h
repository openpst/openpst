/**
* LICENSE PLACEHOLDER
*
* @file dload.h
* @package OpenPST
* @brief DLOAD definitions and structures
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _QC_DLOAD_H
#define _QC_DLOAD_H

/*
* Most of this is from Josuah Hill's DLOAD tool for iPhone
* Some spec is also available in document 80-39912-1 Rev. E  DMSS Download Protocol Interface Specification and Operational Description
* https://github.com/posixninja/DLOADTool/blob/master/dloadtool/dload.h
*/

/*
 DLOAD Commands

|------------------------|
| Revision Compatibility |
|------------------------|
| Revision 1 - 01 - 08   |  All these command IDs have the same functionality in both the bootrom (PBL) and 
| Revision 2 - 01 - 09   |  secondary bootloader (SBL), with the exception of command 0x14 for the Serial 
| Revision 3 - 01 - 0D   |  Number Read Request/Response used in PBL for protocol Rev 7. 
| Revision 4 - 01 - 0E   |
| Revision 5 - 01 - 0F   |  Command ID 0x14 for Serial Number Read Request/Response is deprecated from Rev 8 
| Revision 6 - 01 - 13   |  of the protocol and the new command ID 0x16 is used instead. 
| Revision 7 - 01 - 15   |
| Revision 8 - 01 - 18   |
| Revision 9 - 01 - 1A   |
|------------------------|
*/
enum DLOAD_COMMAND {
    DLOAD_WRITE                         = 0x01, // Write data to memory 
    DLOAD_ACK                           = 0x02, // Acknowledge receiving a packet 
    DLOAD_NAK                           = 0x03, // Negatively acknowledge a packet 
    DLOAD_ERASE                         = 0x04, // Erase a section of memory 
    DLOAD_GO                            = 0x05, // Go to a memory location and run 
    DLOAD_NOP                           = 0x06, // No operation, useful for debugging 
    DLOAD_PARAM_REQ                     = 0x07, // Request implementation information 
    DLOAD_PARAM_RESP                    = 0x08, // Response to a Parameter Request 
    DLOAD_MEMORY_DUMP                   = 0x09, // Debug-only; dump memory (optional
    DLOAD_RESET                         = 0x0A, // Perform a hardware reset 
    DLOAD_UNLOCK                        = 0x0B, // Unlock access to secured operations 
    DLOAD_SQ_VERSION_REQ                = 0x0C, // Return implementation version information 
    DLOAD_SQ_VERSION_RESP               = 0x0D, // Implementation software version information 
    DLOAD_POWERDOWN                     = 0x0E, // Turn phone power off 
    DLOAD_WRITE32                       = 0x0F, // Write data to memory using 32-bit address 
    DLOAD_MEMORY_DEBUG_QUERY            = 0x10, // Debug-only; request information on available memory-debug regions
    DLOAD_MEMORY_DEBUG_INFO             = 0x11, // Debug-only; response to Memory Debug Query 
    DLOAD_MEMORY_READ_REQ               = 0x12, // Request read of specified section of memory 
    DLOAD_MEMORY_READ_RESP              = 0x13, // Response to Memory Read Request; returns requested data 
    DLOAD_UNFRAMED_READ_REQ             = 0x14, // SBL Only - Request unframed read of specified section of memory 
    DLOAD_PBL_SERIAL_NUMBER_READ_REQ    = 0x14, // PBL Only - Request read of serial number from e-fuses 
    DLOAD_PBL_SERIAL_NUMBER_READ_RESP   = 0x14, // PBL Only - Response to Serial Number Read Request; returns serial number
    DLOAD_MEMORY_UNFRAMED_READ_RESP     = 0x15, // SBL Only - Response to Memory Unframed Read request; returns requested data 
    DLOAD_SERIAL_NUMBER_READ_REQ        = 0x16, // Request read of serial number from e-fuses 
    DLOAD_SERIAL_NUMBER_READ_RESP       = 0x16, // Response to Serial Number Read request; returns serial number 
    DLOAD_HARDWARE_ID_READ_REQ          = 0x17, // Request read of Hardware ID from efuses 
    DLOAD_HARDWARE_ID_READ_RESP         = 0x17, // Response to Hardware ID Read request; returns 64-bit Hardware ID 
    DLOAD_PUBLIC_KEY_HASH_READ_REQ      = 0x18, // Request to retrieve hash of the root of trust certificate 
    DLOAD_PUBLIC_KEY_HASH_READ_RESP     = 0x18, // Response to Public Key Hash Read; returns the hash value 
    DLOAD_QPST_COOKIE_READ_REQ          = 0x19, // Request to read QPST Cookie 
    DLOAD_QPST_COOKIE_READ_RESP         = 0x1A, // Response to QPST Cookie Read Request; returns the QPST cookie value
    DLOAD_SWITCH_TO_DLOAD               = 0x3A, // SBL Only - Command packet that  is echoed back by the Phone 
};

enum DLOAD_NCK_REASON {
    DLOAD_NCK_INVALID_FRAME_FCS             = 0x0001,
    DLOAD_NCK_INVALID_DESTINATION_ADDRESS   = 0x0002,
    DLOAD_NCK_INVALID_LENGTH                = 0x0003,
    DLOAD_NCK_UNEXPECTED_END_OF_PACKET      = 0x0004,
    DLOAD_NCK_DATA_LENGTH_TOO_LARGE         = 0x0005,
    DLOAD_NCK_INVALID_COMMAND               = 0x0006,
    DLOAD_NCK_OPERATION_FAILT               = 0x0007,
    DLOAD_NCK_WRONG_FLASH_INTELLIGENT_ID    = 0x0008,
    DLOAD_NCK_BAD_PROGRAMMING_VOLTAGE       = 0x0009,
    DLOAD_NCK_WRITE_VERIFY_FAILED           = 0x000A,
    DLOAD_NCK_UNLOCK_REQUIRED               = 0x000B,
    DLOAD_NCK_INCORRECT_SECURITY_CODE       = 0x000C,
    DLOAD_NCK_CANNOT_POWER_DOWN_PHONE       = 0x000D,
    DLOAD_NCK_OPERATION_NOT_PERMITTED       = 0x000E,
    DLOAD_NCK_INVALID_READ_ADDRESS          = 0x000F,
};

enum DLOAF_FLASH_DEVICE_SIZE {
    DLOAD_FLASH_DEVICE_SIZE_4MB         = 0x01,
    DLOAD_FLASH_DEVICE_SIZE_8MB         = 0x02,
    DLOAD_FLASH_DEVICE_SIZE_16MB_AS_8MB = 0x03,
    DLOAD_FLASH_DEVICE_SIZE_16MB        = 0x04,
    DLOAD_FLASH_DEVICE_SIZE_32MB        = 0x05,
    DLOAD_FLASH_DEVICE_SIZE_64MB        = 0x06,
};

PACKED(typedef struct dload_write_tx_t { // 0x01
    uint8_t command;
    uint32_t address;
    uint16_t size;
    uint8_t* data;
} dload_write_tx_t);

PACKED(typedef struct dload_ack_rx_t { // 0x02
    uint8_t command;
} dload_ack_rx_t);

PACKED(typedef struct dload_nck_rx_t { // 0x03
    uint8_t command;
    uint16_t reason;
} dload_nck_rx_t);

PACKED(typedef struct dload_erase_tx_t { // 0x04
    uint8_t command;
    uint32_t address;
} dload_erase_tx_t);

PACKED(typedef struct dload_go_tx_t { // 0x05
    uint8_t command;
    uint16_t address;
    uint16_t offset;
} dload_go_tx_t);

PACKED(typedef struct dload_nop_tx_t { // 0x06
    uint8_t command;
} dload_nop_tx_t);

PACKED(typedef struct dload_parameters_tx_t { // 0x07
    uint8_t command;
} dload_parameters_tx_t);

PACKED(typedef struct dload_parameters_rx_t { // 0x08
    uint8_t command;
    uint8_t version;
    uint8_t minVersion;
    uint16_t maxWriteSize;
    uint8_t model;
    uint8_t size;
    uint8_t type;
} dload_parameters_rx_t);

PACKED(typedef struct dload_dump_tx_t { // 0x09
    uint8_t command;
    uint32_t address;
    uint16_t length;
} dload_dump_tx_t);

PACKED(typedef struct dload_reset_tx_t { // 0x0A
    uint8_t command;
} dload_reset_tx_t);

PACKED(typedef struct dload_unlock_tx_t { // 0x0B
    uint8_t command;
    uint8_t securityCode[8];
} dload_unlock_tx_t);

PACKED(typedef struct dload_sw_version_tx_t { // 0x0C
    uint8_t command;
} dload_sw_version_tx_t);

PACKED(typedef struct dload_sw_version_rx_t { // 0x0D
    uint8_t command;
    uint8_t length;
    uint8_t version[20];
} dload_sw_version_rx_t);

PACKED(typedef struct dload_power_down_tx_t { // 0x0E
    uint8_t command;
} dload_power_down_tx_t);

#endif // _QC_DLOAD_H
