#include "waveforms.h"

#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#include <pololu/pushbuttons.h>
#include <pololu/lcd.h>
#include <pololu/analog.h>
#include <avr/io.h>
#include <avr/interrupt.h>

static uint16_t i;

void print_splash() {
  clear();
  print_long(read_battery_millivolts());
  print("mV");  
}

ISR(TIMER1_OVF_vect) {
  uint16_t idx = i / (PWM_COEF * DOWNSAMPLE_COEF);
  if (idx >= sizeof(WAVE_TABLE)) {
    TIMSK1 = 0;
    print_splash();
    return;
  }
  
  OCR1B = (uint16_t)(pgm_read_byte(WAVE_TABLE + idx));
  i++; 
}

int main() {
  TIMSK1 = 0;
  TCCR1A = (1 << COM1B1) | (1 << WGM11) | (1 << WGM10);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);
  TCCR1C = 0;
  OCR1A = F_CPU/(SAMP_RATE * PWM_COEF);
  DDRB |= (1 << PORTB2);

  print_splash();
  while (1) {
    if (button_is_pressed(TOP_BUTTON)) {
      wait_for_button_release(TOP_BUTTON);
      clear();
      print("button pressed");
    
      i = 0;
      TIMSK1 = (1 << TOIE1);
      sei();
    }
  }
}
