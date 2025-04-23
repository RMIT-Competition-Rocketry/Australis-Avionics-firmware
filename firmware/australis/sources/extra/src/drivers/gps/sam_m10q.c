/**************************************************************************************************
 * @file        gps.c                                                                             *
 * @author      Matt Ricci                                                                        *
 * @addtogroup  GPS                                                                               *
 * @brief       Brief description of the file's purpose.                                          *
 *                                                                                                *
 * @{                                                                                             *
 **************************************************************************************************/

#include "sam_m10q.h"

#include "string.h"

/* ============================================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * ============================================================================================== */
bool SAM_M10Q_init(SAM_M10Q_t *gps, UART_t *uart, SAM_M10Q_Config *config) {
  gps->uart      = uart;
  gps->setBaud   = NULL;
  gps->pollPUBX  = SAM_M10Q_pollPUBX;
  gps->parsePUBX = SAM_M10Q_parsePUBX;

  gps->uart->print(gps->uart, GPS_PUBX_SILENCE);
  return true;
}

/* ============================================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * ============================================================================================== */
void SAM_M10Q_setBaud(SAM_M10Q_t *gps, uint32_t baud) {
  // TODO: Make this configurable instead of a hardcoded string
  gps->uart->print(gps->uart, "$PUBX,41,1,0003,0003,19200,0*21\r\n");
  gps->uart->setBaud(gps->uart, 19200);
}

/* ============================================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * ============================================================================================== */
void SAM_M10Q_pollPUBX(SAM_M10Q_t *gps) {
  UART_t *uart = gps->uart;
  uart->print(uart, GPS_PUBX_POLL);
}

/* ============================================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * ============================================================================================== */
bool SAM_M10Q_parsePUBX(SAM_M10Q_t *gps, uint8_t *bytes, SAM_M10Q_Data *data) {
  char *string      = (char *)bytes;
  char *const delim = ",";

  char *tokens[SAM_M10Q_PUBX_POSITION_FIELD_COUNT];

  char *token       = strtok(string, ",");
  uint8_t numTokens = 0;
  // Tokenize string data into array
  while (token != NULL && numTokens < SAM_M10Q_PUBX_POSITION_FIELD_COUNT) {
    tokens[numTokens++] = token;             // Store the token
    token               = strtok(NULL, ","); // Get the next token
  }

  // Early exit if first tokens aren't PUBX identifiers
  if (strcmp(tokens[SAM_M10Q_PUBX_POSITION_TOKEN], "$PUBX")
      || strcmp(tokens[SAM_M10Q_PUBX_POSITION_ID], "00"))
    return false;

  // Copy message tokens to GPS data struct
  strncpy(&data->ns, tokens[SAM_M10Q_PUBX_POSITION_NS], sizeof(data->ns));
  strncpy(&data->ew, tokens[SAM_M10Q_PUBX_POSITION_EW], sizeof(data->ew));
  strncpy(data->time, tokens[SAM_M10Q_PUBX_POSITION_TIME], sizeof(data->time));
  strncpy(data->latitude, tokens[SAM_M10Q_PUBX_POSITION_LAT], sizeof(data->latitude));
  strncpy(data->longitude, tokens[SAM_M10Q_PUBX_POSITION_LONG], sizeof(data->longitude));
  strncpy(data->navstat, tokens[SAM_M10Q_PUBX_POSITION_NAV_STAT], sizeof(data->navstat));

  return true;
}

/** @} */
