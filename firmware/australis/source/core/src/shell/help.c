/***********************************************************************************
 * @file        help.c                                                             *
 * @author      Matt Ricci                                                         *
 * @addtogroup  Shell                   																					 *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "stdint.h"

#include "shell.h"

static void Help_exec(Shell *, uint8_t *);

DEFINE_PROGRAM_HANDLE("help", Help_exec)

/* =============================================================================== */
/**
 * @brief Wrapper for shell help function
 **
 * =============================================================================== */
static void Help_exec(Shell *shell, uint8_t *flags) {
  shell->help(shell);
}

/** @} */
