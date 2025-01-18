/***********************************************************************************
 * @file        help.c                                                             *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/
 
#include "help.h"

static void Help_exec(Shell *, uint8_t *);

static ShellProgramHandle_t registerShellProgram() {
	return (ShellProgramHandle_t){
		.name = "help",
		.exec = Help_exec
	};
}

__attribute__((section(".shell_help"), unused))
static ShellProgramHandle_t (*registerShellProgram_ptr)() = registerShellProgram;

/* =============================================================================== */
/**
 * @brief Wrapper for shell help function
 **
 * =============================================================================== */
static void Help_exec(Shell *shell, uint8_t *flags) {
	shell->help(shell);
}

/** @} */