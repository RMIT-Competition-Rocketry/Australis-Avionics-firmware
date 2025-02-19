/***********************************************************************************
 * @file        gps.c                                                              *
 * @author      Matt Ricci                                                         *
 * @addtogroup  GPS                                                                *
 * @brief       Brief description of the file's purpose.                           *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "gps.h"

GPS_t GPS_init(
    GPS_t *gps,
    USART_TypeDef *interface,
    GPIO_TypeDef *port,
    UART_Pins pins,
    uint32_t baud
) {
  UART_init(&gps->base, interface, port, pins, baud, OVER8);
  gps->message = GPS_message;
  gps->decode  = GPS_decode;

  gps->base.print(&gps->base, GPS_PUBX_SILENCE);
  //	gps->base.print(&gps->base, "$PUBX,41,1,0003,0003,19200,0*21\r\n"se.setBaud(&gps->base, 19200);

  return *gps;
}

void GPS_message(GPS_t *gps, char *message) {
  UART_t uart = gps->base;
  uart.print(&uart, GPS_PUBX_POLL);

  // Reads GPS message
  uint8_t flag = 0;
  message[0]   = '$';
  message[1]   = 'P';
  message[2]   = 'U';
  message[3]   = 'B';
  message[4]   = 'X';
  message[5]   = ',';

  while (1) {
    for (int x = 0; x < 6; x++) {
      uint8_t byte = uart.receive(&uart);
      if (message[x] != byte) {
        flag = 0;
        break;
      }

      if (x == 5)
        flag = 1;
    }
    if (flag) {
      uint8_t byte = 0;
      for (int x = 6; byte != '\n'; x++) {
        byte       = uart.receive(&uart);
        message[x] = byte;
      }
      return;
    }
  }
}

void GPS_decode(GPS_t *gps, char *message, struct GPS_Data *data) {
  {
    int GPSpointer  = 9;
    int pointer     = 0;
    int tempPointer = 0;
    char *temp[]    = {data->time, data->latitude, data->N_S, data->longitude, data->E_W, data->altref, data->navstat, data->hacc, data->vacc, data->sog, data->cog, data->vvel, data->diffage, data->hdop, data->vdop, data->tdop, data->satellites};
    while (tempPointer <= 16) {
      if (message[GPSpointer] == ',') {
        if ((pointer == 0) && ((tempPointer == 1) || (tempPointer == 3)))
          data->lock = 0;
        else if ((pointer >= 8) && ((tempPointer == 1) || (tempPointer == 3)))
          data->lock = 1;

        pointer = 0;
        tempPointer++;

      } else {
        if (pointer <= 15) {
          temp[tempPointer][pointer]     = message[GPSpointer];
          temp[tempPointer][pointer + 1] = '\0';
          pointer++;
        }
      }
      GPSpointer++;
    }
  }
  data->hour             = (data->time[0] - '0') * 10 + (data->time[1] - '0');
  data->minute           = (data->time[2] - '0') * 10 + (data->time[3] - '0');
  data->second           = (data->time[4] - '0') * 10 + (data->time[5] - '0');
  data->latitude_degrees = (data->latitude[0] - '0') * 10 + (data->latitude[1] - '0');
  int j                  = 1;
  data->latitude_minutes = 0;
  for (int i = 0; i < 9; i++) {
    if (data->latitude[i + 2] == '.')
      i++;
    data->latitude_minutes = data->latitude_minutes + (data->latitude[i + 2] - '0') * (float)pow(10, j);
    j--;
  }
  data->longitude_degrees = (data->longitude[0] - '0') * 100 + (data->longitude[1] - '0') * 10 + (data->longitude[2] - '0');
  j                       = 1;
  data->longitude_minutes = 0;
  for (int i = 0; i < 9; i++) {
    if (data->longitude[i + 3] == '.')
      i++;
    data->longitude_minutes = data->longitude_minutes + (data->longitude[i + 3] - '0') * (float)pow(10, j);
    j--;
  }
}

/** @} */
