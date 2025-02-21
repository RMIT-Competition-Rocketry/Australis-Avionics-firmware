/**
 * @author Matt Ricci
 * @file   main.h
 **/

#ifndef __MAIN_H
#define __MAIN_H

#include "stdint.h"
#include "stdio.h"
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "event_groups.h"
#include "groups.h"
#include "handles.h"
#include "message_buffer.h"
#include "semphr.h"

#include "flashwrite.h"
#include "gpsacquisition.h"
#include "hdataacquisition.h"
#include "ldataacquisition.h"

#include "loracomm.h"
#include "payloadcomm.h"
#include "usbcomm.h"

#include "stateupdate.h"

#include "a3g4250d.h"
#include "bmp581.h"
#include "can.h"
#include "dataframe.h"
#include "devicelist.h"
#include "devices.h"
#include "drivers.h"
#include "w25q128.h"
#include "gps.h"
#include "kx134_1211.h"
#include "sx1272.h"
#include "sensors.h"
#include "shell.h"
#include "uart.h"

#include "kalmanfilter.h"
#include "membuff.h"
#include "quaternion.h"
#include "slidingwindow.h"

#ifdef DUMMY
  #include "accelX.h"
  #include "accelY.h"
  #include "accelZ.h"
  #include "gyroX.h"
  #include "gyroY.h"
  #include "gyroZ.h"
  #include "press.h"
#endif

void vSystemInit(void *pvParameters);
void deviceInit();
void configure_interrupts();
void Error_Handler(void);

#define AVG_BUFF_SIZE  15
#define LORA_BUFF_SIZE 128
#define MEM_BUFF_SIZE  20992

#endif
