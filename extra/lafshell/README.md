# LAF Shell

This application allows you access a root shell on LG devices in DLOAD with LAF partitions for device flashing. LAF stands for LG Advanced Flasher and is LG's proprietary device flashing implementation. Besides offering a EXEC command which allows you to execute privileged shell commands, it also allows for the general management of the devices partitions for firmware updates.

This program has been known to the LG community as `Send_Command.exe`. This implementation is a bit more complete than the original because it actually knows the correct structure of the commands as well well as generating correct CRC, which are the same as Qualcomm HDLC crc calculations.  When incorrect CRC calculations are passed, you get a command executed with no result. This is pretty annoying because it is always nice to see the command output. 
