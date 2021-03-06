/* 
 * File:   serialwin.h
 * Author: banshee
 *
 * Created on 5 de junio de 2015, 15:40
 */

#ifndef SERIALWIN_H
#define SERIALWIN_H

#define ARDUINO_WAIT_TIME 2000

#include <windows.h>

typedef struct serialContext {
    int comPortNumber;
    HANDLE hSerial;
    int connected;
    COMSTAT status;
    DWORD errors;
} serialContext;

void loadDefaultSerialContext(serialContext* ctx);
void setSerialPort(serialContext* ctx, int port);
int initializeSerialContext(serialContext* ctx);
void freeSerialContext(serialContext* ctx);
int readSerialData(serialContext* ctx, void *buffer, unsigned int nbChar);
int writeSerialData(serialContext* ctx, void *buffer, unsigned int nbChar);
int isSerialConnected(serialContext* ctx);


#endif /* SERIALWIN_H */

