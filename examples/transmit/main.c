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
//////////////////////
// GLOBAL VARIABLES //
//////////////////////

char customData[1000][11];
int size;

//////////////////////////////////////////////////
// CONFIGURATION (FOR APPLICATION CALLBACKS BELOW)
//////////////////////////////////////////////////

// application router ID (LSBF)
static const u1_t APPEUI[8]  = { 0x16, 0x77, 0x01, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };

// unique device ID (LSBF)
static const u1_t DEVEUI[8]  = { 0x4B, 0x7B, 0xAB, 0x3B, 0x66, 0xDF, 0x99, 0x00 };

// device-specific AES key (derived from device EUI)
static const u1_t DEVKEY[16] = { 0x63, 0xD2, 0xCB, 0xC4, 0x31, 0x3D, 0xD8, 0x2F, 0x4A, 0xF8, 0xED, 0x99, 0x65, 0xA0, 0x49, 0xF7 };


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
    // Load log.out into customData[]
    // immediately prepare next transmissio
    static const char* fn = "/home/pi/MasterCode/log.out";
    int fd = open(fn, O_RDWR);
    if(fd < 0) {
	    perror(fn);
	    exit(EXIT_FAILURE);
    }
    char buffer[11];
    int linecount = 0;
    int rc;
    do {
	    rc = read(fd, &customData[linecount], 11);
	    if (rc < 0) {
		    perror("bad stuff.");
	            exit(EXIT_FAILURE);
        	}
	    customData[linecount][rc-1] = 0;
	    debug_str(customData[linecount]);
	    linecount++;
    } while (rc == 11);
    // reset MAC stat
    close(fd);
    size = linecount - 2;
    debug_str("///DATA HERE//////");
    for(int i = 0; i <= size; i++){
    	debug_str(customData[i]);
    }
    printf("%d\n",size);
    debug_str("///DATA HERE//////");
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
    if(size < 0){
	    debug_str("/////////////FINISHED///////////");
	    exit(0);
    }
    
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
    tx:
        /*debug_str("#############################################################");
        debug_str(data);
	*/
        //debug_str("#############################################################");
       
        for(int i = 0; i < 10; i++) {
            LMIC.frame[i] = customData[size][i];
	    //debug_str(&customData[size][i])
	    //debug_str("/////////////");
	    //debug_str(&LMIC.frame[i]);
        }
        LMIC_setTxData2(1, LMIC.frame, 10, 1);
        // (will be sent as soon as duty cycle permits)
	size--;
        break;
    }
}

