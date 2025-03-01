/**
 * @author Matt Ricci
 * @addtogroup System
 */

// ALLOW FORMATTING
#ifndef TASKLIST_H
#define TASKLIST_H

#define MAX_TASKS 20

/**
 * @ingroup System
 * @addtogroup Tas_Management Task Management
 * @brief
 * @{
 */

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t *TaskList_getTaskByName(char *);
TaskHandle_t *TaskList_new();

#endif

/** @} */
