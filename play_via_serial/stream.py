#!/usr/bin/env python
import sys, struct, wave, time
import serial

LEN_SEC = 60.0
VOLUME = 1.0

# should be in sync with serial.c
SAMP_RATE = 48000
PWM_COEF = 1
DOWNSAMPLE_COEF = 4
CHUNK_LEN = 128

f = wave.open(sys.argv[1])
n_chan, _, _,  n_samp_tot, _, _ = f.getparams()
assert(f.getframerate() == SAMP_RATE)
n_samp = int(LEN_SEC * SAMP_RATE)
samples = f.readframes(n_samp)
samples = struct.unpack_from('%sh'%(min(n_samp, n_samp_tot) * n_chan), samples)[0::n_chan]
samples = samples[0::DOWNSAMPLE_COEF]
min_in, max_in = min(samples), max(samples)
max_out = min(2**8 - 1, 20000000/(SAMP_RATE * PWM_COEF))
samples = [int(VOLUME * (s - min_in) * max_out / (max_in - min_in)) for s in samples]

def pad(str, size):
    if len(str) > size:
        return str[:size]
    else:
        return str + '\0' * (size-len(str))

chunk_dur = (1.0 * CHUNK_LEN * DOWNSAMPLE_COEF)/SAMP_RATE
port = serial.Serial(sys.argv[2], 115200, timeout=2*chunk_dur)
t_begin = time.time()
for i in range(0, len(samples), CHUNK_LEN):
    chunk = samples[i : i+CHUNK_LEN]
    chunk = struct.pack('%sB'%len(chunk), *chunk)
    print 'writing', len(chunk)
    port.write(pad(chunk, CHUNK_LEN))
    ack = port.read()
    t_end = time.time()
    print 'got ack:', ack or 'timed out!', t_end - t_begin
    t_begin = t_end

print 'transmitted %s samples' % len(samples)
