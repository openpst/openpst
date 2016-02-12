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

#ifndef _QC_DIAG_H
#define _QC_DIAG_H

#include "include/definitions.h"

#define DIAG_NV_ITEM_SIZE   128
#define DIAG_NV_PEEK_MAX_SIZE   32
#define DIAG_MAX_PACKET_SIZE (2048 * 2)
#define DIAG_RX_TIMEOUT (15*1000)
#define DIAG_SPC_LENGTH 6
#define DIAG_PASSWORD_LENGTH 16

enum QcdmCommand {
    kDiagVerno = 0x00, // 0
    kDiagEsn = 0x01, // 1
    kDiagMemoryPeekByte = 0x02, // 2
    kDiagMemoryPeekWord = 0x03, // 3
    kDiagMemoryPeekDword = 0x04, // 4
    kDiagMemoryPokeByte = 0x05, // 5
    kDiagMemoryPokeWord = 0x06, // 6
    kDiagMemoryPokeDword = 0x07, // 7
    kDiagOutputByte = 0x08, // 8
    kDiagOutputWord = 0x09, // 9
    kDiagInputByte = 0x0a, // 10
    kDiagInputWord = 0x0b, // 11
    kDiagStatus = 0x0c, // 12
    kDiagLogmask = 0x0f, // 15
    kDiagLog = 0x10, // 16
    kDiagNvPeek = 0x11, // 17
    kDiagNvPoke = 0x12, // 18
    kDiagBadCmd = 0x13, // 19
    kDiagBadParm = 0x14, // 20
    kDiagBadLen = 0x15, // 21
    kDiagBadMode = 0x18, // 24
    kDiagTagraph = 0x19, // 25
    kDiagMarkov = 0x1a, // 26
    kDiagMarkovReset = 0x1b, // 27
    kDiagDiagVer = 0x1c, // 28
    kDiagTs = 0x1d, // 29
    kDiagTaParm = 0x1e, // 30
    kDiagMsg = 0x1f, // 31
    kDiagHsKey = 0x20, // 32
    kDiagHsLock = 0x21, // 33
    kDiagHsScreen = 0x22, // 34
    kDiagParmSet = 0x24, // 36
    kDiagNvRead = 0x26, // 38
    kDiagNvWrite = 0x27, // 39
    kDiagControl = 0x29, // 41
    kDiagErrRead = 0x2a, // 42
    kDiagErrClear = 0x2b, // 43
    kDiagSerReset = 0x2c, // 44
    kDiagSerReport = 0x2d, // 45
    kDiagTest = 0x2e, // 46
    kDiagGetDipsw = 0x2f, // 47
    kDiagSetDipsw = 0x30, // 48
    kDiagVocPcmLb = 0x31, // 49
    kDiagVocPktLb = 0x32, // 50
    kDiagOrig = 0x35, // 53
    kDiagEnd = 0x36, // 54
    kDiagDload = 0x3a, // 58
    kDiagtmCmd = 0x3b, // 59
    kDiagTestState = 0x3d, // 61
    kDiagState = 0x3f, // 63
    kDiagPilotSets = 0x40, // 64
    kDiagSpc = 0x41, // 65
    kDiagBadSpcMode = 0x42, // 66
    kDiagParmGet2 = 0x43, // 67
    kDiagSerialChg = 0x44, // 68
    kDiagPassword = 0x46, // 70
    kDiagBadSecMode = 0x47, // 71
    kDiagPrListWr = 0x48, // 72
    kDiagPrListRd = 0x49, // 73
    kDiagSubsysCmd = 0x4b, // 75
    kDiageatureQuery = 0x51, // 81
    kDiagSmsRead = 0x53, // 83
    kDiagSmsWrite = 0x54, // 84
    kDiagSuper = 0x55, // 85
    kDiagSupWalshCodes = 0x56, // 86
    kDiagSetMaxSupCh = 0x57, // 87
    kDiagParmGetIs95b = 0x58, // 88
    kDiagsOp = 0x59, // 89
    kDiagAkeyVerify = 0x5a, // 90
    kDiagBmpHsScreen = 0x5b, // 91
    kDiagConfigComm = 0x5c, // 92
    kDiagExtLogmask = 0x5d, // 93
    kDiagEventReport = 0x60, // 96
    kDiagStreamingConfig = 0x61, // 97
    kDiagParmRetrieve = 0x62, // 98
    kDiagStatusSnapshot = 0x63, // 99
    kDiagRpc = 0x64, // 100
    kDiagGetProperty = 0x65, // 101
    kDiagPutProperty = 0x66, // 102
    kDiagGetGuid = 0x67, // 103
    kDiagUserCmd = 0x68, // 104
    kDiagGetPermProperty = 0x69, // 105
    kDiagPutPermProperty = 0x6a, // 106
    kDiagPermUserCmd = 0x6b, // 107
    kDiagGpsSessCtrl = 0x6c, // 108
    kDiagGpsGrid = 0x6d, // 109
    kDiagGpsStatistics = 0x6e, // 110
    kDiagRoute = 0x6f, // 111
    kDiagIs2000Status = 0x70, // 112
    kDiagRlpStatReset = 0x71, // 113
    kDiagTdsoStatReset = 0x72, // 114
    kDiagLogConfig = 0x73, // 115
    kDiagTraceEventReport = 0x74, // 116
    kDiagSbiRead = 0x75, // 117
    kDiagSbiWrite = 0x76, // 118
    kDiagSsdVerify = 0x77, // 119
    kDiagLogOnDemand = 0x78, // 120
    kDiagExtMsg = 0x79, // 121
    kDiagOncrpc = 0x7a, // 122
    kDiagProtocolLoopback = 0x7b, // 123
    kDiagExtBuildId = 0x7c, // 124
    kDiagExtMsgConfig = 0x7d, // 125
    kDiagExtMsgTerse = 0x7e, // 126
    kDiagExtMsgTerseXlate = 0x7f, // 127
    kDiagSubsysCmdVer2 = 0x80, // 128
    kDiagEventMaskGet = 0x81, // 129
    kDiagEventMaskSet = 0x82, // 130
    kDiagChangePortSettings = 0x8c, // 140
    kDiagCntryInfo = 0x8d, // 141
    kDiagSupsReq = 0x8e, // 142
    kDiagMmsOrigSmsRequest = 0x8f, // 143
    kDiagMeasMode = 0x90, // 144
    kDiagMeasReq = 0x91, // 145
    kDiagMax = 0x92, // 146
};


enum DiagSubsysCmD {
    kDiagSubsysOem = 0,       /* Reserved for OEM use */
    kDiagSubsysZrex = 1,       /* ZREX */
    kDiagSubsysSd = 2,       /* System Determination */
    kDiagSubsysBt = 3,       /* Bluetooth */
    kDiagSubsysWcdma = 4,       /* WCDMA */
    kDiagSubsysHdr = 5,       /* 1xEvDO */
    kDiagSubsysDiablo = 6,       /* DIABLO */
    kDiagSubsysTrex = 7,       /* TREX - Off-target testing environments */
    kDiagSubsysGsm = 8,       /* GSM */
    kDiagSubsysUmts = 9,       /* UMTS */
    kDiagSubsysHwtc = 10,      /* HWTC */
    kDiagSubsystm = 11,      /* Factory Test Mode */
    kDiagSubsysRex = 12,      /* Rex */
    kDiagSubsysOs = kDiagSubsysRex,
    kDiagSubsysGps = 13,      /* Global Positioning System */
    kDiagSubsysWms = 14,      /* Wireless Messaging Service (WMS, SMS) */
    kDiagSubsysCm = 15,      /* Call Manager */
    kDiagSubsysHs = 16,      /* Handset */
    kDiagSubsysAudioSettings = 17,      /* Audio Settings */
    kDiagSubsysDiagServ = 18,      /* DIAG Services */
    kDiagSubsyss = 19,      /* File System - EFS2 */
    kDiagSubsysPortMapSettings = 20,      /* Port Map Settings */
    kDiagSubsysMediaplayer = 21,      /* QCT Mediaplayer */
    kDiagSubsysQcamera = 22,      /* QCT QCamera */
    kDiagSubsysMobimon = 23,      /* QCT MobiMon */
    kDiagSubsysGunimon = 24,      /* QCT GuniMon */
    kDiagSubsysLsm = 25,      /* Location Services Manager */
    kDiagSubsysQcamcorder = 26,      /* QCT QCamcorder */
    kDiagSubsysMux1x = 27,      /* Multiplexer */
    kDiagSubsysData1x = 28,      /* Data */
    kDiagSubsysSrch1x = 29,      /* Searcher */
    kDiagSubsysCallp1x = 30,      /* Call Processor */
    kDiagSubsysApps = 31,      /* Applications */
    kDiagSubsysSettings = 32,      /* Settings */
    kDiagSubsysGsdi = 33,      /* Generic SIM Driver Interface */
    kDiagSubsysUimdiag = kDiagSubsysGsdi,
    kDiagSubsysTmc = 34,      /* Task Main Controller */
    kDiagSubsysUsb = 35,      /* Universal Serial Bus */
    kDiagSubsysPm = 36,      /* Power Management */
    kDiagSubsysDebug = 37,
    kDiagSubsysQtv = 38,
    kDiagSubsysClkrgm = 39,      /* Clock Regime */
    kDiagSubsysDevices = 40,
    kDiagSubsysWlan = 41,      /* 802.11 Technology */
    kDiagSubsysPsDataLogging = 42,      /* Data Path Logging */
    kDiagSubsysPs = kDiagSubsysPsDataLogging,
    kDiagSubsysMflo = 43,      /* MediaFLO */
    kDiagSubsysDtv = 44,      /* Digital TV */
    kDiagSubsysRrc = 45,      /* WCDMA Radio Resource Control state */
    kDiagSubsysProf = 46,      /* Miscellaneous Profiling Related */
    kDiagSubsysTcxomgr = 47,
    kDiagSubsysNv = 48,      /* Non Volatile Memory */
    kDiagSubsysAutoconfig = 49,
    kDiagSubsysParams = 50,      /* Parameters required for debugging subsystems */
    kDiagSubsysMddi = 51,      /* Mobile Display Digital Interface */
    kDiagSubsysDsAtcop = 52,
    kDiagSubsysL4linux = 53,      /* L4/Linux */
    kDiagSubsysMvs = 54,      /* Multimode Voice Services */
    kDiagSubsysCnv = 55,      /* Compact NV */
    kDiagSubsysApioneProgram = 56,      /* apiOne */
    kDiagSubsysHit = 57,      /* Hardware Integration Test */
    kDiagSubsysDrm = 58,      /* Digital Rights Management */
    kDiagSubsysDm = 59,      /* Device Management */
    kDiagSubsysc = 60,      /* Flow Controller */
    kDiagSubsysMemory = 61,      /* Malloc Manager */
    kDiagSubsyssAlternate = 62,      /* Alternate File System */
    kDiagSubsysRegression = 63,      /* Regression Test Commands */
    kDiagSubsysSensors = 64,      /* The sensors subsystem */
    kDiagSubsyslute = 65,      /* FLUTE */
    kDiagSubsysAnalog = 66,      /* Analog die subsystem */
    kDiagSubsysApioneProgramModem = 67,    /* apiOne Program On Modem Processor */
    kDiagSubsysLte = 68,      /* LTE */
    kDiagSubsysBrew = 69,      /* BREW */
    kDiagSubsysPwrdb = 70,      /* Power Debug Tool */
    kDiagSubsysChord = 71,      /* Chaos Coordinator */
    kDiagSubsysSec = 72,      /* Security */
    kDiagSubsysTime = 73,      /* Time Services */
    kDiagSubsysQ6Core = 74,      /* Q6 core services */
    kDiagSubsysCorebsp = 75,      /* CoreBSP */
                                    /* Command code allocation:
                                    [0 - 2047]  - HWENGINES
                                    [2048 - 2147]   - MPROC
                                    [2148 - 2247]   - BUSES
                                    [2248 - 2347]   - USB
                                    [2348 - 2447]   - FLASH
                                    [2448 - 3447]   - UART
                                    [3448 - 3547]   - PRODUCTS
                                    [3547 - 65535]  - Reserved
                                    */

    kDiagSubsysMflo2 = 76,      /* Media Flow */
                                /* Command code allocation:
                                [0 - 1023]       - APPs
                                [1024 - 65535]   - Reserved
                                */
    kDiagSubsysUlog = 77,  /* ULog Services */
    kDiagSubsysApr = 78,  /* Asynchronous Packet Router (Yu, Andy)*/
    kDiagSubsysQnp = 79, /*QNP (Ravinder Are , Arun Harnoor)*/
    kDiagSubsysStride = 80, /* Ivailo Petrov */
    kDiagSubsysOemDPP = 81, /* to read/write calibration to DPP partition */
    kDiagSubsysQ5Core = 82, /* Requested by ADSP team */
    kDiagSubsysUscript = 83, /* core/power team USCRIPT tool */
    kDiagSubsysNas = 84, /* Requested by 3GPP NAS team */
    kDiagSubsysCmAPI = 85, /* Requested by CMAPI */
    kDiagSubsysSsm = 86,
    kDiagSubsysTdscdma = 87,  /* Requested by TDSCDMA team */
    kDiagSubsysSsmTest = 88,
    kDiagSubsysMpower = 89,  /* Requested by MPOWER team */
    kDiagSubsysQdss = 90,  /* For QDSS STM commands */
    kDiagSubsysCxm = 91,
    kDiagSubsysGnssSoc = 92,  /* Secondary GNSS system */
    kDiagSubsysTtlite = 93,
    kDiagSubsystmAnt = 94,
    kDiagSubsysMlog = 95,
    kDiagSubsysLimitsmgr = 96,
    kDiagSubsysEfsmonitor = 97,
    kDiagSubsysDisplayCalibration = 98,
    kDiagSubsysVersionReport = 99,
    kDiagSubsysDsIpa = 100,
    kDiagSubsysSystemOperations = 101,
    kDiagSubsysCnssPower = 102,
    kDiagSubsysLast,

    /* Subsystem IDs reserved for OEM use */
    kDiagSubsysReservedOem0 = 250,
    kDiagSubsysReservedOem1 = 251,
    kDiagSubsysReservedOem2 = 252,
    kDiagSubsysReservedOem3 = 253,
    kDiagSubsysReservedOem4 = 254,
    kDiagSubsysLegacy = 255
};


enum QcdmPhoneMode : uint8_t {
	kQcdmPhoneModeOfflineA  = 0x00,
	kQcdmPhoneModeOfflineD  = 0x01,
	kQcdmPhoneModeReset		= 0x02,
	kQcdmPhoneModeFtm		= 0x03,
	kQcdmPhoneModeOnline	= 0x04,
	kQcdmPhoneModeLpm		= 0x05,
	kQcdmPhoneModePowerOff  = 0x06
};

enum QcdmRuimConfigType : uint8_t {
	kQcdmRuimConfigTypeRuim  = 0x00,
	kQcdmRuimConfigTypeNv	 = 0x01,
	kQcdmRuimConfigTypePref  = 0x02,
	kQcdmRuimConfigTypeGsm1x = 0x03
};


PACKED(typedef struct QcdmRequestHeader{
    uint8_t command;
}) QcdmRequestHeader;

PACKED(typedef struct QcdmResponseHeader{
	uint8_t command;
}) QcdmResponseHeader;

typedef QcdmRequestHeader QcdmGenericRequest;

PACKED(typedef struct QcdmPhoneModeRequest{
    uint8_t command;
    uint8_t mode;
    uint8_t padding;
}) QcdmPhoneModeRequest;

/**
* Phone Mode
*/
PACKED(typedef struct QcdmPhoneModeResponse{
    uint8_t command;
    uint8_t status;
}) QcdmPhoneModeResponse;

/**
* SPC
*/
PACKED(typedef struct QcdmSpcRequest{
    uint8_t command;
    uint8_t spc[6];
}) QcdmSpcRequest;

PACKED(typedef struct QcdmSpcResponse{
    uint8_t command;
    uint8_t status;
}) QcdmSpcResponse;

/**
* Password
*/
PACKED(typedef struct QcdmPasswordRequest{
    uint8_t command = kDiagPassword;
    uint8_t password[8];
}) QcdmPasswordRequest;

PACKED(typedef struct QcdmPasswordResponse{
    uint8_t command;
    uint8_t status;
}) QcdmPasswordResponse;

/**
* NV
*/
PACKED(typedef struct QcdmNvRequest{
    uint8_t command;
    uint16_t nvItem;
    uint8_t data[DIAG_NV_ITEM_SIZE];
}) QcdmNvRequest;

PACKED(typedef struct QcdmNvResponse {
	uint8_t		command;
    uint16_t	nvItem;
    uint8_t		data[DIAG_NV_ITEM_SIZE];
}) QcdmNvResponse;

PACKED(typedef struct QcdmNvPeekRequest{
	uint8_t command;
	uint32_t address;
	uint8_t size; // max DIAG_NV_PEEK_MAX_SIZE bytes
}) QcdmNvPeekRequest;

PACKED(typedef struct QcdmNvPeekResponse {
	uint8_t		command;
	uint8_t		data[DIAG_NV_PEEK_MAX_SIZE];
}) QcdmNvPeekResponse;

/**
* Subsystem General 
*/
PACKED(typedef struct QcdmSubsysHeader{
    uint8_t command;
    uint8_t  subsysId;
    uint16_t subsysCommand;
}) QcdmSubsysHeader;

PACKED(typedef struct QcdmSubsysRequest{
    QcdmSubsysHeader header;
    uint8_t  data[1]; // varies
}) QcdmSubsysRequest;


/**
* Feature Query
*/
PACKED(typedef struct QcdmQueryFeaturesRequest{
    uint8_t command;
    uint16_t featureMaskSize;
    uint8_t  featureMask[1]; // varies
}) QcdmQueryFeaturesRequest;

PACKED(typedef struct QcdmLogResponse{
    uint8_t command;
    uint8_t entries;
    uint16_t  length;
    uint8_t  logs[1]; // varies
}) QcdmLogResponse;

/**
* Version
*/
PACKED(typedef struct QcdmVersionResponse{
    uint8_t command;
	char  cdate[11];
	char  ctime[8];
	char  rdate[11];
	char  rtime[8];
	char  ver_dir[8];
	uint8_t scm;
	uint8_t caiRev;
	uint8_t model;
	uint8_t firmwareRev;
	uint8_t slotCycleIndex;
	uint8_t hwVersionMajor;
	uint8_t hwVersionMinor;
}) QcdmVersionResponse;

PACKED(typedef struct QcdmDiagVersionResponse{
	uint8_t command;
	uint16_t version;
}) QcdmDiagVersionResponse;

/**
* Status
*/
PACKED(typedef struct QcdmStatusResponse{
	uint8_t command;
	uint8_t demod;
	uint8_t decode;
	uint8_t interleaverId;
	uint32_t esn;
	uint16_t rmMode;
	uint32_t min1[2];
	uint32_t min2[2];
	uint8_t minIndex;
	uint16_t cdmaRxState;
	uint8_t cdmaGoodFrames;
	uint16_t analogCorrectedFrames;
	uint16_t analogBadFrames;
	uint16_t  analogWordSyncs;
	uint16_t entryReason;
	uint16_t currentChannel;
	uint8_t cdmaCodeChan;
	uint16_t pilotBase;
	uint16_t sid;
	uint16_t nid;
	uint16_t locationId;
	uint16_t rssiLevel;
	uint8_t power;
}) QcdmStatusResponse;

/**
* GUID
*/
PACKED(typedef struct QcdmGuidResponse{
	uint8_t command;
	uint32_t guid[4];
}) QcdmGuidResponse;

/**
* Subsys: kDiagSubsysDiagServ
* QPST Cookie Structures
*/
PACKED(typedef struct QcdmSaveQpstCookieRequest {
    QcdmSubsysHeader header;
    uint8_t version;
    uint8_t cookieLength;
    uint8_t cookie[1];
}) QcdmSaveQpstCookieRequest;

PACKED(typedef struct QcdmSaveQpstCookieResponse {
    QcdmSubsysHeader header;
    uint8_t version;
    uint8_t error;
}) QcdmSaveQpstCookieResponse;

PACKED(typedef struct QcdmQueryQpstCookieRequest {
    QcdmSubsysHeader header;
    uint8_t version;
}) QcdmQueryQpstCookieRequest;

PACKED(typedef struct QcdmQueryQpstCookieResponse {
    QcdmSubsysHeader header;
    uint8_t version;
    uint8_t error;
    uint8_t cookieLength;
    uint8_t cookie[1];
}) QcdmQueryQpstCookieResponse;


PACKED(typedef struct QcdmTestStruct {
    QcdmSubsysHeader header;
    uint8_t processorId;
}) QcdmTestStruct;

#endif // _QC_DIAG_H
