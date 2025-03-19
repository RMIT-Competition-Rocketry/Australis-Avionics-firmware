/***********************************************************************************
 * @file        shell.c                                                            *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                                                              *
 *                                                                                 *
 * @todo Add commands to buffer to allow managing shell history for frontend       *
 ***********************************************************************************/

#include "shell.h"

#include "devicelist.h"

extern uint32_t __shell_vector_start;
extern uint32_t __shell_vector_end;

/* =============================================================================== */
/**
 * @brief Executes a shell program in a newly created task.
 *
 * Runs the shell program using the provided task parameters and deletes the task
 * upon completion.
 *
 * @param argument Pointer to the task parameters (ShellTaskParams).
 * @return void
 *
 * =============================================================================== */
void vShellExec(void *argument) {
  ShellTaskParams params = *(ShellTaskParams *)argument;

  for (;;) {
    params.shell->run(params.shell, params.str);

    // Remove handle from shell first to prevent
    // nullptr dereference in Shell_clear
    params.shell->taskHandle = NULL;

    // Delete task and end thread
    vTaskDelete(NULL);
  }
}

/* =============================================================================== */
/**
 * @brief Initializes the shell, registering programs from shell vector.
 *
 * Configures shell structure, sets up functions, and registers available shell
 * programs. Returns an error code if the number of programs exceeds the maximum.
 *
 * @param shell Pointer to the Shell structure to initialize.
 * @return 0 on success, 1 if the program count exceeds the maximum.
 *
 * =============================================================================== */
int Shell_init(Shell *shell) {
  shell->usb     = *(UART_t *)DeviceList_getDeviceHandle(DEVICE_UART_USB).device;
  shell->help    = Shell_help;
  shell->run     = Shell_run;
  shell->runTask = Shell_runTask;
  shell->clear   = Shell_clear;

  // TODO:
  // Error checking to determine if any shell programs exist or vector overflow
  // occurred.

  return 0;
}

/* =============================================================================== */
/**
 * @brief Displays available shell commands.
 *
 * Prints a list of all registered shell commands to the USB interface.
 *
 * @param shell Pointer to the Shell structure.
 * @return void
 *
 * =============================================================================== */
void Shell_help(Shell *shell) {
  shell->usb.print(&shell->usb, "Use `help [name]` for more information on a specific command\n\r");
  shell->usb.print(&shell->usb, "The following commands are currently available:\n\r");
  for (uint32_t *i = (uint32_t *)&__shell_vector_start; i < (uint32_t *)&__shell_vector_end; i++) {
    ShellProgramHandle_t *handle = (ShellProgramHandle_t *)*i;
    if (strcmp(handle->name, "")) {
      shell->usb.print(&shell->usb, ":");
      shell->usb.print(&shell->usb, handle->name);
      shell->usb.print(&shell->usb, "\n\r");
    }
  }
}

/* =============================================================================== */
/**
 * @brief Executes a shell program by name.
 *
 * Parses the program name and flags, then finds and runs the corresponding shell
 * program.
 *
 * @param shell 			Pointer to the Shell structure containing registered programs.
 * @param programName Name of the program to execute, with optional flags.
 * @return void
 *
 * =============================================================================== */
void Shell_run(Shell *shell, uint8_t *programName) {
  char *token = strtok((char *)programName, " ");
  char *flags = strchr(token, '\0') + 1;

  // Register programs in vector to shell
  for (uint32_t *i = (uint32_t *)&__shell_vector_start; i < (uint32_t *)&__shell_vector_end; i++) {
    // Dereference memory location and cast to program handle pointer
    ShellProgramHandle_t *handle = (ShellProgramHandle_t *)*i;
    // Iterate shell vector and execute function from handle with matching name (if any)
    if (!strcmp(handle->name, programName)) {
      handle->exec(shell, flags);
      return; // Early exit if program is found
    }
  }

  // Print help string if no matching command is found
  shell->usb.print(&shell->usb, (char *)programName);
  shell->usb.print(&shell->usb, ": command not recognized. Run `help` for a list of available commands\n\r");
}

/* =============================================================================== */
/**
 * @brief Creates a task to run a shell program.
 *
 * Initializes task parameters and spawns a new RTOS task to parse and execute
 * the shell program.
 *
 * @param shell Pointer to the Shell structure containing the program details.
 * @param str Command string to parse and execute.
 * @return void
 *
 * =============================================================================== */
void Shell_runTask(Shell *shell, uint8_t *str) {

  static ShellTaskParams params;
  params.shell = shell;
  params.str   = str;

  // Create new task to parse and execute shell program
  xTaskCreate(
      vShellExec,
      "ShellProgram",
      256,
      (void *)&params,
      configMAX_PRIORITIES - 6,
      &shell->taskHandle
  );
}

/* =============================================================================== */
/**
 * @brief Send clear sequence to host terminal
 *
 * @details `usbClearCommandExecute` transmits over UART an ANSI control sequence for
 * clearing the host terminal window.
 **
 * =============================================================================== */
bool Shell_clear(Shell *shell) {
  shell->usb.print(&shell->usb, CMD_CLEAR);
  // Delete any running task
  if (shell->taskHandle != NULL)
    vTaskDelete(shell->taskHandle);
  return true;
}
