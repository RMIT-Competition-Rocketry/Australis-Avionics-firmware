/***********************************************************************************
 * @file        devices.c                                                          *
 * @author      Matt Ricci                                                         *
 * @brief       Provides device and peripheral driver initialisations.             *
 ***********************************************************************************/

#include "devicelist.h"
#include "devices.h"
#include "params.h"

// Peripheral includes
#include "uart.h"

// Device includes
#include "a3g4250d.h"
#include "bmp581.h"
#include "w25q128.h"
#include "gps.h"
#include "kx134_1211.h"
#include "sx1272.h"

/* =============================================================================== */
/**
 * @brief Initialise and store device drivers.
 *
 * @return .
 **
 * =============================================================================== */
bool initDevices() {
  static DeviceHandle_t deviceList[DEVICE_MAX_KEYS];
  DeviceList_init(deviceList);

  // ==========================================================================
  // HIGH RANGE ACCELEROMETER
  //
  // Provides low resolution (high scale) inertial data for 3-axis acceleration.
  // This is used for data logging as well as state estimation when
  // the rocket's upward velocity is greater than the maximum high
  // resolution scale.
  static KX134_1211_t hAccel;
  KX134_1211_init(
      &hAccel,
      ACCEL_PORT_2,     // GPIO port connecting accelerometer 2 CS pin
      ACCEL_CS_2,       // Position of accelerometer 2 CS pin in GPIO prt
      ACCEL_SCALE_HIGH, // Set to high resolution for larger G forces
      ACCEL_AXES_2,     // Accelerometer 2 mounting axes
      ACCEL_SIGN_2      // +/- for mounting axes
  );
  deviceList[DEVICE_ACCEL_HIGH].deviceName = "HAccel";
  deviceList[DEVICE_ACCEL_HIGH].device     = &hAccel;

  // ==========================================================================
  // LOW RANGE ACCELEROMETER
  //
  // Provides high resolution (low scale) inertial data for 3-axis acceleration.
  // This is used for data logging as well as state estimation when
  // the rocket's upward velocity is within range of the high resolution.
  static KX134_1211_t lAccel;
  KX134_1211_init(
      &lAccel,
      ACCEL_PORT_1,    // GPIO port connecting accelerometer 1 CS pin
      ACCEL_CS_1,      // Position of accelerometer 1 CS pin in GPIO prt
      ACCEL_SCALE_LOW, // Set to low resolution for smaller G forces
      ACCEL_AXES_1,    // Accelerometer 1 mounting axes
      ACCEL_SIGN_1     // +/- for mounting axes
  );
  deviceList[DEVICE_ACCEL_LOW].deviceName = "LAccel";
  deviceList[DEVICE_ACCEL_LOW].device     = &lAccel;

  // ==========================================================================
  // BAROMETER
  //
  // Measures temperature compensated atmospheric pressure. Data from the
  // barometer is used in altitude calculation, as well as providing one
  // metric for detecting apogee (decreasing pressure readings).
  static BMP581_t baro;
  BMP581_init(
      &baro,
      BARO_PORT,               // GPIO port connecting barometer CS pin
      BARO_CS,                 // Position of barometer CS pin in GPIO prt
      BMP581_TEMP_SENSITIVITY, // Set temperature measurement sensitivity
      BMP581_PRESS_SENSITIVITY // Set pressure measurement sensitivity
  );
  deviceList[DEVICE_BARO].deviceName = "Baro";
  deviceList[DEVICE_BARO].device     = &baro;

  // ==========================================================================
  // GYROSCOPE
  //
  // Measures inertial data for 3-axis rotations. This data is used in
  // calculations for attitude quaternion to determine rotation during flight
  // and apply tilt-angle compensation.
  static A3G4250D_t gyro;
  A3G4250D_init(
      &gyro,
      GYRO_PORT,            // GPIO port connecting gyroscope CS pin
      GYRO_CS,              // Position of gyroscope CS pin in GPIO prt
      A3G4250D_SENSITIVITY, // Set measurement sensitivity
      GYRO_AXES,            // Gyroscope mounting axes
      GYRO_SIGN             // +/- for mounting axes
  );
  deviceList[DEVICE_GYRO].deviceName = "Gyro";
  deviceList[DEVICE_GYRO].device     = &gyro;

  // ==========================================================================
  // FLASH
  //
  // Flash storage for measured and calculated data during flight.
  static W25Q128_t flash;
  W25Q128_init(
      &flash,
      FLASH_PORT,      // GPIO port connecting flash CS pin
      FLASH_CS,        // Position of flash CS pin in GPIO prt
      FLASH_PAGE_SIZE, // Number of bytes per page
      FLASH_PAGE_COUNT // Total number of pages available in flash
  );
  deviceList[DEVICE_FLASH].deviceName = "Flash";
  deviceList[DEVICE_FLASH].device     = &flash;

  // ==========================================================================
  // USB UART
  //
  // UART device allowing communication via USB through an FTDI bridge. This
  // particular UART output provides interaction to the system via shell and
  // debug print output.
  static UART_t uart;
  UART_init(
      &uart,
      USB_INTERFACE, // Memory mapped address of UART interface for USB
      USB_PORT,      // GPIO port connecting UART data pins
      USB_PINS,      // Position of data pins in GPIO prt
      USB_BAUD,      // Baud rate setting of UART communications
      USB_OVERSAMPLE // OVER8 mode on/off
  );
  deviceList[DEVICE_UART_USB].deviceName = "USB";
  deviceList[DEVICE_UART_USB].device     = &uart;

  // ==========================================================================
  // GPS
  //
  // GPS device for low frequency positional readings. Commands are sent and
  // data received via the UART interface.
  static GPS_t gps;
  GPS_init(
      &gps,
      GPS_INTERFACE, // Memory mapped address of UART interface for GPS
      GPS_PORT,      // GPIO port connecting UART data pins
      GPS_PINS,      // Position of data pins in GPIO prt
      GPS_BAUD       // Baud rate setting of UART communications
  );
  deviceList[DEVICE_GPS].deviceName = "GPS";
  deviceList[DEVICE_GPS].device     = &gps;

  // ==========================================================================
  // LORA
  //
  // LoRa transceiver for external wireless communicatons. Can be configured to
  // either receive or transmit data.
  static SX1272_t lora;
  SX1272_init(
      &lora,
      LORA_PORT, // GPIO port connecting LoRa CS pin
      LORA_CS,   // Position of LoRa CS pin in GPIO prt
      BW500,     // Set LoRa bandwidth to 500KHz
      SF9,       // Spreading factor 9
      CR5        // Coding rate 4/5
  );
  deviceList[DEVICE_LORA].deviceName = "LoRa";
  deviceList[DEVICE_LORA].device     = &lora;

  // ==========================================================================
  // !! THIS DEVICE HANDLE WILL BE MODIFIED AT RUN-TIME !!
  // DEVICE_ACCEL represents the current accelerometer selected by the system.
  //
  // The intent is that the high frequency data task will determine which of the
  // two connected accelerometers (configured for high and low resolution) will be
  // used for data acquisition. Other tasks should interact with the current
  // accelerometer without directly modifying it.
  deviceList[DEVICE_ACCEL] = deviceList[DEVICE_ACCEL_LOW];

  return true;
}

// TODO: get rid of this shite vvv

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
