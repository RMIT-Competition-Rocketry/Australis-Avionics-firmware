#ifndef _DEVICELIST_H
#define _DEVICELIST_H

#include <stdbool.h>
#include <stdio.h>

#define DEVICE_NAME_LENGTH 20

typedef enum {
  DEVICE_ACCEL_HIGH,
  DEVICE_ACCEL_LOW,
  DEVICE_ACCEL,
  DEVICE_GYRO,
  DEVICE_BARO,
  DEVICE_FLASH,
  DEVICE_LORA,
  DEVICE_GPS,
  DEVICE_UART_USB,
  DEVICE_MAX_KEYS
} DeviceKey;

typedef struct DeviceHandle_t {
  char name[DEVICE_NAME_LENGTH];
  void *device;
  struct DeviceHandle_t *ref;
} DeviceHandle_t;

void DeviceList_init(DeviceHandle_t deviceList[DEVICE_MAX_KEYS]);
DeviceHandle_t DeviceList_getDeviceHandle(DeviceKey key);
void DeviceList_printDevices();
#endif
