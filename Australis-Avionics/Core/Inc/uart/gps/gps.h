/**
 * @author Matt Ricci
 * @addtogroup UART
 * @{
 * @addtogroup GPS
 * @}
 * @todo Add to UART group
 */

#ifndef _GPS_H
#define _GPS_H

#define GPS_PUBX_SILENCE                                                       \
  "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n$PUBX,40,RMC,0,0,0,0,0,0*47\r\n$PUBX,40,"    \
  "GSA,0,0,0,0,0,0*4E\r\n$PUBX,40,GSV,0,0,0,0,0,0*59\r\n$PUBX,40,GGA,0,0,0,0," \
  "0,0*5A\r\n$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n$PUBX,40,GSV,0,0,0,0,0,0*59\r\n"

#define GPS_PUBX_POLL "$PUBX,00*33\r\n"

#include "math.h"
#include "stdint.h"
#include "stm32f4xx.h"

#include "devicelist.h"
#include "uart/uart.h"

/**
 * @addtogroup GPS
 * @{
 */

typedef struct GPS_Data {
  // PUBX 00 message
  char time[15];
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  char latitude[15];
  char latitude_degrees;
  float latitude_minutes;
  char N_S[15];
  char longitude[15];
  char longitude_degrees;
  float longitude_minutes;
  char E_W[15];
  char altref[15];     // Altitude above sea level
  char navstat[3];     // Navigation status (NF = No Fix, DR = Dead Reckoning, G2 = 2D Fix, G3 = 3D Fix,D2 = 2D Differential, D3 = 3D Differential, RK = combined GPS + dead reckoning solution, TT = Time only solution)
  char hacc[15];       // Horizontal accuracy estimate
  char vacc[15];       // Vertical accuracy estimate
  char sog[15];        // Speed over ground
  char cog[15];        // Course over ground
  char vvel[15];       // Vertical velocity
  char diffage[15];    // Age of DGPS data
  char hdop[15];       // Horizontal dilution of precision
  char vdop[15];       // Vertical dilution of precision
  char tdop[15];       // Time dilution of precision
  char satellites[15]; // Number of satellites used
  uint8_t lock;        // 0 = no lock, 1 = lock
} GPS_Data;

typedef struct GPS {
  UART base;
  GPIO_TypeDef *port;
  UART_Pins pins;
  uint32_t baud;
  void (*message)(struct GPS *, char *);
  void (*decode)(struct GPS *, char *, struct GPS_Data *);
} GPS;

DeviceHandle_t GPS_init(GPS *, char *, USART_TypeDef *, GPIO_TypeDef *, UART_Pins, uint32_t);

void GPS_message(GPS *, char *);
void GPS_decode(GPS *, char *, struct GPS_Data *);

/** @} */
#endif
