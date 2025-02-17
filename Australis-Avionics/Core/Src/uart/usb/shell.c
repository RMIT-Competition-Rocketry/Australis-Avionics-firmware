/***********************************************************************************
 * @file        shell.c                                                            *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                                                              *
 *                                                                                 *
 * @todo Add commands to buffer to allow managing shell history for frontend       *
 ***********************************************************************************/

#include "devices.h"
#include "drivers.h"
#include "shell.h"

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
  shell->usb     = *(UART *)DeviceList_getDeviceHandle(DEVICE_UART_USB).device;
  shell->help    = Shell_help;
  shell->run     = Shell_run;
  shell->runTask = Shell_runTask;
  shell->clear   = Shell_clear;

  // Return error code if shell vector exceeds max allowable programs
  if (((&__shell_vector_end - &__shell_vector_start) / sizeof(uint32_t)) > SHELL_MAX_PROGRAMS)
    return 1;

  // Register programs in vector to shell
  int idx = 0;
  for (uint32_t *i = (uint32_t *)&__shell_vector_start; i < (uint32_t *)&__shell_vector_end; i++) {
    // Dereference function pointer
    ShellProgramHandle_t (*registerShellProgram)() = (ShellProgramHandle_t(*)())(*i);

    // Register program handle to shell
    shell->programHandles[idx] = registerShellProgram();
    idx++;
  }

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
  for (int i = 0; i < SHELL_MAX_PROGRAMS; i++) {
    if (strcmp(shell->programHandles[i].name, "")) {
      shell->usb.print(&shell->usb, ":");
      shell->usb.print(&shell->usb, shell->programHandles[i].name);
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

  // Iterate through shell vector and execute function from handle
  // with matching name (if any).
  for (int i = 0; i < SHELL_MAX_PROGRAMS; i++) {
    if (!strcmp(shell->programHandles[i].name, programName)) {
      shell->programHandles[i].exec(shell, flags);
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
  shell->usb.sendBytes(&shell->usb, (uint8_t *)"\033[3J\033[H\033[2J", 11);
  // Delete any running task
  if (shell->taskHandle != NULL)
    vTaskDelete(shell->taskHandle);
  return true;
}
