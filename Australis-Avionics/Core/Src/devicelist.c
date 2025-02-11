#include "devicelist.h"

static DeviceHandle_t *deviceListPtr;

/* =============================================================================== */
/**
 * @brief Initialise all system devices
 *
 * @params
 *
 **
 * =============================================================================== */
void DeviceList_init(DeviceHandle_t deviceList[DEVICE_MAX_KEYS]) {
  deviceListPtr = deviceList;
}

/* =============================================================================== */
/**
 * @brief Retrieve device handle from list by key.
 *
 * @params key
 *
 **
 * =============================================================================== */
DeviceHandle_t DeviceList_getDeviceHandle(DeviceKey key) {
  // Return empty handle if key is invalid
  if (key >= DEVICE_MAX_KEYS)
    return (DeviceHandle_t){};

  // Otherwise, return device
  return deviceListPtr[key];
}

/* =============================================================================== */
/**
 * @brief Print out names of all devices in list.
 *
 * @params
 *
 **
 * =============================================================================== */
void DeviceList_printDevices() {
  for (int i = 0; i < DEVICE_MAX_KEYS; i++)
    printf("%s\n", deviceListPtr[i].name);
}
