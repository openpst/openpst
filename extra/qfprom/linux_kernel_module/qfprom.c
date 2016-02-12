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
*
* @file qfprom.c
* @package OpenPST Extras - QFPROM Kernel Module
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include "qfprom.h"

int tz_qfprom_read_row(uint32_t address, uint32_t type, scm_qfprom_read_row_data_t* row_data)
{
    int ret = 0;
    scm_qfprom_read_row_req_t req           = {};
    scm_qfprom_read_row_resp_t resp         = {};
    struct ion_client *ion_clientp          = NULL;
    struct ion_handle *ion_row_data_handlep = NULL;
    uint32_t row_data_phyaddr_len           = 0;
    ion_phys_addr_t row_data_phyaddr        = 0;
    scm_qfprom_read_row_data_t* row_datap;

    ion_clientp = msm_ion_client_create(UINT_MAX, "tz_qfprom_read_row");

    if (IS_ERR_OR_NULL(ion_clientp)) {      
        log("Error creating ion client\n");
        return -EINVAL;
    }

    ion_row_data_handlep = ion_alloc(ion_clientp, sizeof(scm_qfprom_read_row_data_t), 
        sizeof(scm_qfprom_read_row_data_t), ION_HEAP(ION_QSECOM_HEAP_ID), 0);


    if (IS_ERR_OR_NULL(ion_row_data_handlep)) {
        log("Ion client could not retrieve the handle\n");
        goto error;
    }

    if (ion_phys(ion_clientp, ion_row_data_handlep, &row_data_phyaddr, &row_data_phyaddr_len)) {
        log("Ion conversion to physical address failed\n");
        goto error;
    }

    req.address     = address;
    req.type        = type;
    req.row_data    = row_data_phyaddr;
    req.error       = row_data_phyaddr + (sizeof(scm_qfprom_read_row_data_t) - sizeof(uint32_t));

    ret = scm_call(TZ_SVC_FUSE, TZ_QFPROM_READ_ROW_ID, &req, sizeof(req), &resp, sizeof(resp));

    if (ret) {
        
        log("Reading row at 0x%08X (%d) failed. SCM call failed. %d\n", address, type, ret);
        
        row_data->lsb   = 0;
        row_data->msb   = 0;    
        row_data->error = ret;  

    } else {

        row_datap = (scm_qfprom_read_row_data_t*)ion_map_kernel(ion_clientp, ion_row_data_handlep);
        
        if (IS_ERR_OR_NULL(row_datap)) {
            log("Ion memory mapping failed\n");
            goto error;
        }

        if (row_datap->error) {             

            log("Error reading row at 0x%08X (%d). Error: %d\n", address, type, row_datap->error);

        } else {
            log("Row data at 0x%08X (%d) - LSB: 0x%08X - MSB: 0x%08X\n", address, type, row_datap->lsb, row_datap->msb);
        }

        row_data->lsb   = row_datap->lsb;
        row_data->msb   = row_datap->msb;   
        row_data->error = row_datap->error; 
    }

    ion_free(ion_clientp, ion_row_data_handlep);
    ion_client_destroy(ion_clientp);    
    return ret; 
error:
    ion_free(ion_clientp, ion_row_data_handlep);
    ion_client_destroy(ion_clientp);
    return -EINVAL;
}


int tz_qfprom_write_row(uint32_t address, uint32_t lsb, uint32_t msb, uint32_t bus_clk_khz, scm_qfprom_write_row_data_t* row_data)
{
    int ret = 0;
    scm_qfprom_write_row_req_t req          = {};
    scm_qfprom_write_row_resp_t resp        = {};
    struct ion_client *ion_clientp          = NULL;
    struct ion_handle *ion_row_data_handlep = NULL;
    uint32_t row_data_phyaddr_len           = 0;
    ion_phys_addr_t row_data_phyaddr        = 0;
    scm_qfprom_write_row_data_t* row_datap;

    ion_clientp = msm_ion_client_create(UINT_MAX, "tz_qfprom_write_row");

    if (IS_ERR_OR_NULL(ion_clientp)) {      
        log("Error creating ion client\n");
        return -EINVAL;
    }

    ion_row_data_handlep = ion_alloc(ion_clientp, sizeof(scm_qfprom_write_row_data_t), 
        sizeof(scm_qfprom_write_row_data_t), ION_HEAP(ION_QSECOM_HEAP_ID), 0);


    if (IS_ERR_OR_NULL(ion_row_data_handlep)) {
        log("Ion client could not retrieve the handle\n");
        goto error;
    }

    if (ion_phys(ion_clientp, ion_row_data_handlep, &row_data_phyaddr, &row_data_phyaddr_len)) {
        log("Ion conversion to physical address failed\n");
        goto error;
    }
    
    row_datap = (scm_qfprom_write_row_data_t*)ion_map_kernel(ion_clientp, ion_row_data_handlep);
    
    if (IS_ERR_OR_NULL(row_datap)) {
        log("Ion memory mapping failed\n");
        goto error;
    }

    row_datap->lsb = lsb;
    row_datap->msb = msb;

    req.address     = address;
    req.bus_clk_khz = bus_clk_khz;
    req.row_data    = row_data_phyaddr;
    req.error       = row_data_phyaddr + (sizeof(scm_qfprom_write_row_data_t) - sizeof(uint32_t));

    //ret = scm_call(TZ_SVC_FUSE, TZ_QFPROM_ROLLBACK_WRITE_ROW_ID, &req, sizeof(req), &resp, sizeof(resp));
    ret = scm_call(TZ_SVC_FUSE, TZ_QFPROM_WRITE_ROW_ID, &req, sizeof(req), &resp, sizeof(resp));

    if (ret) {
        
        log("Writing row at 0x%08X faile. SCM call failed. %d\n", address, ret);
        
        row_data->lsb   = 0;
        row_data->msb   = 0;    
        row_data->error = ret;  

    } else {

        if (row_datap->error) {             

            log("Error writing row at 0x%08X. Error: %d\n", address, row_datap->error);

        } else {
            log("Row data at 0x%08X - LSB: 0x%08X - MSB: 0x%08X\n", address, row_datap->lsb, row_datap->msb);
        }

        row_data->lsb   = row_datap->lsb;
        row_data->msb   = row_datap->msb;   
        row_data->error = row_datap->error; 
    }

    ion_free(ion_clientp, ion_row_data_handlep);
    ion_client_destroy(ion_clientp);    
    return ret; 
error:
    ion_free(ion_clientp, ion_row_data_handlep);
    ion_client_destroy(ion_clientp);
    return -EINVAL;
}


int qfprom_read_direct(uint32_t address, scm_qfprom_read_row_data_t* row_data)
{

    uint32_t direct_read_lsb;
    uint32_t direct_read_msb;
    void __iomem *direct_read_lsb_address;
    void __iomem *direct_read_msb_address;
    
    direct_read_lsb_address = ioremap(address,  sizeof(uint32_t));
    direct_read_msb_address = ioremap(address + sizeof(uint32_t), sizeof(uint32_t));

    direct_read_lsb = (uint32_t) readl(direct_read_lsb_address);
    direct_read_msb = (uint32_t) readl(direct_read_msb_address);

    iounmap(direct_read_lsb_address);
    iounmap(direct_read_msb_address);

    log("Direct Read: 0x%08X - LSB: 0x%08X - MSB: 0x%08X\n", 
        address, direct_read_lsb, direct_read_msb
    );
 
    row_data->lsb = direct_read_lsb;
    row_data->msb = direct_read_msb;
    row_data->error = 0;

    return 0;
}

int qfprom_write_direct(uint32_t address, uint32_t lsb, uint32_t msb, uint32_t bus_clk_khz, scm_qfprom_write_row_data_t* row_data)
{
    /*
    uint32_t blow_status = 0;
    uint32_t read_lsb;
    uint32_t read_msb;
    uint32_t timeout;
    int err;

    // Set QFPROM  blow timer register 
    writel_relaxed(QFPROM_BLOW_TIMER_VALUE, QFPROM_BLOW_TIMER_ADDR);
    mb();

    // Enable LVS0 regulator 
    err = regulator_enable(qfp_priv->fuse_vdd);
    
    if (err != 0){        
        pr_err("Error (%d) enabling regulator... you should probably care about this.\n", err);
    }
    
    msleep(20);

    // Write data 
    __raw_writel(lsb, &address);
    __raw_writel(msb, &address + sizeof(lsb));
    
    mb();

    // blow_status = QFPROM_BLOW_STATUS_BUSY; 
    timeout = QFPROM_FUSE_BLOW_TIME_IN_US;
    // wait for 400us before checking for the first time 
    udelay(400);
    do {
        *status = readl_relaxed(QFPROM_BLOW_STATUS_ADDR);

        if (!(*status & QFPROM_BLOW_STATUS_BUSY)) {
            return 0;
        }

        timeout--;
        udelay(1);
    } while (timeout);
    
    pr_err("Timeout waiting for FUSE blow, status = %x . This is probably important\n", *status);

    // Check error status 
    if (blow_status & QFPROM_BLOW_STATUS_ERROR) {
        pr_err("Fuse blow status error: %d. Bummer, I was hoping that would work\n", blow_status);
    }

    // Disable regulator 
    err = regulator_disable(qfp_priv->fuse_vdd);    

    if (err != 0) {
        pr_err("Error (%d) disabling regulator, I can't just turn it off??? WTF\n", err);
    }
    
    
    msleep(20);

    // Verify written data 
    read_lsb = readl_relaxed(address);
    read_msb = readl_relaxed(address + sizeof(lsb));

    if (read_lsb != lsb) {
        pr_err("Error: read/write data mismatch (LSB)\n");
        pr_err("Address = %p written data = %x read data = %x.\n", address, lsb, read_lsb);
    }


   if (read_msb != msb) {
        pr_err("Error: read/write data mismatch (MSB)\n");
        pr_err("Address = %p written data = %x read data = %x.\n", address, msb, read_msb);
    }
*/
    return 0;
 }
