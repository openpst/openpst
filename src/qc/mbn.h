/**
* LICENSE PLACEHOLDER
*
* @file mbn.h
* @package OpenPST
* @brief MBN file format definitions and structures
*
* @author Gassan Idriss <ghassani@gmail.com>
*/

#ifndef _QC_MBN_H
#define _QC_MBN_H

#include "include/definitions.h"

#define MBN_HEADER_MAX_SIZE 80
#define MBN_HEADER_MIN_SIZE 40
#define MBN_EIGHTY_BYTE_MAGIC 0x73D71034
#define MBN_ROOT_CERTIFICATE_SIZE 1024 * 6
#define MBN_MAX_ROOT_CERTIFICATES 4

enum MBN_IMAGE_SEGMENTS {
  kMbnSegmentHeader = 1,
  kMbnSegmentCode,
  kMbnSegmentSignature,
  kMbnSegmentX509Chain
};

PACKED(typedef struct {
    uint32_t  codeword;            /* Codeword/magic number defining flash type
                                information. */
    uint32_t  magic;               /* Magic number */
	uint32_t  image_id;        /* image content */
    uint32_t  reserved1;          /* RESERVED */
    uint32_t  reserved2;          /* RESERVED */
    uint32_t  image_src;             /* Location of RPM_SBL in flash or e-hostdl in RAM. This is given in
                                byte offset from beginning of flash/RAM.  */
    uint32_t image_dest_ptr;        /* Pointer to location to store RPM_SBL/e-hostdl in RAM.
                                Also, entry point at which execution begins.
                                */
    uint32_t  image_size;      /* Size of RPM_SBL image in bytes */
    uint32_t  code_size;       /* Size of code region in RPM_SBL image in bytes */
    uint32_t signature_ptr;         /* Pointer to images attestation signature */
    uint32_t  signature_size;        /* Size of the attestation signature in
                                bytes */
    uint32_t cert_chain_ptr;  /* Pointer to the chain of attestation
                                certificates associated with the image. */
    uint32_t  cert_chain_size; /* Size of the attestation chain in bytes */

    uint32_t  oem_root_cert_sel;  /* Root certificate to use for authentication.
                                Only used if SECURE_BOOT1 table_sel fuse is
                                OEM_TABLE. 1 indicates the first root
                                certificate in the chain should be used, etc */
    uint32_t  oem_num_root_certs; /* Number of root certificates in image.
                                Only used if SECURE_BOOT1 table_sel fuse is
                                OEM_TABLE. Denotes the number of certificates
                                OEM has provisioned                          */

    uint32_t  reserved3;          /* RESERVED */
    uint32_t  reserved4;          /* RESERVED */
    uint32_t  reserved5;          /* RESERVED */
    uint32_t  reserved6;          /* RESERVED */
    uint32_t  reserved7;          /* RESERVED */
}) MbnSblHeader;

PACKED(typedef struct {
  uint32_t image_id;           /**< Identifies the type of image this header
                                  represents (OEM SBL, AMSS, Apps boot loader,
                                  etc.). */
  uint32_t header_vsn_num;     /**< Header version number. */
  uint32_t image_src;          /**< Offset from end of the Boot header where the
                                  image starts. */
  uint32_t image_dest_ptr;     /**< Pointer to location to store image in RAM.
                                  Also, ent676ry point at which image execution
                                  begins. */
  uint32_t image_size;         /**< Size of complete image in bytes */
  uint32_t code_size;          /**< Size of code region of image in bytes */
  uint32_t signature_ptr;      /**< Pointer to images attestation signature */
  uint32_t signature_size;     /**< Size of the attestation signature in
                                 bytes */
  uint32_t cert_chain_ptr;     /**< Pointer to the chain of attestation
                                 certificates associated with the image. */
  uint32_t cert_chain_size;    /**< Size of the attestation chain in bytes */
}) MbnHeader;

enum MbnImageId {
    kMbnImageNone           = 0x00,
    kMbnImageOemSbl    = 0x01,
    kMbnImageAmss       = 0x02,
    kMbnImageOcbl      = 0x03,
    kMbnImageHash       = 0x04,
    kMbnImageAppbl     = 0x05,
    kMbnImageApps           = 0x06,
    kMbnImageHostDl         = 0x07,
    kMbnImageDsp1           = 0x08,
    kMbnImageFsbl           = 0x09,
    kMbnImageDbl            = 0x0A,
    kMbnImageOsbl           = 0x0B,
    kMbnImageDsp2           = 0x0C,
    kMbnImageEhostdl        = 0x0D,
    kMbnImageNandprg        = 0x0E,
    kMbnImageNorprg         = 0x0F,
    kMbnImageRamfs1         = 0x10,
    kMbnImageRamfs2         = 0x11,
    kMbnImageAdspQ5        = 0x12,
    kMbnImageAppsKernel    = 0x13,
    kMbnImageBackupRamfs   = 0x14,
    kMbnImageSbl1           = 0x15,
    kMbnImageSbl2           = 0x16,
    kMbnImageRpm            = 0x17,
    kMbnImageSbl3           = 0x18,
    kMbnImageTz             = 0x19,
    kMbnImageSsdKeys       = 0x1A,
    kMbnImageGen            = 0x1B,
    kMbnImageDsp3           = 0x1C,
    kMbnImageAcdb           = 0x1D,
    kMbnImageWdt            = 0x1E,
    kMbnImageMba            = 0x1F
};

#endif // _QC_MBN_H
