/**
 * @author Matt Ricci
 * @addtogroup API
 * @{
 * @addtogroup CAN
 * @}
 */

// ALLOW FORMATTING
#ifndef CAN_H
#define CAN_H

#include "stm32f439xx.h"
#include "stdbool.h"

#define CAN_STID_MAX 2048

typedef struct {

} CAN_Config;

typedef struct {
  uint32_t id;
  uint8_t length;
  uint64_t data[2];
} CAN_Data;

typedef struct CAN {
  CAN_TypeDef *interface;                                    //!<
  CAN_Config config;                                         //!<
  uint8_t (*transmit)(struct CAN *can, CAN_Data *txData);    //!<
  bool (*receive)(struct CAN *can, CAN_Data *rxData);        //!<
  void (*updateConfig)(struct CAN *can, CAN_Config *config); //!<
} CAN_t;

CAN_t CAN_init(CAN_TypeDef *interface, CAN_Config *config);
uint8_t CAN_transmit(CAN_t *can, CAN_Data *txData);
bool CAN_receive(CAN_t *can, CAN_Data *rxData);
void CAN_updateConfig(CAN_t *can, CAN_Config *config);

void CAN_Peripheral_config();
void CANGPIO_config();

#endif
