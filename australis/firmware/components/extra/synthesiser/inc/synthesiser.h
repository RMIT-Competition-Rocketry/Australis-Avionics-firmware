// ALLOW FORMATTING
#ifndef SYNTHESISER_H
#define SYNTHESISER_H

#include "tim.h"
#include "gpiopin.h"

#define FREQ(period) (1.0f / period)
#define PERIOD(freq) (1.0f / freq)

// Note frequency definitions for lowest octave
#define NOTE_C       16.35f
#define NOTE_Cs      17.33f
#define NOTE_Df      NOTE_Cs
#define NOTE_D       18.36f
#define NOTE_Ds      19.45f
#define NOTE_Ef      NOTE_Ds
#define NOTE_E       20.60f
#define NOTE_Ff      NOTE_E
#define NOTE_F       21.83f
#define NOTE_FS      23.13f
#define NOTE_Gf      NOTE_FS
#define NOTE_G       24.50f
#define NOTE_Gs      25.96f
#define NOTE_Af      NOTE_Gs
#define NOTE_A       27.50f
#define NOTE_As      29.14f
#define NOTE_Bf      NOTE_As
#define NOTE_B       30.87f
#define NOTE_REST    0.0f

/**
 * @brief
 * @details
 */
typedef enum {
  BEAT_SIXTEENTH = 1, //!<
  BEAT_EIGHTH    = 2, //!<
  BEAT_QUARTER   = 4, //!<
} NoteTime;

/**
 * @brief
 * @details
 */
typedef struct {
  float tone;     //!< Period of note's waveform
  float beat;     //!< Number of beats to play
  uint8_t octave; //!< Octave to be played
} Note;

/**
 * @brief
 * @details
 */
typedef struct {
  Note *notes;          //!< User supplied array of note structs to be played
  int8_t phase;         //!< Value indicating positive/negative cycle of voice
  uint8_t noteCount;    //!< Number of notes held in note array
  uint8_t noteIndex;    //!< Current note to be played
  float beat;           //!< Beat count for current note
  uint32_t accumulator; //!< Phase accumulator for voice synthesis
} Voice;

/**
 * @brief
 * @details
 */
typedef struct {
  TIM_TypeDef *timerInterface; //!<
  TIM_Channel timerChannel;    //!<
  GPIO_TypeDef *buzzerPort;    //!<
  GPIO_Pin buzzerPin;          //!<
  uint32_t sampleRate;         //!<
  uint16_t tempo;              //!<
  NoteTime beat;               //!<
} Synthesiser_Config;

/**
 * @brief
 * @details
 */
typedef struct Synthesiser {
  Synthesiser_Config config; //!<
  GPIOpin_t buzzer;          //!<
  TIM_t sampleTimer;         //!<
  void (*play)(struct Synthesiser *, Voice *, uint8_t);
  void (*updateConfig)(struct Synthesiser *, Synthesiser_Config *);
} Synthesiser_t;

void synthTimerPWMInterrupt();
void synthTimerUpdateInterrupt();

Synthesiser_t Synthesiser_init(Synthesiser_Config *config);
void Synthesiser_play(Synthesiser_t *synth, Voice *voices, uint8_t voiceCount);
void Synthesiser_updateConfig(Synthesiser_t *synth, Synthesiser_Config *config);

#endif
