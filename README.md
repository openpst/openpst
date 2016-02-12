Linux:
[![Build Status](https://travis-ci.org/openpst/openpst.svg?branch=master)](https://travis-ci.org/openpst/openpst)
Windows:
[![Build Status](https://ci.appveyor.com/api/projects/status/github/openpst/openpst)](https://ci.appveyor.com/project/openpst/openpst)

# OpenPST

OpenPST is currently in early development. While there is a lot of functionality, it is still a bit buggy. We welcome developers to help out in any way they can. Feel free to contact the lead developer at `ghassani@gmail.com`

###Goals
OpenPST aims to be a multi-platform phone servicing tool for Qualcomm based chipsets. It is comprised of multiple programs each fulfilling a different purpose. 

### Current Features

#### Sahara Protocol

Sahara protocol that allows for memory debugging, firmware transfer, and emergency device programming. Typically known to be Qualcomm USB Product ID 9006 and 9008. 

Currently supported features:

 - **Image Transfer** - When Sahara is in image transfer mode it expects to be fed an image. -This allows you to upload the requested image.
 - **Client Commands** - In some instances, Sahara allows for client command execution. There are a limited number of commands available. 
 - **Memory Debug/Read** - In a boot failure, a device may boot into Sahara for memory debug. OpenPST supports full reading of any allowed
   memory address when in this mode. In addition it can process all the
   device provided debug addresses and dump them to your local machine
   for inspection. Currently only 32bit devices are supported.

#### Streaming DLOAD Protocol

Streaming DLOAD is a flash recovery protocol. Generally, in emergency mode a device programmer file is required to be upload to reprogram the flash. This is one of the protocols used to restore the device.

	- General features
	- Read flash
	- Open image / read & write
	- Open multi / read & write

#### QCDM

Qualcomm Diagnostic Monitory protocol has been around for quite a while. You can get dignostic information from it obviously, but you can also program NV (Non Volatile) memory which allows for certain configurations of the device. Mostly useful for CDMA based devices but any Qualcomm device has this protocol available.

Currently Supported Features:

- General NAM programming for CDMA based devices
- NV Read/Write
- EFS Reading/Writing	
 
#### MBN Inspection
	- Tool to inspect MBN image headers
	- Display header information
	- Extract X509 Certificate, Signature, and Code segments
	
#### Extras

Also included are a couple extra stuffs that may be of interest. See the extra/ folder. Included extra's:

- **QFPROM Read/Write** - Read and write (at least with permission available) QFPROM rows from a kernel module. Also allows for reading/writing of rows from a TCP server.

- **LAF Shell** - On LG devices with LAF parition, boot into DLOAD to be able to execute privledged commands. Known to most as the `Send_Command.exe` binary passed around the net.

###Dependencies

 - [QT 5
 - .x](https://www.qt.io/download/)
 - [Serial library by wjwwood](http://wjwwood.github.com/serial/)

### Building

    git clone git://github.com/openpst/openpst.git --depth=1 --recursive

####Windows

 1. Install QT
 2. Install QT Visual Studio plugin
 3. Setup QT enviornment in Visual Studio
 4. Open `OpenPST-vs2013.sln` in root of project
 5. Build

#### Linux

 1. Install build essentials and QT libraries
 2. Run `make` from root of project

#### QT Creator

 1. Open OpenPST.pro in QT Creator. Build

