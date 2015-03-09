#!/usr/bin/env python
import sys, struct, wave

LEN_SEC = 1.0
VOLUME = 1.0

PWM_COEF = 2
DOWNSAMPLE_COEF = 4

#samp rate=44100, playback rate=11025, pwm rate=88200

f = wave.open(sys.argv[1])
n_chan, _, samp_rate,  n_samp_tot, _, _ = f.getparams()
n_samp = int(LEN_SEC * samp_rate)
samples = f.readframes(n_samp)
samples = struct.unpack_from('%sh'%(min(n_samp, n_samp_tot) * n_chan), samples)[0::n_chan]
samples = samples[0::DOWNSAMPLE_COEF]
min_in, max_in = min(samples), max(samples)
max_out = min(2**8, 20000000/(samp_rate * PWM_COEF))
samples = ','.join([str(int(VOLUME * (s - min_in) * max_out / (max_in - min_in))) for s in samples])
print """
#include <avr/pgmspace.h>
#include <stdint.h>
const uint32_t SAMP_RATE = {samp_rate};
const uint8_t PWM_COEF = {PWM_COEF};
const uint8_t DOWNSAMPLE_COEF = {DOWNSAMPLE_COEF};
const uint8_t WAVE_TABLE[] PROGMEM = {{ {samples} }};
""".format(**locals())
