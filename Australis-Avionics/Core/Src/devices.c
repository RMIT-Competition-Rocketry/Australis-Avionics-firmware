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
#include "gpiopin.h"

// Device includes
#include "a3g4250d.h"
#include "bmp581.h"
#include "w25q128.h"
#include "sam_m10q.h"
#include "kx134_1211.h"
#include "sx1272.h"

static DeviceHandle_t deviceList[DEVICE_MAX_KEYS];

/* ============================================================================================== */
/**
 * @brief Initialise and store device drivers.
 *
 * @return .
 **
 * ============================================================================================== */
bool initDevices() {
  DeviceList_init(deviceList);

  // SPI peripherals and devices
  initSpiPins();
  initSensors();
  initFlash();
  initLora();

  // UART peripherals and devices
  initUart();

  // @TODO: add in error checking
  return true;
}

/* ============================================================================================== */
/**
 * @brief   Initialise SPI bus pins.
 * @details Initialises GPIO pins for SCK, SDI and SDO on each bus. See \ref devices.c for Port/pin,
 *          alternate function selections and pin mapping definitions.
 *
 * @return  \c NULL.
 **
 * ============================================================================================== */
void initSpiPins() {
  // Sensor configuration
  GPIO_Config spiSensorConfig = GPIO_CONFIG_DEFAULT;
  spiSensorConfig.mode        = GPIO_MODE_AF;
  spiSensorConfig.afr         = SENSORS_SPI_AF;

  GPIOpin_t sensorSCK         = GPIOpin_init(SENSORS_SPI_PORT, SENSORS_SPI_SCK, &spiSensorConfig);
  GPIOpin_t sensorSDI         = GPIOpin_init(SENSORS_SPI_PORT, SENSORS_SPI_SDI, &spiSensorConfig);
  GPIOpin_t sensorSDO         = GPIOpin_init(SENSORS_SPI_PORT, SENSORS_SPI_SDO, &spiSensorConfig);

  // Flash configuration
  GPIO_Config spiFlashConfig = GPIO_CONFIG_DEFAULT;
  spiFlashConfig.mode        = GPIO_MODE_AF;
  spiFlashConfig.afr         = FLASH_SPI_AF;

  GPIOpin_t flashSCK         = GPIOpin_init(FLASH_SPI_PORT, FLASH_SPI_SCK, &spiFlashConfig);
  GPIOpin_t flashSDI         = GPIOpin_init(FLASH_SPI_PORT, FLASH_SPI_SDI, &spiFlashConfig);
  GPIOpin_t flashSDO         = GPIOpin_init(FLASH_SPI_PORT, FLASH_SPI_SDO, &spiFlashConfig);

  // LoRa configuration
  GPIO_Config spiLoraConfig = GPIO_CONFIG_DEFAULT;
  spiLoraConfig.mode        = GPIO_MODE_AF;
  spiLoraConfig.afr         = LORA_SPI_AF;

  GPIOpin_t loraSCK         = GPIOpin_init(LORA_SPI_PORT, LORA_SPI_SCK, &spiLoraConfig);
  GPIOpin_t loraSDI         = GPIOpin_init(LORA_SPI_PORT, LORA_SPI_SDI, &spiLoraConfig);
  GPIOpin_t loraSDO         = GPIOpin_init(LORA_SPI_PORT, LORA_SPI_SDO, &spiLoraConfig);
}

/* ============================================================================================== */
/**
 * @brief Initialise and store device drivers.
 *
 * @return .
 **
 * ============================================================================================== */
bool initSensors() {

  SPI_t spiSensors = SPI_init(SENSORS_SPI_INTERFACE, NULL);

  // ==========================================================================
  // HIGH RANGE ACCELEROMETER
  //
  // Provides low resolution (high scale) inertial data for 3-axis acceleration.
  // This is used for data logging as well as state estimation when
  // the rocket's upward velocity is greater than the maximum high
  // resolution scale.
  GPIOpin_t hAccelCS = GPIOpin_init(ACCEL_CS2, NULL);
  static KX134_1211_t hAccel;
  KX134_1211_init(
      &hAccel,
      &spiSensors,
      hAccelCS,
      ACCEL_SCALE_HIGH, // Set to high scale for larger G forces
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
  GPIOpin_t lAccelCS = GPIOpin_init(ACCEL_CS1, NULL);
  static KX134_1211_t lAccel;
  KX134_1211_init(
      &lAccel,
      &spiSensors,
      lAccelCS,
      ACCEL_SCALE_LOW, // Set to low scale for smaller G forces
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
  GPIOpin_t baroCS = GPIOpin_init(BARO_CS, NULL);
  static BMP581_t baro;
  BMP581_init(
      &baro,
      &spiSensors,
      baroCS,
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
  GPIOpin_t gyroCS = GPIOpin_init(GYRO_CS, NULL);
  static A3G4250D_t gyro;
  A3G4250D_init(
      &gyro,
      &spiSensors,
      gyroCS,
      A3G4250D_SENSITIVITY, // Set measurement sensitivity
      GYRO_AXES,            // Gyroscope mounting axes
      GYRO_SIGN             // +/- for mounting axes
  );
  deviceList[DEVICE_GYRO].deviceName = "Gyro";
  deviceList[DEVICE_GYRO].device     = &gyro;

  // ==========================================================================
  // !! THIS DEVICE HANDLE WILL BE MODIFIED AT RUN-TIME !!
  // DEVICE_ACCEL represents the current accelerometer selected by the system.
  //
  // The intent is that the high frequency data task will determine which of the
  // two connected accelerometers (configured for high and low resolution) will be
  // used for data acquisition. Other tasks should interact with the current
  // accelerometer without directly modifying it.
  deviceList[DEVICE_ACCEL] = deviceList[DEVICE_ACCEL_LOW];

  // @TODO: add in error checking
  return true;
}

/* ============================================================================================== */
/**
 * @brief Initialise and store flash device driver.
 *
 * @return .
 **
 * ============================================================================================== */
bool initFlash() {

  SPI_t spiFlash = SPI_init(SENSORS_SPI_INTERFACE, NULL);

  // ==========================================================================
  // FLASH
  //
  // Flash storage for measured and calculated data during flight. Data is
  // written to the device in dataframe chunks. Each interval of high and
  // low resolution data is appended in a frame to a circular memory buffer
  // by the system, and is written to flash during idle time once at least a
  // full page of data is available.
  GPIOpin_t flashCS = GPIOpin_init(FLASH_CS_PORT, FLASH_CS_PIN, NULL);
  static W25Q128_t flash;
  W25Q128_init(
      &flash,
      &spiFlash,
      flashCS
  );
  deviceList[DEVICE_FLASH].deviceName = "Flash";
  deviceList[DEVICE_FLASH].device     = &flash;

  // @TODO: add in error checking
  return true;
}

/* ============================================================================================== */
/**
 * @brief Initialise and store LoRa device driver.
 *
 * @return .
 **
 * ============================================================================================== */
bool initLora() {

  SPI_t spiLora = SPI_init(SENSORS_SPI_INTERFACE, NULL);

  // ==========================================================================
  // LORA
  //
  // LoRa transceiver for external wireless communicatons. Can be configured to
  // either receive or transmit data.
  GPIOpin_t loraCS = GPIOpin_init(LORA_CS, NULL);
  static SX1272_t lora;
  SX1272_init(
      &lora,
      &spiLora,
      loraCS,
      LORA_BW, // Set LoRa bandwidth to 500KHz
      LORA_SF, // Spreading factor 9
      LORA_CR  // Coding rate 4/5
  );
  deviceList[DEVICE_LORA].deviceName = "LoRa";
  deviceList[DEVICE_LORA].device     = &lora;

  // @TODO: add in error checking
  return true;
}

/* ============================================================================================== */
/**
 * @brief Initialise and store UART device drivers.
 *
 * @return .
 **
 * ============================================================================================== */
bool initUart() {

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
  static SAM_M10Q_t gps;
  GPS_init(
      &gps,
      GPS_INTERFACE, // Memory mapped address of UART interface for GPS
      GPS_PORT,      // GPIO port connecting UART data pins
      GPS_PINS,      // Position of data pins in GPIO prt
      GPS_BAUD       // Baud rate setting of UART communications
  );
  deviceList[DEVICE_GPS].deviceName = "GPS";
  deviceList[DEVICE_GPS].device     = &gps;

  // @TODO: add in error checking
  return true;
}

// !! ABANDON HOPE ALL YE WHO ENTER HERE !!
//                 🤢🤮

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
