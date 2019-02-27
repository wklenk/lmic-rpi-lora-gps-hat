/*
 * Copyright (c) 2014-2016 IBM Corporation.
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _OPEN_SYS_ITOA_EXT

#include "lmic.h"
#include "debug.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

//////////////////////////////////////////////////
// CONFIGURATION (FOR APPLICATION CALLBACKS BELOW)
//////////////////////////////////////////////////

// application router ID (LSBF)
static const u1_t APPEUI[8]  = { 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x };

// unique device ID (LSBF)
static const u1_t DEVEUI[8]  = { 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x };

// device-specific AES key (derived from device EUI)
static const u1_t DEVKEY[16] = { 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x };


//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

// provide application router ID (8 bytes, LSBF)
void os_getArtEui (u1_t* buf) {
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui (u1_t* buf) {
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey (u1_t* buf) {
    memcpy(buf, DEVKEY, 16);
}


//////////////////////////////////////////////////
// MAIN - INITIALIZATION AND STARTUP
//////////////////////////////////////////////////

// initial job
static void initfunc (osjob_t* j) {
    // reset MAC state
    LMIC_reset();
    // start joining
    LMIC_startJoining();
    // init done - onEvent() callback will be invoked...
}


// application entry point
int main () {
    osjob_t initjob;
    
    // initialize runtime env
    os_init();
    // initialize debug library
    debug_init();
    // setup initial job
    os_setCallback(&initjob, initfunc);
    // execute scheduled jobs and events
    os_runloop();
    // (not reached)
    return 0;
}


//////////////////////////////////////////////////
// LMIC EVENT CALLBACK
//////////////////////////////////////////////////

void onEvent (ev_t ev) {
    debug_event(ev);
    
    switch(ev) {
        
        // network joined, session established
        case EV_JOINED:
        debug_val("netid = ", LMIC.netid);
        goto tx;
        
        // scheduled data sent (optionally data received)
        case EV_TXCOMPLETE:
        if(LMIC.dataLen) { // data received in rx slot after tx
            debug_buf(LMIC.frame+LMIC.dataBeg, LMIC.dataLen);
        }
        static const char* fn = "/home/pi/setup/log.out";
        int fd;
    tx:
        // immediately prepare next transmission
        fd = open(fn, O_RDWR);
        if(fd < 0) {
            perror(fn);
            exit(EXIT_FAILURE);
        }
        
        char data[10];
        char buffer[10];
        int rc = read(fd, &data, 10);
        if (rc < 0) {
            perror("bad stuff.");
            exit(EXIT_FAILURE);
        }
        /* // Found at: https://codereview.stackexchange.com/questions/156477/c-program-to-count-number-of-lines-in-a-file
         while ((bytes = fread(buffer, 1, sizeof(buffer) - 1, fd))) {
         lastchar = buffer[bytes - 1];
         for (char *c = buffer; (c = memchr(c, '\n', bytes - (c - buffer))); c++) {
         lines++;
         }
         }
         */
        close(fd);
        debug_str("#############################################################");
        debug_str(data);
        debug_str("#############################################################");
        
        for(int i =0; i < strlen(data); i++) {
            //LMIC.frame[i] = sprintf(buffer, "%x", data[i]);
            
            LMIC.frame[i] = data[i];
        }
        
        /*
         LMIC.frame[0] = 0x0;
         LMIC.frame[1] = 0x1;
         LMIC.frame[2] = 0x1;
         LMIC.frame[3] = 0x0;
         LMIC.frame[4] = 0x1;
         LMIC.frame[5] = 0x6;
         LMIC.frame[6] = 0xa;
         LMIC.frame[7] = 0x3;
         LMIC.frame[8] = 0x2;
         LMIC.frame[9] = 0xf;
         */
        
        //LMIC.frame[0] = LMIC.snr;
        // schedule transmission (port 1, datalen 1, no ack requested)
        LMIC_setTxData2(1, LMIC.frame, 10, 1);
        // (will be sent as soon as duty cycle permits)
        break;
    }
}

