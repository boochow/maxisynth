/**
 *  @file header.c
 *  @brief drumlogue SDK unit header
 *
 *  Copyright (c) 2020-2022 KORG Inc. All rights reserved.
 *
 */

#include "unit.h"  // Note: Include common definitions for all units

// ---- Unit header definition  --------------------------------------------------------------------

const __unit_header unit_header_t unit_header = {
    .header_size = sizeof(unit_header_t),                  // leave as is, size of this header
    .target = UNIT_TARGET_PLATFORM | k_unit_module_synth,  // target platform and module for this unit
    .api = UNIT_API_VERSION,                               // logue sdk API version against which unit was built
    .dev_id = 0x0U,                                        // developer identifier
    .unit_id = 0x0U,                                       // Id for this unit, should be unique within the scope of a given dev_id
    .version = 0x00010000U,                                // This unit's version: major.minor.patch (major<<16 minor<<8 patch).
    .name = "maxisynth",                                   // Name for this unit, will be displayed on device
    .num_presets = 0,                                      // Number of internal presets this unit has
    .num_params = 12,                                      // Number of parameters for this unit, max 24
    .params = {
        // Format: min, max, center, default, type, fractional, frac. type, <reserved>, name

        // See common/runtime.h for type enum and unit_param_t structure

        // Page 1
        {0, 127, 60, 60, k_unit_param_type_midi_note, 0, 0, 0, {"Note"}},
        {0, 2, 0, 0, k_unit_param_type_none, 0, 0, 0, {"Wave"}},
        {0, 1270, 0, 1270, k_unit_param_type_none, 1, 1, 0, {"Cutoff"}},
        {-128, 128, 0, -16, k_unit_param_type_none, 5, 0, 0, {"Reso"}},

        // Page 2
        {0, 1000, 0, 0, k_unit_param_type_msec, 0, 0, 0, {"Attack"}},
        {0, 1000, 0, 0, k_unit_param_type_msec, 0, 0, 0, {"Decay"}},
        {0, 100, 0, 100, k_unit_param_type_percent, 0, 0, 0, {"Sustain"}},
        {0, 1000, 0, 0, k_unit_param_type_msec, 0, 0, 0, {"Release"}},

        // Page 3
        {0, 100, 0, 100, k_unit_param_type_percent, 0, 0, 0, {"EG >AMP"}},
        {-100, 100, 0, 0, k_unit_param_type_percent, 0, 0, 0, {">Pitch"}},
        {-100, 100, 0, 0, k_unit_param_type_percent, 0, 0, 0, {">Cutoff"}},
        {0, 100, 0, 0, k_unit_param_type_percent, 0, 0, 0, {">PWM"}},

        // Page 4
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 5
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},

        // Page 6
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}},
        {0, 0, 0, 0, k_unit_param_type_none, 0, 0, 0, {""}}}};
