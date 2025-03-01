/**
 * @author Matt Ricci
 * @addtogroup System
 */

// ALLOW FORMATTING
#ifndef _SHELL_H
#define _SHELL_H

#include "FreeRTOS.h"
#include "stdbool.h"
#include "stdint.h"
#include "task.h"

#include "uart.h"

// clang-format off

// Macro for shell program definition boilerplate. The expansion of this will define
// a static global-scope ShellProgramHandle_t pointer to a compound literal struct.
#define DEFINE_PROGRAM_HANDLE(progName, execFunction)                              \
  static ShellProgramHandle_t __attribute__((section(".shell_" progName), unused)) \
  *progHandle = &(ShellProgramHandle_t){                                           \
      .name = progName,                                                            \
      .exec = execFunction                                                         \
  };

// Max allowable characters for name
#define SHELL_PROGRAM_NAME_LENGTH 20

// Terminal clear screen control sequence
#define CMD_CLEAR "\033[3J\033[H\033[2J"

// clang-format on

/**
 * @ingroup System
 * @addtogroup Shell
 * @brief Australis shell interface for user interaction
 *        with the system via terminal command line.
 * @{
 */

/**
 * @brief   Struct definition for shell interface.
 * @details
 */
typedef struct Shell {
  UART_t usb;                                 //!< UART interface to connect shell I/O.
  void (*help)(struct Shell *);               //!< @see Shell_help
  void (*run)(struct Shell *, uint8_t *);     //!< @see Shell_run
  void (*runTask)(struct Shell *, uint8_t *); //!< @see Shell_runTask
  bool (*clear)(struct Shell *);              //!< @see Shell_clear
  TaskHandle_t taskHandle;                    //!< Handle of currently active program in shell thread.
} Shell;

/**
 * @brief   Struct definition for shell program handle.
 * @details Provides the interface for shell programs.
 * @details All handles must have a name (case-sensitive) that represents the
 *          program as it will be called from the shell interface.
 * @details Additionally, the handle must initialise the exec function pointer.
 *          This defines the program's entry point and is called by the shell.
 */
typedef struct ShellProgramHandle_t {
  char name[SHELL_PROGRAM_NAME_LENGTH];    //!< Program name as referenced by the shell
  void (*exec)(struct Shell *, uint8_t *); //!< Program entry point function pointer
} ShellProgramHandle_t;

/**
 * @brief Struct definition for parameters passed to shell task.
 *
 */
typedef struct ShellTaskParams {
  Shell *shell; //!<
  uint8_t *str; //!<
} ShellTaskParams;

int Shell_init(Shell *);
void Shell_help(Shell *);
void Shell_runTask(Shell *, uint8_t *);
void Shell_run(Shell *, uint8_t *);
bool Shell_clear(Shell *);

/** @} */

#endif
