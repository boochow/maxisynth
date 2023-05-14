# maxisynth: logue SDK 2.0 basic Synth unit using Maximilian library

This repository shows how to use the Maximilian library to develop a synth unit on the KORG logue SDK v2 for drumlogue.

Since logue SDK v2 currently provides only the APIs to communicate with the drumlogue, you must develop all components, such as oscillators, filters, and envelopes, to build your synth unit. You can reduce many tasks to implement these components with the Maximilian library.

## How to build

Since Maximilian currently has some minor issues to use with logue SDK, use the patched fork of Maximilian, which I am providing at:

https://github.com/boochow/Maximilian

Place these repositories under `logue-sdk/platforms/drumlogue/` like this:

```
drumlogue/
├── common
├── Maximilian
└── maxisynth
```

then type

`../../docker/run_cmd.sh build drumlogue/maxisynth`

and copy `maxisynth/maxisynth.drmlgunit` to your drumlogue.

The `maxisynth` repo has three branches:

* `main` : a complete synthesizer unit with one oscillator, one low-pass filter, one envelope generator, and  12 parameters

* `band-limited-osc` : a minimum synthesizer unit with one sawtooth oscillator and Note parameter

* `maximilian-synth-template` : a modified version of `dummy-synth` as a starting point to develop a new synth unit with Maximilian

  

## Block diagram

  ![block diagram](https://github.com/boochow/maxisynth/blob/images/maxisynth-blockdgm.png?raw=true)
