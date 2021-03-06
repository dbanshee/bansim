/* 
 * File:   pcarsApi.h
 * Author: banshee
 *
 * Created on 3 de junio de 2015, 21:26
 */

#include <stdbool.h>
#include <windows.h>
#include "../headers/ext/SharedMemory.h"


#ifndef PCARSAPI_H
#define PCARSAPI_H

#define PCARS_SHAREDMEM_FILE "$pcars$"

typedef struct pCarsContext {
    HANDLE hFileHandle;
    SharedMemory* shmMem;
} pCarsContext;

void loadDefaultPCarsContext(pCarsContext* pContext);
int initializePCarsContext(pCarsContext* pContext);
void freePCarsContext(pCarsContext* pContext);

#endif /* PCARSAPI_H */

