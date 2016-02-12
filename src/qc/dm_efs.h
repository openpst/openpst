/**
* LICENSE PLACEHOLDER
*
* @file dm.h
* @package OpenPST
* @brief QCDM definitions and structures
*
* @author Gassan Idriss <ghassani@gmail.com>
* @author Matteson Raab <mraabhimself@gmail.com>
*/

#ifndef _QC_DIAG_EFS_H
#define _QC_DIAG_EFS_H

#include "qc/dm.h"

#define DIAG_EFS_DEFAULT_WINDOW_SIZE  0x100000
#define DIAG_EFS_DEFAULT_WINDOW_BYTE_SIZE 0x100000

#define DIAG_EFS_VERSION     0x0001
#define DIAG_EFS_MIN_VERSION 0x0001
#define DIAG_EFS_MAX_VERSION 0x0001

enum DIAG_EFS_COMMAND {
    kDiagEfsHello = 0,  // Parameter negotiation packet               
    kDiagEfsQuery = 1,  // Send information about EFS2 params         
    kDiagEfsOpen = 2,  // Open a file                                
    kDiagEfsClose = 3,  // Close a file                               
    kDiagEfsRead = 4,  // Read a file                                
    kDiagEfsWrite = 5,  // Write a file                               
    kDiagEfsSymlink = 6,  // Create a symbolic link                     
    kDiagEfsReadLINK = 7,  // Read a symbolic link                       
    kDiagEfsUnlink = 8,  // Remove a symbolic link or file             
    kDiagEfsMkdir = 9,  // Create a directory                         
    kDiagEfsRmdir = 10,  // Remove a directory                         
    kDiagEfsOpenDIR = 11,  // Open a directory for reading               
    kDiagEfsReadDIR = 12,  // Read a directory                           
    kDiagEfsCloseDIR = 13,  // Close an open directory                    
    kDiagEfsRename = 14,  // Rename a file or directory                 
    kDiagEfsStat = 15,  // Obtain information about a named file      
    kDiagEfsLstat = 16,  // Obtain information about a symbolic link   
    kDiagEfsFstat = 17,  // Obtain information about a file descriptor 
    kDiagEfsChmod = 18,  // Change file permissions                    
    kDiagEfsStatFS = 19,  // Obtain file system information             
    kDiagEfsAccess = 20,  // Check a named file for accessibility       
    kDiagEfsDevInfo = 21,  // Get flash device info             
    kDiagEfsFactoryImageStart = 22,  // Start data output for Factory Image
    kDiagEfsFactoryImageRead = 23,  // Get data for Factory Image         
    kDiagEfsFactoryImageEnd = 24,  // End data output for Factory Image  
    kDiagEfsPrepareFactoryImage = 25,  // Prepare file system for image dump 
    kDiagEfsEfsDepreciated = 26,  // Write an EFS item file             
    kDiagEfsGetDepreciated = 27,  // Read an EFS item file              
    kDiagEfsError = 28,  // Send an EFS Error Packet back through DIAG 
    kDiagEfsExtendedInfo = 29,  // Get Extra information.                
    kDiagEfsChown = 30,  // Change ownership                      
    kDiagEfsBenchStartTest = 31,  // Start Benchmark               
    kDiagEfsBenchGetResults = 32,  // Get Benchmark Report          
    kDiagEfsBenchInit = 33,  // Init/Reset Benchmark          
    kDiagEfsSetReservation = 34,  // Set group reservation         
    kDiagEfsSetQuota = 35,  // Set group quota               
    kDiagEfsGetGroupInfo = 36,  // Retrieve Q&R values           
    kDiagEfsDeltree = 37,  // Delete a Directory Tree       
    kDiagEfsPut = 38,  // Write a EFS item file in order
    kDiagEfsGet = 39,  // Read a EFS item file in order 
    kDiagEfsTruncate = 40,  // Truncate a file by the name   
    kDiagEfsFtruncate = 41,  // Truncate a file by a descriptor 
    kDiagEfsStatVFS_V2 = 42,  // Obtains extensive file system info 
    kDiagEfsMd5Sum = 43,  // Calculate md5 hash of a file  
    kDiagEfsHotplugFormat = 44,  // Format a Connected device 
    kDiagEfsShred = 45,  // Shred obsolete file content. 
    kDiagEfsSetIdleDevEvntDur = 46,  // Idle_dev_evt_dur value in mins 
    kDiagEfsHotplugDeviceInfo = 47,  // get the hotplug device info.  
    kDiagEfsSyncNoWait = 48,  // non-blocking sync of remotefs device 
    kDiagEfsSyncGetStatus = 49,  // query previously issued sync status 
    kDiagEfsTruncate64 = 50,  // Truncate a file by the name.        
    kDiagEfsFtruncate64 = 51,  // Truncate a file by a descriptor.    
    kDiagEfsLseek64 = 52,  // Seek to requested file offset.      
    kDiagEfsMakeGoldenCopy = 53,  // Make golden copy for Remote Storage
    kDiagEfsFsImageOpen = 54,  //Open FileSystem Image extraction
    kDiagEfsFsImageRead = 55,  // Read File System Image.        
    kDiagEfsFsImageClose = 56,  // Close File System Image.      
};

enum kDiagEfsError {
    kDiagEfsErrorInconsistentState  = 0x40000001,
    kDiagEfsErrorInvalidSequence      = 0x40000002,
    kDiagEfsErrorDirNotOpen        = 0x40000003,
    kDiagEfsErrorDirEntNotFound    = 0x40000004,
    kDiagEfsErrorInvalidPath        = 0x40000005,
    kDiagEfsErrorPathTooLong       = 0x40000006,
    kDiagEfsErrorTooManyOpenDirs  = 0x40000007,
    kDiagEfsErrorInvalidDirEntry   = 0x40000008,
    kDiagEfsErrorTooManyOpenFiles = 0x40000009,
    kDiagEfsErrorUnknmownFileType    = 0x4000000A,
    kDiagEfsErrorNotNandFlash      = 0x4000000B,
    kDiagEfsErrorUnavailableInfo    = 0x4000000C,
};


enum DIAG_EFS_FILE_TYPES {
    kDiagEfsFileTypeFile         = 0x00,
    kDiagEfsFileTypeDir          = 0x01,
    kDiagEfsFileTypeLink         = 0x02,
    kDiagEfsFileTypeImmovable    = 0x03,
};

enum DIAG_EFS_DEVICE_TYPES {
    kDiagEfsDeviceTypeNor  = 0x00,
    kDiagEfsDeviceTypeNand = 0x01
};

enum DIAG_EFS_FILESYSTEM_IMAGE_TYPES {
    kDiagEfsFsTypeTar = 0,  
    kDiagEfsFsTypeZip = 1
};

PACKED(typedef struct QcdmEfsHelloRequest{
    QcdmSubsysHeader header;
    uint32_t targetPacketWindowSize;        // Window size in packets for sends from phone
    uint32_t targetPacketWindowByteSize;    // Window size in bytes for sends from phone
    uint32_t hostPacketWindowSize;          // Window size in packets for sends from host
    uint32_t hostPacketWindowByteSize;      // Window size in bytes for sends from host
    uint32_t dirIteratorWindowSize;         // Window size in packets for dir iteration
    uint32_t dirIteratorWindowByteSize;     // Window size in bytes for dir iteration
    uint32_t version;         
    uint32_t minVersion;     
    uint32_t maxVersion;     
    uint32_t featureBits;    
}) QcdmEfsHelloRequest;

PACKED(typedef struct QcdmEfsHelloResponse{
    QcdmSubsysHeader header;
    uint32_t targetPacketWindowSize;        // Window size in packets for sends from phone
    uint32_t targetPacketWindowByteSize;    // Window size in bytes for sends from phone
    uint32_t hostPacketWindowSize;          // Window size in packets for sends from host
    uint32_t hostPacketWindowByteSize;      // Window size in bytes for sends from host
    uint32_t dirIteratorWindowSize;         // Window size in packets for dir iteration
    uint32_t dirIteratorWindowByteSize;     // Window size in bytes for dir iteration
    uint32_t version;
    uint32_t minVersion;
    uint32_t maxVersion;
    uint32_t featureBits;
}) QcdmEfsHelloResponse;

/**
* Query Command
*/
PACKED(typedef struct QcdmEfsQueryRequest{
    QcdmSubsysHeader header;
}) QcdmEfsQueryRequest;

PACKED(typedef struct QcdmEfsQueryResponse{
    QcdmSubsysHeader header;
    int32_t maxFilenameLength;
    int32_t mapPathnameLength;
    int32_t maxSymlinkDepth;
    int32_t maxFileSize;
    int32_t maxDirectories;
    int32_t maxMounts;
}) QcdmEfsQueryResponse;

/**
* Open File Command
*/
PACKED(typedef struct QcdmEfsOpenFileRequest{
    QcdmSubsysHeader header;
    int32_t flags;
    int32_t mode;
    char path[1];
}) QcdmEfsOpenFileRequest;

PACKED(typedef struct QcdmEfsOpenFileResponse{
    QcdmSubsysHeader header;
    int32_t fp;
    int32_t error;
}) QcdmEfsOpenFileResponse;

/**
* Close File Command
*/
PACKED(typedef struct QcdmEfsCloseFileRequest{
    QcdmSubsysHeader header;
    int32_t fp;
}) QcdmEfsCloseFileRequest;

PACKED(typedef struct QcdmEfsCloseFileReponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsCloseFileReponse;

/**
* Read File Command
*/
PACKED(typedef struct QcdmEfsReadFileRequest{
    QcdmSubsysHeader header;
    int32_t fp;
    uint32_t size;
    uint32_t offset;
}) QcdmEfsReadFileRequest;

PACKED(typedef struct QcdmEfsReadFileResponse{
    QcdmSubsysHeader header;
    int32_t fp;
    uint32_t offset;
    int32_t bytesRead;
    int32_t error;
    uint8_t data[1]; // varies
}) QcdmEfsReadFileResponse;

/**
* Write File Command
*/
PACKED(typedef struct QcdmEfsWriteFileRequest{
    QcdmSubsysHeader header;
    int32_t fp;
    uint32_t offset;
    uint8_t data[1];
}) QcdmEfsWriteFileRequest;

PACKED(typedef struct QcdmEfsWriteFileResponse{
    QcdmSubsysHeader header;
    int32_t fp;
    uint32_t offset;
    int32_t bytesWritten;
    int32_t error;
}) QcdmEfsWriteFileResponse;

/**
* Create Symlink Command
*/
PACKED(typedef struct QcdmEfsCreateLinkRequest{
    QcdmSubsysHeader header;
    char path[1];
    char newPath[1];
}) QcdmEfsCreateLinkRequest;

PACKED(typedef struct QcdmEfsCreateLinkResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsCreateLinkResponse;

/**
* Read Symlink Command
*/
PACKED(typedef struct QcdmEfsReadLinkRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsReadLinkRequest;

PACKED(typedef struct QcdmEfsReadLinkResponse{
    QcdmSubsysHeader header;
    int32_t error;
    char path[1];
}) QcdmEfsReadLinkResponse;

/**
* Unlink Command
*/
PACKED(typedef struct QcdmEfsUnlinkRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsUnlinkRequest;

PACKED(typedef struct QcdmEfsUnlinkResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsUnlinkResponse;

/**
* Create Directory Command
*/
PACKED(typedef struct QcdmEfsMkdirRequest{
    QcdmSubsysHeader header;
    int16_t mode;
    char path[1];
}) QcdmEfsMkdirRequest;

PACKED(typedef struct QcdmEfsMkdirResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsMkdirResponse;

/**
* Remove Directory Command
*/
PACKED(typedef struct QcdmEfsRmdirRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsRmdirRequest;

PACKED(typedef struct QcdmEfsRmdirResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsRmdirResponse;

/**
* Open Directory Command
*/
PACKED(typedef struct QcdmEfsOpenDirRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsOpenDirRequest;

PACKED(typedef struct QcdmEfsOpenDirResponse{
    QcdmSubsysHeader header;
    uint32_t dp;
    int32_t error;
}) QcdmEfsOpenDirResponse;

/**
* Read Directory Command
*/
PACKED(typedef struct QcdmEfsReadDirRequest{
    QcdmSubsysHeader header;
    uint32_t dp;
    int32_t sequenceNumber;
}) QcdmEfsReadDirRequest;

PACKED(typedef struct QcdmEfsReadDirResponse{
    QcdmSubsysHeader header;
    uint32_t dp;
    int32_t sequenceNumber;
    int32_t error;
    int32_t entryType;
    int32_t mode;
    int32_t size;
    int32_t atime;
    int32_t mtime;
    int32_t ctime;
    char name[1];
}) QcdmEfsReadDirResponse;

/**
* Close Directory Command
*/
PACKED(typedef struct QcdmEfsCloseDirRequest{
    QcdmSubsysHeader header;
    uint32_t dp;
}) QcdmEfsCloseDirRequest;

PACKED(typedef struct QcdmEfsCloseDirResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsCloseDirResponse;

/**
* Rename Command
*/
PACKED(typedef struct QcdmEfsRenameRequest{
    QcdmSubsysHeader header;
    char path[1];
    char newPath[1];
}) QcdmEfsRenameRequest;

PACKED(typedef struct QcdmEfsRenameResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsRenameResponse;

/**
* Stat Command
*/
PACKED(typedef struct QcdmEfsStatRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsStatRequest;

PACKED(typedef struct QcdmEfsStatResponse{
    QcdmSubsysHeader header;
    int32_t error;
    int32_t mode;
    int32_t size;
    int32_t linkCount;
    int32_t atime;
    int32_t mtime;
    int32_t ctime;
}) QcdmEfsStatResponse;

/**
* Lstat Command
*/
PACKED(typedef struct QcdmEfsLstatRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsLstatRequest;

PACKED(typedef struct QcdmEfsLstatResponse{
    QcdmSubsysHeader header;
    int32_t error;
    int32_t mode;
    int32_t atime;
    int32_t mtime;
    int32_t ctime;
}) QcdmEfsLstatResponse;

/**
* Fstat Command
*/
PACKED(typedef struct QcdmEfsFstatRequest{
    QcdmSubsysHeader header;
    int32_t fp;
}) QcdmEfsFstatRequest;

PACKED(typedef struct QcdmEfsFstatResponse{
    QcdmSubsysHeader header;
    int32_t error;
    int32_t mode;
    int32_t size;
    int32_t linkCount;
    int32_t atime;
    int32_t mtime;
    int32_t ctime;
}) QcdmEfsFstatResponse;

/**
* CHMOD Command
*/
PACKED(typedef struct QcdmEfsChmodRequest{
    QcdmSubsysHeader header;
    int16_t mode;
    char path[1];
}) QcdmEfsChmodRequest;

PACKED(typedef struct QcdmEfsChmodResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsChmodResponse;

/**
* Statfs Command
*/
PACKED(typedef struct QcdmEfsStatfsRequest{
    QcdmSubsysHeader header;
    char path[1];
}) QcdmEfsStatfsRequest;

PACKED(typedef struct QcdmEfsStatfsResponse{
    QcdmSubsysHeader header;
    int32_t error;
    int32_t filesystemId;
    uint8_t filesystemType[8];
    int32_t blockSize;
    int32_t totalBlocks;
    int32_t availableBlocks;
    int32_t freeBlocks;
    int32_t maxFileSize;
    int32_t numberofFiles;
    int32_t maxNumberOfFiles;
}) QcdmEfsStatfsResponse;

/**
* Access Command
*/
PACKED(typedef struct QcdmEfsAccessRequest{
    QcdmSubsysHeader header;
    char permissionBits;
    char path[1];
}) QcdmEfsAccessRequest;

PACKED(typedef struct QcdmEfsAccessResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsAccessResponse;

/**
* Device Info Command
*/
PACKED(typedef struct QcdmEfsDeviceInfoRequest{
    QcdmSubsysHeader header;
}) QcdmEfsDeviceInfoRequest;

PACKED(typedef struct QcdmEfsDeviceInfoResponse{
    QcdmSubsysHeader header;
    int32_t error;
    int32_t totalNumberOfBlocks;
    int32_t pagesPerBlock;
    int32_t pageSize;
    int32_t totalPageSize;
    int32_t makerId;
    int32_t deviceId;
    uint8_t deviceType;
    uint8_t name[1];
}) QcdmEfsDeviceInfoResponse;


/**
* Factory Image Start
*/
PACKED(typedef struct QcdmEfsFactoryImageStartRequest{
    QcdmSubsysHeader header;
}) QcdmEfsFactoryImageStartRequest;

PACKED(typedef struct QcdmEfsFactoryImageStartResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsFactoryImageStartResponse;

/**
* Factory Image Read
*/
PACKED(typedef struct QcdmEfsFactoryImageReadRequest{
    QcdmSubsysHeader header;
    int8_t streamState;
    int8_t infoClusterSent;
    int16_t clusterMapSequence;
    int16_t clusterDataSequence;
}) QcdmEfsFactoryImageReadRequest;

PACKED(typedef struct QcdmEfsFactoryImageReadResponse{
    QcdmSubsysHeader header;
    int32_t error;
    int8_t streamState;
    int8_t infoClusterSent;
    int16_t clusterMapSequence;
    int16_t clusterDataSequence;
    uint8_t data[1]; // varies
}) QcdmEfsFactoryImageReadResponse;

/**
* Factory Image End
*/
PACKED(typedef struct QcdmEfsFactoryImageEndRequest{
    QcdmSubsysHeader header;
}) QcdmEfsFactoryImageEndRequest;

PACKED(typedef struct QcdmEfsFactoryImageEndResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsFactoryImageEndResponse;

/**
* Factory Image Prepare
*/
PACKED(typedef struct QcdmEfsFactoryImagePrepareRequest{
    QcdmSubsysHeader header;
}) QcdmEfsFactoryImagePrepareRequest;

PACKED(typedef struct QcdmEfsFactoryImagePrepareResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsFactoryImagePrepareResponse;

/**
* Chown
*/
PACKED(typedef struct QcdmEfsChownRequest{
    QcdmSubsysHeader header;
    uint32_t uid;
    uint32_t gid;
    uint8_t path[1]; // varies
}) QcdmEfsChownRequest;

PACKED(typedef struct QcdmEfsChownResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsChownResponse;

/**
* Set Reservation
*/
PACKED(typedef struct QcdmEfsSetReservationRequest{
    QcdmSubsysHeader header;
    uint32_t gid;
    size_t amount;
    uint8_t path[1]; // varies
}) QcdmEfsSetReservationRequest;

PACKED(typedef struct QcdmEfsSetReservationResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsSetReservationResponse;

/**
* Set Quota
*/
PACKED(typedef struct QcdmEfsSetQuotaRequest{
    QcdmSubsysHeader header;
    uint32_t gid;
    size_t amount;
    uint8_t path[1]; // varies
}) QcdmEfsSetQuotaRequest;

PACKED(typedef struct QcdmEfsSetQuotaResponse{
    QcdmSubsysHeader header;
    int32_t error;
}) QcdmEfsSetQuotaResponse;

/**
* Get Group Info
*/
PACKED(typedef struct QcdmEfsGetGroupInfoRequest{
    QcdmSubsysHeader header;
    uint32_t gid;
    uint8_t path[1]; // varies
}) QcdmEfsGetGroupInfoRequest;

PACKED(typedef struct QcdmEfsGetGroupInfoResponse{
    QcdmSubsysHeader header;
    int32_t error;
    uint32_t quotaSize;
    uint32_t reservationSize;
    uint32_t spaceUsed;
}) QcdmEfsGetGroupInfoResponse;

/**
* Deltree
*/
PACKED(typedef struct QcdmEfsDeltreeRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsDeltreeRequest;

PACKED(typedef struct QcdmEfsDeltreeResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsDeltreeResponse;


/**
* Truncate
*/
PACKED(typedef struct QcdmEfsTruncateRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint32_t length;
    uint8_t path[1]; // varies
}) QcdmEfsTruncateRequest;

PACKED(typedef struct QcdmEfsTruncateResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsTruncateResponse;

/**
* Ftruncate
*/
PACKED(typedef struct QcdmEfsFtncateRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint32_t length;
    int32_t fp;
}) QcdmEfsFtncateRequest;

PACKED(typedef struct QcdmEfsFtncateResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsFtncateResponse;

/**
* Statfs V2
*/
PACKED(typedef struct QcdmEfsStatfsV2Request{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsStatfsV2Request;

PACKED(typedef struct QcdmEfsStatfsV2Response{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
    uint32_t fsId;
    int8_t fsType[8];
    uint32_t blockSize;
    uint32_t totalBlocks;
    uint32_t availableBlocks;
    uint32_t freeBlocks;
    uint32_t maxFileSize;
    uint32_t fileCount;
    uint32_t maxFiles;
    uint32_t maxWriteSize;
    uint32_t maxPathSize;
    uint32_t caseSensitive;
    uint32_t casePreserving;
    uint32_t fMaxFileSize;
    uint32_t fFileSizeUnits;
    uint32_t fMaxOpenFiles;
    uint32_t fNameRule;
    uint32_t fEameEncoding;
}) QcdmEfsStatfsV2Response;


/**
* MD5 Sum
*/
PACKED(typedef struct QcdmEfsMd5SumRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1];
}) QcdmEfsMd5SumRequest;

PACKED(typedef struct QcdmEfsMd5SumResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
    uint8_t hash[16];
}) QcdmEfsMd5SumResponse;

/**
* Hotplug Format
*/
PACKED(typedef struct QcdmEfsHotplugFormatRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsHotplugFormatRequest;

PACKED(typedef struct QcdmEfsHotplugFormatResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsHotplugFormatResponse;

/**
* Shred
*/
PACKED(typedef struct QcdmEfsShredRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsShredRequest;

PACKED(typedef struct QcdmEfsShredResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint16_t results;
    int32_t error;
}) QcdmEfsShredResponse;

/**
* Hotplug Format
*/
PACKED(typedef struct QcdmEfsHotplugDeviceInfoRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsHotplugDeviceInfoRequest;

PACKED(typedef struct QcdmEfsHotplugDeviceInfoResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
    uint32_t manufacturerId;
    uint32_t oemId;
    uint32_t productRevision;
    uint8_t productName[16];
    uint8_t manufactureredDate[16];
}) QcdmEfsHotplugDeviceInfoResponse;

/**
* Sync
*/
PACKED(typedef struct QcdmEfsSyncRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsSyncRequest;

PACKED(typedef struct QcdmEfsSyncResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint32_t token;
    int32_t error;
}) QcdmEfsSyncResponse;

/**
* Sync Get Status
*/
PACKED(typedef struct QcdmEfsGetSyncStatusRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint32_t token;
    char path[1];
}) QcdmEfsGetSyncStatusRequest;

PACKED(typedef struct QcdmEfsGetSyncStatusResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t status;
    int32_t error;
}) QcdmEfsGetSyncStatusResponse;

/**
* Truncate64
*/
PACKED(typedef struct QcdmEfsTruncate64Request{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint32_t lengthLsb;
    uint32_t lengthMsb;
    uint8_t path[1]; // varies
}) QcdmEfsTruncate64Request;

PACKED(typedef struct QcdmEfsTruncate64Response{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsTruncate64Response;

/**
* Ftruncate64
*/
PACKED(typedef struct QcdmEfsFtruncate64Request{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint32_t lengthLsb;
    uint32_t lengthMsb;
    int32_t fp;
}) QcdmEfsFtruncate64Request;

PACKED(typedef struct QcdmEfsFtruncate64Response{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsFtruncate64Response;

/**
* Lseek64
*/
PACKED(typedef struct QcdmEfsLseek64Request{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int whence;
    uint32_t lengthLsb;
    uint32_t lengthMsb;
    int32_t fp;
}) QcdmEfsLseek64Request;

PACKED(typedef struct QcdmEfsLseek64Response{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
    uint32_t offsetLsb;
    uint32_t offsetMsb;
}) QcdmEfsLseek64Response;

/**
* Make golden copy
*/
PACKED(typedef struct QcdmEfsMakeGoldenCopyRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t path[1]; // varies
}) QcdmEfsMakeGoldenCopyRequest;

PACKED(typedef struct QcdmEfsMakeGoldenCopyResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsMakeGoldenCopyResponse;

/**
* File System Image Open
*/
PACKED(typedef struct QcdmEfsFsImageOpenRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint8_t imageType; // fs_diag_filesystem_image_type
    uint8_t path[1]; // varies
}) QcdmEfsFsImageOpenRequest;

PACKED(typedef struct QcdmEfsFsImageOpenResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t handle;
    int32_t error;
}) QcdmEfsFsImageOpenResponse;

/**
* File System Image Read
*/
PACKED(typedef struct QcdmEfsFsImageReadRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t handle; 
}) QcdmEfsFsImageReadRequest;

PACKED(typedef struct QcdmEfsFsImageReadResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    uint16_t length;
    int32_t error;
    uint8_t data[1]; // varies
}) QcdmEfsFsImageReadResponse;

/**
* File System Image Close
*/
PACKED(typedef struct QcdmEfsFsImageCloseRequest{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t handle;
}) QcdmEfsFsImageCloseRequest;

PACKED(typedef struct QcdmEfsFsImageCloseResponse{
    QcdmSubsysHeader header;
    uint16_t sequence;
    int32_t error;
}) QcdmEfsFsImageCloseResponse;

#endif // _QC_DIAG_EFS_H
