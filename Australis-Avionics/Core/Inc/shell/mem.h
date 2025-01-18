/**
 * @author Matt Ricci
 * @addtogroup Shell
 * @file shell.h
 */

#ifndef _MEM_H
#define _MEM_H

#include "FreeRTOS.h"

#include "devices.h"
#include "shell.h"
#include "flash.h"
#include "uart.h"

#define CMD_FLASH_ERASE     "erase"
#define CMD_FLASH_READ_PAGE "read page"
#define CMD_FLASH_READ_ALL  "read all"

#endif
