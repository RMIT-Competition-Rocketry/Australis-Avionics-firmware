/***********************************************************************************
 *
 * @file        A3G4250D.c                                                         *
 * @author      Matt Ricci                                                         *
 * @addtogroup  A3G4250D                                                           *
 *                                                                                 *
 * @todo Move private interface methods (read/write register) to static functions  *
 *       with internal prototypes.                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "a3g4250d.h"

#include "string.h"

/* =============================================================================== */
/**
 * @brief Initialiser for a A3G4250D gyroscope.
 *
 * @param *gyro 			Pointer to A3G4250D struct to be initialised.
 * @param *port 			Pointer to GPIO port struct.
 * @param cs 					Device chip select address.
 * @param scale       Selected scale for read gyro rates.
 * @param *axes       Array defining sensor mounting axes.
 * @return @c NULL.
 **
 * =============================================================================== */
A3G4250D_t A3G4250D_init(
    A3G4250D_t *gyro,
    SPI_t *spi,
    GPIOpin_t cs,
    float sensitivity,
    const uint8_t *axes,
    const int8_t *sign
) {
  gyro->base            = spi;
  gyro->cs              = cs;
  gyro->sensitivity     = sensitivity;
  gyro->update          = A3G4250D_update;
  gyro->readGyro        = A3G4250D_readGyro;
  gyro->readRawBytes    = A3G4250D_readRawBytes;
  gyro->processRawBytes = A3G4250D_processRawBytes;
  memcpy(gyro->axes, axes, A3G4250D_DATA_COUNT);
  memcpy(gyro->sign, sign, A3G4250D_DATA_COUNT);

  // Wait for the spefified period - need to wait for 2ms here.
  for (uint32_t i = 0; i < 0xFFFF; i++);

  A3G4250D_writeRegister(gyro, A3G4250D_CTRL_REG1, A3G4250D_CTRL_REG1_ODR_800Hz | A3G4250D_CTRL_REG1_AXIS_ENABLE | A3G4250D_CTRL_REG1_PD_ENABLE);

  return *gyro;
}

/******************************** DEVICE METHODS ********************************/

/* =============================================================================== */
/**
 * @brief Read 3-axis floating point gyro rates.
 *
 * @param 	*gyro 		Pointer to gyro struct.
 * @param 	*out 		  Floating point gyro rate array.
 * @returns @c NULL.
 **
 * =============================================================================== */
void A3G4250D_readGyro(A3G4250D_t *gyro, float *out) {
  uint8_t bytes[A3G4250D_DATA_TOTAL];
  gyro->readRawBytes(gyro, bytes);
  gyro->processRawBytes(gyro, bytes, out);
}

/* =============================================================================== */
/**
 * @brief Updates internally stored gyro readings.
 *
 * @param 	*gyro 		Pointer to gyro struct.
 * @returns @c NULL.
 **
 * =============================================================================== */
void A3G4250D_update(A3G4250D_t *gyro) {
  gyro->readRawBytes(gyro, gyro->rawGyroData);
  gyro->processRawBytes(gyro, gyro->rawGyroData, gyro->gyroData);
}

/* =============================================================================== */
/**
 * @brief Process raw 3-axis data to floating point gyro rates.
 *
 * @param 	*gyro 		Pointer to gyro struct.
 * @param 	*bytes 		Raw 3-axis data array.
 * @param 	*out 			Processed 3-axis data array to write.
 * @returns @c NULL.
 **
 * =============================================================================== */
void A3G4250D_processRawBytes(A3G4250D_t *gyro, uint8_t *bytes, float *out) {
  out[0] = gyro->sign[0] * gyro->sensitivity * (int16_t)(((uint16_t)bytes[0] << 8) | bytes[1]); // gyro X
  out[1] = gyro->sign[1] * gyro->sensitivity * (int16_t)(((uint16_t)bytes[2] << 8) | bytes[3]); // gyro Y
  out[2] = gyro->sign[2] * gyro->sensitivity * (int16_t)(((uint16_t)bytes[4] << 8) | bytes[5]); // gyro Z
}

/* =============================================================================== */
/**
 * @brief Read raw 3-axis data.
 *
 * @param 	*gyro 		Pointer to gyro struct.
 * @param 	*out 			Raw 3-axis data array to write.
 * @returns @c NULL.
 **
 * =============================================================================== */
void A3G4250D_readRawBytes(A3G4250D_t *gyro, uint8_t *out) {
#define INDEX_AXES(index, byte) 2 * gyro->axes[index] + byte
  out[INDEX_AXES(0, 0)] = A3G4250D_readRegister(gyro, A3G4250D_OUT_X_H); // gyro X high
  out[INDEX_AXES(0, 1)] = A3G4250D_readRegister(gyro, A3G4250D_OUT_X_L); // gyro X low
  out[INDEX_AXES(1, 0)] = A3G4250D_readRegister(gyro, A3G4250D_OUT_Y_H); // gyro Y high
  out[INDEX_AXES(1, 1)] = A3G4250D_readRegister(gyro, A3G4250D_OUT_Y_L); // gyro Y low
  out[INDEX_AXES(2, 0)] = A3G4250D_readRegister(gyro, A3G4250D_OUT_Z_H); // gyro Z high
  out[INDEX_AXES(2, 1)] = A3G4250D_readRegister(gyro, A3G4250D_OUT_Z_L); // gyro Z low
#undef INDEX_AXES
}

/******************************** INTERFACE METHODS ********************************/

void A3G4250D_writeRegister(A3G4250D_t *gyro, uint8_t address, uint8_t data) {
  SPI_t *spi   = gyro->base;
  GPIOpin_t cs = gyro->cs;

  cs.reset(&cs);

  // Send read command and address
  uint8_t payload = address & 0x7F; // Load payload with address and read command
  spi->transmit(spi, payload);      // Transmit payload
  spi->transmit(spi, data);         // Transmit dummy data and read response data

  cs.set(&cs);
}

uint8_t A3G4250D_readRegister(A3G4250D_t *gyro, uint8_t address) {
  uint8_t response = 0;
  SPI_t *spi       = gyro->base;
  GPIOpin_t cs     = gyro->cs;

  cs.reset(&cs);

  // Send read command and address
  uint8_t payload = address | 0x80;              // Load payload with address and read command
  response        = spi->transmit(spi, payload); // Transmit payload
  response        = spi->transmit(spi, 0xFF);    // Transmit dummy data and read response data

  cs.set(&cs);

  return response;
}

/** @} */
