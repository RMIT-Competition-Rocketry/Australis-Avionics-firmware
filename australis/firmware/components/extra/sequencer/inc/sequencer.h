// ALLOW FORMATTING
#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "tim.h"
#include "gpiopin.h"

typedef struct {
  float tone;
  uint8_t octave;
  uint8_t beat;
  uint8_t volume;
} Note;

typedef struct Sequencer {
  TIM_t noteTimer;
  TIM_t beatTimer;
  uint16_t bpm;
  GPIOpin_t buzzer;
} Sequencer_t;

Sequencer_t Sequencer_init(
  TIM_TypeDef *noteTimer,
  TIM_TypeDef *beatTimer,
  GPIO_TypeDef *buzzerPort,
  uint8_t buzzerPin
);

void Sequencer_play(
  Sequencer_t *sequencer,
  Note *notes,
  uint16_t noteCount,
  uint16_t bpm,
  uint8_t volume
);

#endif
