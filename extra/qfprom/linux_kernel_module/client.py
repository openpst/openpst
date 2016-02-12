#!/usr/bin/python
'''
*  Copyright (C) Gassan Idriss <ghassani@gmail.com>
*
* This program is free software ; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation ; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY ; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with the program ; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*
* @file client.py
* @package OpenPST Extras - QFPROM Kernel Module
* @brief This client can be used with the qfprom kernel module when started with start_tcp=PORT
* @usage
*   read addresses:
*       python client.py [HOST] [PORT] -r 0x00000000 0x00000001 0x00000002
*   read corrected addresses:
*       python client.py [HOST] [PORT] -rc 0x00000000 0x00000001 0x00000002
* @author Gassan Idriss <ghassani@gmail.com>
'''
import sys
import argparse
import socket
import ctypes
import pprint
import time
import binascii
import re
import struct
import csv

def read_row(sock, addr, corrected=False):    
    request = struct.pack('<BLL', 0x01, addr, 0x00000001 if corrected else 0x00000000)
    #print 'Request   :', binascii.hexlify(request)
    written = sock.send(request)
    #print "Sent %d bytes\n" % written 
    response = sock.recv(1024);
    #print 'Response   :', binascii.hexlify(response)
    command, address, read_type, lsb, msb, error = struct.unpack('<BLLLLL', response)
    
    return {'address': address, 'lsb': lsb, 'msb' : msb, 'error' : error}

def read_many(sock, addresses, corrected=False):
    try:

        for addr in addresses:
            address = int(addr, 16)
            result = read_row(sock, address, False)
            if result['error'] > 0:
                print "Error reading 0x%08X - %d" % (result['address'], result['error'] )
            else:
                print "Row data for 0x%08X - LSB: 0x%08X MSB: 0x%08X" % ( result['address'], result['lsb'], result['msb'] )
    except TypeError, e:
        pass
    except NameError, e:
        pass
    except Exception, e:
        print e

def write_row(sock, addr, lsb, msb, bus_clk_khz):    
    request = struct.pack('<BLLLL', 0x01, addr, bus_clk_khz, lsb, msb)
    #print 'Request   :', binascii.hexlify(request)
    written = sock.send(request)
    #print "Sent %d bytes\n" % written 
    response = sock.recv(1024);
    #print 'Response   :', binascii.hexlify(response)
    command, address, read_type, lsb, msb, error = struct.unpack('<BLLLLL', response)
    
    return {'address': address, 'lsb': lsb, 'msb' : msb, 'error' : error}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('host', metavar='host', help='the host to connect to')
    parser.add_argument('port', metavar='port', type=int, help='the port to connect on')
    parser.add_argument('-r',   metavar='--read',   nargs='*',   help='Read specified address(es)')
    parser.add_argument('-rc',  metavar='--read-corrected',   nargs='*',   help='Read specified corrected address(es)')
    #parser.add_argument('-rr',  metavar='--read-range',   nargs='*',   help='Read specified range of addresses')
    #parser.add_argument('-rcr',  metavar='--read-corrected-range',   nargs='*',   help='Read specified range of corrected addresses')
    args = parser.parse_args()
    
    print "Trying to connect to %s on port %d" % (args.host, args.port)

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    try:
        sock.connect((args.host, args.port))
    except Exception, e:
        print "Error connecting to %s on port %d" % (args.host, args.port)
        return

    print "Connected"
  
    read_many(sock, args.r, False)
    read_many(sock, args.rc, True)
    
    print "Disconnecting"

    sock.send(struct.pack('B', 0x00))


if __name__ == '__main__':    
    main()

