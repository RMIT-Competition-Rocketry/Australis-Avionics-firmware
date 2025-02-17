/**
 * @author Matt Ricci
 * @addtogroup Shell
 */

#ifndef _SHELL_H
#define _SHELL_H

#include "FreeRTOS.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "task.h"

#include "devicelist.h"
#include "devices.h"
#include "flash.h"
#include "uart.h"

/** @{ */

#define SHELL_MAX_PROGRAMS        10
#define SHELL_PROGRAM_NAME_LENGTH 20

#define CMD_CLEAR                 "clear"

extern uint32_t __shell_vector_start;
extern uint32_t __shell_vector_end;
extern Flash flash;
extern UART usb;

struct Shell;

/**
 * @brief Struct definition for \ref 
 *
 */
typedef struct ShellProgramHandle_t {
  char name[SHELL_PROGRAM_NAME_LENGTH];
  void (*exec)(struct Shell *, uint8_t*);
} ShellProgramHandle_t;

/**
 * @brief Struct definition for \ref 
 *
 */
typedef struct Shell {
  UART usb;
  Flash flash;
  void (*help)(struct Shell *);
  void (*run)(struct Shell *, uint8_t *);
  void (*runTask)(struct Shell *, uint8_t *);
  bool (*clear)(struct Shell *);
  TaskHandle_t taskHandle;
  ShellProgramHandle_t programHandles[SHELL_MAX_PROGRAMS];
} Shell;

/**
 * @brief Struct definition for \ref 
 *
 */
typedef struct ShellTaskParams {
  Shell *shell;
  uint8_t *str;
} ShellTaskParams;

int Shell_init(Shell *);
void Shell_help(Shell *);
void Shell_runTask(Shell *, uint8_t *);
void Shell_run(Shell *, uint8_t *);
bool Shell_clear(Shell *);

/** @} */

#endif
