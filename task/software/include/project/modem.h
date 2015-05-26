#ifndef __PROJECT_MODEM_H__
#define __PROJECT_MODEM_H__

#include <FreeRTOS.h>
#include <queue.h>

void xModemCommunicationTask(void *Parameter);

extern xQueueHandle xModemReceiveQueue;
extern xQueueHandle xModemTransmitQueue;
extern xQueueHandle xModemOutgoingSMSQueue;

#endif
