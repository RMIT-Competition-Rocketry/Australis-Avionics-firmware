/***********************************************************************************
 * @file        tasks.c                                                            *
 * @author      Matt Ricci                                                         *
 * @brief                                                                          *
 ***********************************************************************************/

#include "FreeRTOS.h"
#include "task.h"

#include "tasklist.h"
#include "loracomm.h"

#include "tasks.h"

/* ============================================================================================== */
/**
 * @brief Initialise and store FreeRTOS task handles.
 *
 * @return .
 **
 * ============================================================================================== */

void initTasks() {
  xTaskCreate(vLoRaSample, "LoRaSample", 256, NULL, configMAX_PRIORITIES - 6, TaskList_new());
  xTaskCreate(vLoRaTransmit, "LoRaTx", 256, NULL, configMAX_PRIORITIES - 5, TaskList_new());
}
