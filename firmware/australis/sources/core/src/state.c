/***********************************************************************************
 * @file        state.c                                                            *
 * @author      Matt Ricci                                                         *
 * @addtogroup  State                                                              *
 *                                                                                 *
 * @{                                                                              *
 ***********************************************************************************/

#include "state.h"

#define AVG_BUFF_SIZE   15
#define MEM_BUFF_SIZE   20992
#define FLASH_PAGE_SIZE 256

float avgPressBuff[AVG_BUFF_SIZE];
float avgVelBuff[AVG_BUFF_SIZE];
uint8_t buff[MEM_BUFF_SIZE];

static State state = {
};

/* =============================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * =============================================================================== */
void State_init() {
  state.flightState = PRELAUNCH;
  state.tilt        = 0.0f;
  state.cosine      = 0.0f;
  state.altitude    = 0.0f;
  state.velocity    = 0.0f;
  state.rotation    = Quaternion_new();
  state.avgVel      = SlidingWindow_new(avgVelBuff, AVG_BUFF_SIZE);
  state.avgPress    = SlidingWindow_new(avgVelBuff, AVG_BUFF_SIZE);
  state.mem         = MemBuff_new(buff, MEM_BUFF_SIZE, FLASH_PAGE_SIZE);
}

/* =============================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * =============================================================================== */
State *State_getState() {
  return &state;
}
