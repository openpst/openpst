/**
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
*/

#include <iostream>
#include <string>
#include <stdio.h>
#include "include/definitions.h"
#include "util/sleep.h"
#include "serial/qcdm_serial.h"
#include "qc/dm.h"
#include "qc/dm_efs_manager.h"

using namespace std;
using namespace serial;
using namespace OpenPST;

QcdmSerial port("", 115200, serial::Timeout::simpleTimeout(500));
DmEfsManager efsManager(port);

/**
* @brief usage - Show program usage information
* @return void
*/
void usage() {
    printf("---------------------------------------------------\n");
    printf(" Samsung SIM Unlock for Qualcomm SOC:              \n");
    printf("---------------------------------------------------\n");
    printf(" Usage:\n");
    printf("\tqcsamunlock [PORT]\n");
    printf("\tqcsamunlock\\\\.\\COM10\n");
    printf("\tqcsamunlock /dev/ttyUSB0\n");
    printf("---------------------------------------------------\n");
    printf(" Copyright 2015 Gassan Idriss <ghassani@gmail.com> \n");
    printf(" This software is distributed with OpenPST.        \n");
    printf(" See http://www.github.com/ghassani/openpst        \n");    
    printf(" This software is free and licensed under GPL.     \n");
    printf("---------------------------------------------------\n");
}

/**
* @brief processItem - Delete an NV item by deltree traversal exploit
* @param int item - The NV item number to delete 
* @return bool
*/
bool processItem(int item, int sequence)
{
    ostringstream path;     
    path << "/public/../nvm/num/" << item; // deltree only works in /public

    if (efsManager.mkdir("public", 0x01FF) == efsManager.kDmEfsSuccess) {
        if (efsManager.deltree(path.str(), sequence) == efsManager.kDmEfsSuccess) {
            return true;
        } else {            
            return false;
        }
    }

    return false;
}

/**
* @brief main - Program entry point
* @param int argc
* @param char **argv
* @return int
*/
int main(int argc, char **argv) {
    QcdmEfsStatfsResponse        statResponse;
    QcdmEfsSyncResponse          syncResponse;
    QcdmEfsGetSyncStatusResponse syncStatusResponse;
    int syncMaxRetries           = 10;
    int syncRetries              = 0;
    int sequence                 = 0;

    if (argc < 2) {
        usage();
        return 0;
    }

    try {
        
        port.setPort(argv[1]);
        port.open();

    } catch (serial::IOException e) {        
        printf("[-] Error connecting to device %s. Error: %s\n", argv[1], e.what());
        goto exit;
    }   

    if (efsManager.statfs("/", statResponse) == efsManager.kDmEfsSuccess) {
        
        if (!processItem(10080, ++sequence)) {
            printf("[-] Error removing nv item 10080\n");
        }

        if (!processItem(10074, ++sequence)) {
             printf("[-] Error removing nv item 10074\n");
        }

        if (!processItem(10073, ++sequence)) {
            printf("[-] Error removing nv item 10073\n");
        }


    } else {
        printf("[-] Error checking for EFS access\n");
        goto exit;
    }

    try {
        
        syncResponse = efsManager.syncNoWait("/");

        printf("[+] Sync initiated. Received token %08X\n", syncResponse.token);

    } catch (std::exception e) {
        printf("[-] Error encountered initiating filysystem sync: Error: %s\n", e.what());
        goto exit;
    }

    try {
        
        sequence = 0;

        while (syncMaxRetries > syncRetries) {
            
            syncStatusResponse = efsManager.getSyncStatus("/", syncResponse.token, ++sequence);

            if (syncStatusResponse.status) {
                printf("[+] Sync Complete\n");
                printf("[+] Operation Successful. Reboot device and insert a different carriers SIM.\n");
                goto exit;
            } else {
                sleep(1000); // wait and check again
                syncRetries++;
            }
        }

        printf("[-] Sync Check Error. Device may still have been unlocked. Reboot and insert a different carriers SIM.\n");

    } catch (std::exception e) {
        printf("[-] Error encountered during sync check: %s\n", e.what());
        goto exit;
    }

  exit:
    port.close();
    return 0;
}
