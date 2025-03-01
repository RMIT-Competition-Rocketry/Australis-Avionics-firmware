/**
 * @author Matt Ricci
 * @addtogroup Shell
 * @file shell.h
 */

#ifndef _MEM_H
#define _MEM_H

#include "FreeRTOS.h"

#include "devicelist.h"
#include "w25q128.h"
#include "shell.h"
#include "uart.h"

#define CMD_FLASH_ERASE     "erase"
#define CMD_FLASH_READ_PAGE "read page"
#define CMD_FLASH_READ_ALL  "read all"

#endif
