#ifndef CM300_NUS_H
#define CM300_NUS_H

#include "CodalConfig.h"

bool bleUartInit(void);
bool bleUartIsConnected(void);
bool bleUartUpdate(void);


uint32_t bleUartAvailable(void);
int32_t  bleUartWrite(uint8_t *p_data, uint32_t length);
uint8_t  bleUartRead(void);
int32_t bleUartPrintf(const char *fmt, ...);

#endif /* CM300_NUS_H */
