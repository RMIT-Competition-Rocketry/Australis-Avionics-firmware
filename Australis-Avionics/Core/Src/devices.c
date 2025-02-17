#include "devices.h"

static KX134_1211 hAccel;
static KX134_1211 lAccel;
static KX134_1211 accel;
static A3G4250D gyro;
static Flash flash;
static BMP581 baro;
static LoRa lora;
static UART usb;
static GPS gps;

bool initDevices() {
  static DeviceHandle_t deviceList[DEVICE_MAX_KEYS];
  DeviceList_init(deviceList);

  deviceList[DEVICE_ACCEL_HIGH] = KX134_1211_init(&hAccel, "HAccel", ACCEL_PORT_2, ACCEL_CS_2, ACCEL_SCALE_HIGH, ACCEL_AXES_2, ACCEL_SIGN_2);
  deviceList[DEVICE_ACCEL_LOW]  = KX134_1211_init(&lAccel, "LAccel", ACCEL_PORT_1, ACCEL_CS_1, ACCEL_SCALE_LOW, ACCEL_AXES_1, ACCEL_SIGN_1);
  deviceList[DEVICE_BARO]       = BMP581_init(&baro, "Baro", BARO_PORT, BARO_CS, BMP581_TEMP_SENSITIVITY, BMP581_PRESS_SENSITIVITY);
  deviceList[DEVICE_GYRO]       = A3G4250D_init(&gyro, "Gyro", GYRO_PORT, GYRO_CS, A3G4250D_SENSITIVITY, GYRO_AXES, GYRO_SIGN);
  deviceList[DEVICE_FLASH]      = Flash_init(&flash, "Flash", FLASH_PORT, FLASH_CS, FLASH_PAGE_SIZE, FLASH_PAGE_COUNT);
  deviceList[DEVICE_UART_USB]   = UART_init(&usb, "USB", USB_INTERFACE, USB_PORT, USB_PINS, USB_BAUD, OVER8);
  deviceList[DEVICE_GPS]        = GPS_init(&gps, "GPS", GPS_INTERFACE, GPS_PORT, GPS_PINS, GPS_BAUD);
  deviceList[DEVICE_LORA]       = LoRa_init(&lora, "LoRa", LORA_PORT, LORA_CS, BW500, SF9, CR5);

  // Initialise current accelerometer device handle
  deviceList[DEVICE_ACCEL] = deviceList[DEVICE_ACCEL_LOW];

  // TODO: Add error code to device handle and return false on error.
  //       Alternatively change return type and return error struct with status
  //       and description.
  //
  // for(int i = 0; i < DEVICE_MAX_KEYS; i++) {
  //   if(deviceList[i].err != NULL )
  //     return false;
  // }

  return true;
}

extern uint32_t __state_vector_start;
extern uint32_t __state_vector_end;

StateHandle_t StateHandle_getHandle(char *name) {
  StateHandle_t *handleRef = StateHandle_getHandleRef(name);
  return (handleRef == NULL) ? (StateHandle_t){"NULL", NULL} : *handleRef;
}

StateHandle_t *StateHandle_getHandleRef(char *name) {
  // Iterate through all handles in State vector
  for (uint8_t *i = (uint8_t *)&__state_vector_start; i < (uint8_t *)&__state_vector_end; i += sizeof(StateHandle_t)) {
    StateHandle_t *handle = (StateHandle_t *)i;
    // Return State handle if names match
    if (!strcmp(handle->name, name)) {
      handle->ref = handle;
      return handle;
    }
  }
  // Return NULL pointer if no matching State is found
  return NULL;
}
