SRC = play.c
WAV_FILE = test.wav

play.o: waveforms.h
waveforms.h: waveforms.py $(WAV_FILE)
	./waveforms.py $(WAV_FILE) > $@

clean::
	rm -f waveforms.h

include ../common.mk
