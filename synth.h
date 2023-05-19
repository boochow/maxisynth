#pragma once
/*
 *  File: synth.h
 *
 *  Simple Synth using Maximilian
 *
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <algorithm>

#include <arm_neon.h>

#include "unit.h"  // Note: Include common definitions for all units

#include "maximilian.h"
#include "libs/maxiPolyBLEP.h"

// Uncomment this line to use maxiEnvGen class
// #define USE_MAXIENVGEN

// Use this instead of mtof() in maxmilian to allow note number in float
inline double note2freq(float note) {
    return (440.f / 32) * powf(2, (note - 9.0) / 12);
}

enum Params {
    Note = 0,
    Waveform,
    Cutoff,
    Resonance,
    Attack,
    Decay,
    Sustain,
    Release,
    EnvAmpInt,
    EnvPitchInt,
    EnvCutoffInt,
    EnvPWMInt
};

class Synth {
 public:
  /*===========================================================================*/
  /* Public Data Structures/Types. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Lifecycle Methods. */
  /*===========================================================================*/

  Synth(void) {}
  ~Synth(void) {}

  inline int8_t Init(const unit_runtime_desc_t * desc) {
    // Check compatibility of samplerate with unit, for drumlogue should be 48000
    if (desc->samplerate != 48000)
      return k_unit_err_samplerate;

    // Check compatibility of frame geometry
    if (desc->output_channels != 2)  // should be stereo output
      return k_unit_err_geometry;

    maxiSettings::sampleRate = 48000;
    oscillator_.setSampleRate(48000);
#ifdef USE_MAXIENVGEN
    envelope_.setup({0, 1, 1, 1, 0, 0},
                    {1, 1, maxiEnvGen::HOLD, 1, 1},
                    {1, 1, 1, 1, 1},
                    false, true);
#endif
    // Note: if need to allocate some memory can do it here and return k_unit_err_memory if getting allocation errors

    return k_unit_err_none;
  }

  inline void Teardown() {
    // Note: cleanup and release resources if any
  }

  inline void Reset() {
    // Note: Reset synth state. I.e.: Clear filter memory, reset oscillator
    // phase etc.
    oscillator_.setWaveform(maxiPolyBLEP::Waveform::SAWTOOTH);
    gate_ = 0;
  }

  inline void Resume() {
    // Note: Synth will resume and exit suspend state. Usually means the synth
    // was selected and the render callback will be called again
  }

  inline void Suspend() {
    // Note: Synth will enter suspend state. Usually means another synth was
    // selected and thus the render callback will not be called
  }

  /*===========================================================================*/
  /* Other Public Methods. */
  /*===========================================================================*/

  fast_inline void Render(float * out, size_t frames) {
    float * __restrict out_p = out;
    const float * out_e = out_p + (frames << 1);  // assuming stereo output

#ifdef USE_MAXIENVGEN
    const float trigger = gate_ ? 1.0 : -1.0;
#else
    const int trigger = (gate_ > 0);
#endif

    for (; out_p != out_e; out_p += 2) {
      // Envelope
#ifdef USE_MAXIENVGEN
      float env = envelope_.play(trigger);
#else
      float env = envelope_.adsr(1.0, trigger);
#endif
      // Oscillator
      float pitch = note2freq(note_ + egPitch_ * env);
      oscillator_.setPulseWidth(0.5f + egPwm_ * env);
      float sig = oscillator_.play(pitch);
      // Filter
      double cutoff_note = note_ + cutoffOffset_ + egCutoff_ * env;
      float cutoff = min(23999., note2freq(max(0., cutoff_note)));
      filter_.set(maxiBiquad::LOWPASS, cutoff, resonance_, 2.);
      sig = filter_.play(sig * amp_);
      // Amplifier
      sig = sig * (egAmp_ * env + egAmpRev_ * trigger);
      // Note: should take advantage of NEON ArmV7 instructions
      vst1_f32(out_p, vdup_n_f32(sig));
    }
  }

  inline void setParameter(uint8_t index, int32_t value) {
    p_[index] = value;
    switch (index) {
    case Note:
      note_ = value;
      break;
    case Waveform:
      if (value == 2) {
        oscillator_.setWaveform(maxiPolyBLEP::Waveform::TRIANGLE);
      } else if (value == 1) {
        oscillator_.setWaveform(maxiPolyBLEP::Waveform::RECTANGLE);
      } else {
        oscillator_.setWaveform(maxiPolyBLEP::Waveform::SAWTOOTH);
      }
      break;
    case Cutoff:
      cutoffOffset_ = 0.1 * value - 63.5; // -63.5 .. +63.5
      break;
    case Resonance:
      resonance_ = powf(2, 1.f / (1<<5) * value); // 2^(-4) .. 2^4
      break;
#ifdef USE_MAXIENVGEN
    case Attack:
      envelope_.setTime(0, value + 1);
      break;
    case Decay:
      envelope_.setTime(1, value + 1);
      break;
    case Sustain:
      envelope_.setLevel(2, 0.01 * value);
      envelope_.setLevel(3, 0.01 * value);
      break;
    case Release:
      envelope_.setTime(3, value + 1);
      break;
#else
    case Attack:
      envelope_.setAttackMS(value + 1);
      break;
    case Decay:
      envelope_.setDecay(value + 1);
      break;
    case Sustain:
      envelope_.setSustain(0.01 * value);
      break;
    case Release:
      envelope_.setRelease(value + 1);
      break;
#endif
    case EnvAmpInt:
      egAmp_ = 0.01f * value;
      egAmpRev_ = 1.f - egAmp_;
      break;
    case EnvPitchInt:
      egPitch_ = 0.24f * value; // 24 semitones / 100%
      break;
    case EnvCutoffInt:
      egCutoff_ = 0.6 * value;  // 60 semitones / 100%
      break;
    case EnvPWMInt:
      egPwm_ = 0.0049 * value;  // 0.49 / 100%
      break;
    default:
      break;
    }
  }

  inline int32_t getParameterValue(uint8_t index) const {
    return p_[index];
  }

  inline const char * getParameterStrValue(uint8_t index, int32_t value) const {
    (void)value;
    switch (index) {
      // Note: String memory must be accessible even after function returned.
      //       It can be assumed that caller will have copied or used the string
      //       before the next call to getParameterStrValue
      default:
        break;
    }
    return nullptr;
  }

  inline const uint8_t * getParameterBmpValue(uint8_t index,
                                              int32_t value) const {
    (void)value;
    switch (index) {
      // Note: Bitmap memory must be accessible even after function returned.
      //       It can be assumed that caller will have copied or used the bitmap
      //       before the next call to getParameterBmpValue
      // Note: Not yet implemented upstream
      default:
        break;
    }
    return nullptr;
  }

  inline void NoteOn(uint8_t note, uint8_t velocity) {
    note_ = note;
    GateOn(velocity);
  }

  inline void NoteOff(uint8_t note) {
    (void)note;
    GateOff();
  }

  inline void GateOn(uint8_t velocity) {
    amp_ = 1. / 127 * velocity;
    gate_ += 1;
  }

  inline void GateOff() {
    if (gate_ > 0 ) {
      gate_ -= 1;
    }
  }

  inline void AllNoteOff() {}

  inline void PitchBend(uint16_t bend) { (void)bend; }

  inline void ChannelPressure(uint8_t pressure) { (void)pressure; }

  inline void Aftertouch(uint8_t note, uint8_t aftertouch) {
    (void)note;
    (void)aftertouch;
  }

  inline void LoadPreset(uint8_t idx) { (void)idx; }

  inline uint8_t getPresetIndex() const { return 0; }

  /*===========================================================================*/
  /* Static Members. */
  /*===========================================================================*/

  static inline const char * getPresetName(uint8_t idx) {
    (void)idx;
    // Note: String memory must be accessible even after function returned.
    //       It can be assumed that caller will have copied or used the string
    //       before the next call to getPresetName
    return nullptr;
  }

 private:
  /*===========================================================================*/
  /* Private Member Variables. */
  /*===========================================================================*/

  std::atomic_uint_fast32_t flags_;

  int32_t p_[24];
  maxiPolyBLEP oscillator_;
  maxiBiquad filter_;
#ifdef USE_MAXIENVGEN
  maxiEnvGen envelope_;
#else
  maxiEnv envelope_;
#endif

  int32_t note_;
  float amp_;
  uint32_t gate_;
  float cutoffOffset_;
  float resonance_;

  float egAmp_;
  float egAmpRev_;
  float egPitch_;
  float egCutoff_;
  float egPwm_;
  /*===========================================================================*/
  /* Private Methods. */
  /*===========================================================================*/

  /*===========================================================================*/
  /* Constants. */
  /*===========================================================================*/
};
