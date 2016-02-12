### Memory Debug

Memory debug mode allows you to read out from the memory on a boot failure. 
Memory reading is supported for 32bit as well as 64bit.
Currently OpenPST supports only 32 bit.

When recieving the hello and responding back, the device will then send a memory address of n bytes to read. This location contains a map to various segments of memory.Once read, OpenPST will ask you if you would like to dump each reference. If you choose to dump each reference you will be prompted to select a location. Files larger than 1MB will be confirmed to transfer as it can take some time for large segments. In addition, OpenPST will offer you a chance to dump the raw memory table to the filesystem.

When in this mode, OpenPST can read any size chunk of memory space to your local filesystem. 

Memory debug is known to be available on the following USB interfaces:
- LGE AndroidNet USB Serial Port. This USB interface has a vendor ID of 1004 (LG) and product id 61A1.
- Qualcomm HS-USB Diagnostic 9006. This USB interface has a vendor ID of 05c6 and product id of 9006.

May be available in other diagnostic interfaces.

### Image Transfer

Image transfer is available when the device requires firmware to be uploaded or the device is in an emergency mode.

Sahara protocol will tell you what file it wants in this mode. You can reference the definitions in qc/mbn.h for a list of values.

In emergency mode it will only request the EHOSTDL file which will be a signed (or unsigned if not using secure boot) image to re-program the flash. Some common protocols used by flash programmers are Streaming DLOAD, DMSS DLOAD, as well as Firehose.

Available on the following known USB interfaces:

- Qualcomm HS-USB QDLoader 9008. This USB interface has a vendor ID of 05c6 (Qualcomm) and product id of 9006.

### Known Linux Problems

- Gobi firmware loader will interfere with establishing a hello handshake
