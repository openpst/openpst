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
* @file server.c
* @package OpenPST Extras - QFPROM Kernel Module
*
* @author Gassan Idriss <ghassani@gmail.com>
*/
#include "server.h"

#define BUFFER_SIZE 1024

static struct task_struct* thread = NULL;
static struct socket* socket;
static struct socket* accept_socket;
static uint8_t buffer[BUFFER_SIZE];

int tcp_server_thread(void)
{
    int ret = 0;
    int recveive_size = 0;
    qfprom_tcp_req_t* request;
    qfprom_tcp_resp_t* response;
    scm_qfprom_read_row_data_t row_data = {};
    scm_qfprom_write_row_data_t write_row_data = {};
    bool connected = false;

    ret = sock_create_kern(PF_INET, SOCK_STREAM, IPPROTO_TCP, &accept_socket);
    
    if (ret < 0) {
        log("Could not create tcp accept socket\n");
        goto error;
    }

    for (;;) {

        log("Waiting for connection\n");

        while(accept_socket->ops->accept(socket, accept_socket, SOCK_NONBLOCK) < 0) {
            if (kthread_should_stop()) {
                goto shutdown;
            }
        }

        log("Connected\n");

        connected = true;

        while (connected) {
            if (kthread_should_stop()) {
                goto shutdown;
            }
            recveive_size = tcp_server_receive(accept_socket, buffer, BUFFER_SIZE);
            if (recveive_size) {
                if (recveive_size < 0) {
                    goto error;
                }

                request = (qfprom_tcp_req_t*) &buffer;

                if (request->command == QFPROM_TCP_DISCONNECT_REQUEST || strcasecmp("quit", buffer) == 0 || strcasecmp("exit", buffer) == 0) {
                    goto end_session;
                } else if(request->command == QFPROM_TCP_READ_REQUEST) {
                    
                    memset(&row_data, 0x00, sizeof(row_data));
                    
                    response = (qfprom_tcp_resp_t*) kcalloc(sizeof(uint8_t), sizeof(qfprom_tcp_resp_t), GFP_KERNEL);

                    if (!response) {
                        log("Error allocating response buffer\n");
                        goto shutdown;
                    }
                    
                    response->command       = request->command;
                    response->read.address  = request->read.address;

                    ret = tz_qfprom_read_row(request->read.address, request->read.type, &row_data);

                    if (ret) {                      
                        response->read.error    = ret;
                        response->read.lsb      = 0x000000;
                        response->read.msb      = 0x000000;
                    } else {
                        response->read.error    = row_data.error;
                        response->read.lsb      = row_data.error ? 0x000000 : row_data.lsb;
                        response->read.msb      = row_data.error ? 0x000000 : row_data.msb;
                    }

                    tcp_server_send(accept_socket, (uint8_t*)response, sizeof(qfprom_tcp_resp_t));
                    
                    kfree(response);

                } else if(request->command == QFPROM_TCP_WRITE_REQUEST) {
                    
                    memset(&write_row_data, 0x00, sizeof(write_row_data));
                    
                    response = (qfprom_tcp_resp_t*) kcalloc(sizeof(uint8_t), sizeof(qfprom_tcp_resp_t), GFP_KERNEL);
                    
                    if (!response) {
                        log("Error allocating response buffer\n");
                        goto error;
                    }
                    
                    response->command              = request->command;
                    response->write.address        = request->write.address;
                    response->write.bus_clk_khz    = request->write.bus_clk_khz;

                    ret = tz_qfprom_write_row(request->write.address, request->write.lsb, request->write.msb, request->write.bus_clk_khz, &write_row_data);

                    if (ret) {                      
                        response->write.error   = ret;
                        response->write.lsb     = 0x000000;
                        response->write.msb     = 0x000000;
                    } else {
                        response->write.error   = write_row_data.error;
                        response->write.lsb     = write_row_data.error ? 0x000000 : write_row_data.lsb;
                        response->write.msb     = write_row_data.error ? 0x000000 : write_row_data.msb;
                    }

                    tcp_server_send(accept_socket, (uint8_t*)response, sizeof(qfprom_tcp_resp_t));
                    
                    kfree(response);

                } else if (request->command == QFPROM_TCP_READ_DIRECT_REQUEST) {
                    
                    memset(&row_data, 0x00, sizeof(row_data));

                    response = (qfprom_tcp_resp_t*) kcalloc(sizeof(uint8_t), sizeof(qfprom_tcp_resp_t), GFP_KERNEL);
                    
                    if (!response) {
                        log("Error allocating response buffer\n");
                        goto error;
                    }

                    response->command       = request->command;
                    response->read.address  = request->read.address;

                    ret = qfprom_read_direct(request->read.address, &row_data);

                    response->read.error    = row_data.error;
                    response->read.lsb      = row_data.lsb;
                    response->read.msb      = row_data.msb;

                    tcp_server_send(accept_socket, (uint8_t*)response, sizeof(qfprom_tcp_resp_t));
                    
                    kfree(response);

                } else {
                    log("Invalid Command %d\n", request->command);
                }
            }
        }
end_session:
        connected = false;
        log("Disconnected\n");
        if (accept_socket && accept_socket->ops) {
            accept_socket->ops->shutdown(accept_socket, 0);
            accept_socket->ops->release(accept_socket);
        }
    }

shutdown:
    connected = false;
    thread = NULL;
    do_exit(0);
    return 1;

error:
    connected = false;
    tcp_server_shutdown();
    do_exit(0);
    return 1;
}


int tcp_server_start(int port)
{
    int ret;
    struct sockaddr_in addr;
    mm_segment_t fs;
    int buffer_size = BUFFER_SIZE;  
    bool reuse_addr = true;

    ret = sock_create_kern(AF_INET, SOCK_STREAM, IPPROTO_TCP, &socket);
    
    if (ret < 0) {
        log("Could not create tcp socket\n");
        return ret;
    }

    addr.sin_family         = AF_INET;
    addr.sin_port           = htons(port);
    addr.sin_addr.s_addr    = htonl(INADDR_ANY);

    fs = get_fs();
    
    set_fs(KERNEL_DS);

    sock_setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (void *) &buffer_size, sizeof (buffer_size));
    sock_setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (void *) &reuse_addr, sizeof (reuse_addr));

    set_fs(fs);

    ret = socket->ops->bind(socket,(struct sockaddr*) &addr, sizeof(addr));

    if (ret < 0) {
        log("Error setting up TCP socket\n");
        return 1;
    }

    if (socket->ops->listen(socket, 1)) {
        log("Error setting up TCP listener\n");
        return 1;
    }


    log("Starting TCP thread to listen on port %d\n", port);
    
    thread = kthread_run((void*)&tcp_server_thread, NULL, "tz_qfprom_tcp_server");

    return 0;
}

int tcp_server_receive(struct socket* sock, uint8_t* buffer, int length) 
{
    int ret = 0;
    struct msghdr message;
    struct kvec iov;
    mm_segment_t fs;

    if (sock == NULL) {
        log("NULL socket\n");
        return -1;
    }

    if (sock->sk == NULL) {
        return 0;
    }

    iov.iov_base = buffer;
    iov.iov_len  = length;

    message.msg_control     = NULL;
    message.msg_controllen  = 0;
    message.msg_flags       = 0;
    message.msg_namelen     = 0;
    message.msg_namelen     = 0;

    fs = get_fs();

    set_fs(KERNEL_DS);

    ret = kernel_recvmsg(sock, &message, &iov, 1, length, 0);

    set_fs(fs);

    if (ret > 0) {
        log("Received %d bytes\n", ret);
    }

    return ret;
}

int tcp_server_send(struct socket *sock, uint8_t * data, int length) 
{
    int ret = 0;
    struct msghdr message;
    struct kvec iov;
    mm_segment_t fs;

    
    iov.iov_base    = data;
    iov.iov_len     = length;

    message.msg_control     = NULL;
    message.msg_controllen  = 0;
    message.msg_flags       = 0;
    message.msg_name        = 0;
    message.msg_namelen     = 0;

    fs = get_fs();

    set_fs(KERNEL_DS);

    ret = kernel_sendmsg(sock, &message, &iov, 1, length);    

    log("Wrote %d bytes\n", ret);

    set_fs(fs);

    return ret;
}

void tcp_server_shutdown()
{
    log("Shutting down TCP server\n");
    
    if (thread) {
        kthread_stop(thread);
        thread = NULL;
    }

    if (accept_socket && accept_socket->ops) {
        accept_socket->ops->shutdown(accept_socket, 0);
        accept_socket->ops->release(accept_socket);
    }

    if (socket && socket->ops) {
        socket->ops->shutdown(socket, 0);
        socket->ops->release(socket);
    }
}


