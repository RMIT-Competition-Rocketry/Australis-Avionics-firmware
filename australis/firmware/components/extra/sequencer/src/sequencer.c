/**************************************************************************************************
 * @file        sequencer.c                                                                       *
 * @author      Matt Ricci                                                                        *
 * @brief       Brief description of the file's purpose.                                          *
 *                                                                                                *
 * @{                                                                                             *
 **************************************************************************************************/

#include "sequencer.h"

#include "tim.h"
#include "gpiopin.h"

#include "math.h"
#include "stddef.h"

static void _Sequencer_playNote(Sequencer_t *sequencer, Note note, uint8_t volume);

/* ============================================================================================== */
/**
 * @brief
 *
 * @param
 *
 * @return
 **
 * ============================================================================================== */
Sequencer_t Sequencer_init(
  TIM_TypeDef *noteTimer,
  TIM_TypeDef *beatTimer,
  GPIO_TypeDef *buzzerPort,
  uint8_t buzzerPin
) {

  Sequencer_t sequencer;

  TIM_Config timConfig = TIM_CONFIG_DEFAULT;
  timConfig.OPM        = true;
  sequencer.noteTimer  = TIM_init(noteTimer, &timConfig);
  sequencer.beatTimer  = TIM_init(beatTimer, &timConfig);

  sequencer.buzzer     = GPIOpin_init(buzzerPort, buzzerPin, NULL);

  return sequencer;
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
static void _Sequencer_playNote(Sequencer_t *sequencer, Note note, uint8_t volume) {
  // TODO: Update this to use PWM for volume control. For now, ignore volume.
  sequencer->noteTimer.setTimingPeriod(&sequencer->noteTimer, 1 / (pow(2, note.octave) * note.tone));

  sequencer->beatTimer.startCounter(&sequencer->beatTimer);
  sequencer->noteTimer.startCounter(&sequencer->noteTimer);

  for (uint16_t beats = 0; beats < note.beat;) {
    // Wait for tone period to expire before toggling buzzer output
    if (sequencer->noteTimer.pollUpdate(&sequencer->noteTimer)) {
      sequencer->buzzer.toggle(&sequencer->buzzer);
      sequencer->noteTimer.startCounter(&sequencer->noteTimer);
    }

    // Restart timer and increment beat count if beat timer
    // has expired
    if (sequencer->beatTimer.pollUpdate(&sequencer->beatTimer)) {
      sequencer->beatTimer.startCounter(&sequencer->beatTimer);
      beats++;
    }
  }
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
void Sequencer_play(
  Sequencer_t *sequencer,
  Note *notes,
  uint16_t noteCount,
  uint16_t bpm,
  uint8_t volume
) {
  // Update sequencer BPM setting
  sequencer->bpm = bpm;

  // Set beat timer with period from initial bpm
  sequencer->beatTimer.setTimingPeriod(&sequencer->beatTimer, 1.0f / (bpm / 60.0f));

  // Play entire note sequence
  for (uint32_t i = 0; i < noteCount; i++) {
    _Sequencer_playNote(sequencer, notes[i], volume);
  }
}

/** @} */
