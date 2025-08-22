/**************************************************************************************************
 * @file        synthesiser.c                                                                       *
 * @author      Matt Ricci                                                                        *
 * @brief       Brief description of the file's purpose.                                          *
 *                                                                                                *
 * @{                                                                                             *
 **************************************************************************************************/

#include "synthesiser.h"

#include "tim.h"
#include "gpiopin.h"

#include "math.h"
#include "stdbool.h"
#include "stddef.h"

static void _Synthesiser_init(Synthesiser_t *, Synthesiser_Config *);

// Single instance of synthesiser
static Synthesiser_t singleton;
static bool initialised = false;

// NOTE:
// Updated inside timer interrupt
volatile static bool sample = false;

void synthTimerPWMInterrupt() {
  singleton.buzzer.port->BSRR          = 0x01 << singleton.buzzer.pin;
  singleton.sampleTimer.interface->SR &= ~TIM_SR_CC1IF;
}

void synthTimerUpdateInterrupt() {
  sample                               = true;
  singleton.buzzer.port->BSRR          = (0x01 << singleton.buzzer.pin) << 16;
  singleton.sampleTimer.interface->SR &= ~TIM_SR_UIF;
}

/* ============================================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * ============================================================================================== */
Synthesiser_t Synthesiser_init(Synthesiser_Config *config) {

  if (!initialised) {
    singleton.play         = Synthesiser_play;
    singleton.updateConfig = Synthesiser_updateConfig;
    initialised            = true;
  }

  singleton.updateConfig(&singleton, config);
  return singleton;
}

// ALLOW FORMATTING
#ifndef __DOXYGEN__

/* =============================================================================== */
/**
 * @brief   Private initialiser for Synthesiser struct.
 *
 * @param   interface
 * @param   config
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
static void _Synthesiser_init(Synthesiser_t *synth, Synthesiser_Config *config) {
  // Initialise GPIO for output
  synth->buzzer          = GPIOpin_init(config->buzzerPort, config->buzzerPin, NULL);

  TIM_Config timerConfig = TIM_CONFIG_DEFAULT;
  timerConfig.OC[0].PE   = true; // Enable CCM preload
  timerConfig.CCIE[0]    = true; // Enable CCM interrupt
  timerConfig.UIE        = true; // Enable update interrupt
  timerConfig.ARPE       = true; // Enable auto-reload/preload
  synth->sampleTimer     = TIM_init(config->timerInterface, &timerConfig);

  // Set timing period and PWM configuration, default to 50% duty cycle
  synth->sampleTimer.setTimingPeriod(&synth->sampleTimer, (PERIOD(config->sampleRate)));
  synth->sampleTimer.setTimingPWM(&synth->sampleTimer, config->timerChannel, 50.0f);
}

#endif

/* =============================================================================== */
/**
 * @brief
 * @details
 *
 * @param
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
void Synthesiser_play(Synthesiser_t *synth, Voice *voices, uint8_t voiceCount) {
  synth->sampleTimer.startCounter(&synth->sampleTimer);

  // BPM is scaled by beat, BEAT_SIXTEENTH being the base
  uint32_t bpm = synth->config.tempo * synth->config.beat;
  // The increment defines the percentage of time that each
  // sample period contributes to a single beat.
  // On each sample interval, the beat count of all active
  // voices are incremented by this value to indicate how
  // many beats have passed for the current note.
  float beatInterval   = PERIOD((bpm / 60.0f));
  float sampleInterval = PERIOD(synth->config.sampleRate);
  float beatIncrement  = sampleInterval / beatInterval;

  uint8_t count        = 0;

  uint8_t activeVoices = voiceCount;

  GPIOpin_init(GPIOA, GPIO_PIN1, NULL);

  while (activeVoices) {

    // Wait for sample period
    while (!sample);
    sample = false;

    // Phase of output waveform
    float phase = 0;

    // Iterate each voice and accumulate phase on
    // sample interval
    for (uint8_t i = 0; i < voiceCount; i++) {
      Note currentNote = voices[i].notes[voices[i].noteIndex];

      // Output frequency is doubled to accomodate a full-phase period
      // F_out = (2^octave * tone) * 2
      float outFreq = ((1 << currentNote.octave) * currentNote.tone) * 2;
      float inFreq  = synth->config.sampleRate;

      // Calculate accumulator based on current note frequency
      uint32_t increment      = ((outFreq / inFreq) * 0x100000000);
      uint32_t newAccumulator = (voices[i].accumulator + increment);

      // Toggle phase on accumulator overflow and add to output phase
      if (newAccumulator < voices[i].accumulator) {
        voices[i].phase ^= 1;
      }
      phase += voices[i].phase;

      // Update voice accumulator
      voices[i].accumulator = newAccumulator;

      // Increment note index on beat overflow
      if ((voices[i].beat += beatIncrement) >= currentNote.beat) {
        voices[i].noteIndex++;
        voices[i].beat = 0;
      }

      // Remove voice from active count if finished
      if (voices[i].noteIndex == voices[i].noteCount)
        activeVoices -= 1;
    }

    phase /= voiceCount;

    // Clamp phase to avoid inaudible
    // output due to bad duty cycle
    if (phase == 0)
      phase += 0.01;
    if (phase == 1)
      phase -= 0.01;

    // Update phase from new samples
    synth->sampleTimer.setTimingPWM(
      &synth->sampleTimer,
      synth->config.timerChannel,
      phase * 100
    );
  }
}

/* =============================================================================== */
/**
 * @brief
 * @details
 *
 * @param
 *
 * @return  @c NULL.
 **
 * =============================================================================== */
void Synthesiser_updateConfig(Synthesiser_t *synth, Synthesiser_Config *config) {
  // Initialise config with default values if passed NULL.
  if (config == NULL) {
    config = NULL;
  }

  // Update peripheral with new config
  synth->config = *config;

  // Initialise TIM registers and enable peripheral
  _Synthesiser_init(synth, config);
}

/** @} */
