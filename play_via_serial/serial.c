#include <pololu/serial.h>
#include <pololu/lcd.h>
#include <pololu/analog.h>
#include <pololu/pushbuttons.h>

#ifndef F_CPU
#define F_CPU 20000000UL
#endif


/* should be in sync with stream.py */
const uint32_t SAMP_RATE = 48000;
const uint8_t PWM_COEF = 1;
const uint8_t DOWNSAMPLE_COEF = 4;
#define CHUNK_LEN 128

char buf1[CHUNK_LEN];
char buf2[CHUNK_LEN];

char* volatile recv_buf = buf1;
char* volatile play_buf = buf2;
volatile char need_chunk = 1;

uint16_t pos = 0;

void print_splash() {
  clear();
  print_long(read_battery_millivolts());
  print("mV");
}

ISR(TIMER1_OVF_vect) {
  uint16_t idx = pos / (PWM_COEF * DOWNSAMPLE_COEF);
  if (idx >= CHUNK_LEN) {
    if (need_chunk) {
      TIMSK1 = 0;
      return;
    }

    char *tmp = play_buf;
    play_buf = recv_buf;
    recv_buf = tmp;
    pos = 0;

    need_chunk = 1;
    serial_send("a", 1);

    return;
  }
  OCR1B = (uint16_t)(uint8_t)play_buf[idx];
  pos++;
}

int main() {
  TIMSK1 = 0;
  TCCR1A = (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);
  TCCR1C = 0;
  OCR1A = F_CPU/44100;
  DDRB |= (1 << PORTB2);

  serial_set_baud_rate(115200);
  sei();

  print_splash();

  while (1) {
    while (!need_chunk);
    while (serial_receive_blocking(recv_buf, CHUNK_LEN, 100));
    need_chunk = 0;
    TIMSK1 = (1 << TOIE1);
  }
}
