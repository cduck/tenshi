// Licensed to Pioneers in Engineering under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Pioneers in Engineering licenses
// this file to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
//  with the License.  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License

#include "inc/encoder.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "inc/pindef.h"


DECLARE_I2C_REGISTER_C(int32_t, encoder_count);
DECLARE_I2C_REGISTER_C(FIXED1616, encoder_speed_precise);

void init_encoder() {
  DDR(PINDEF_ENCA) &= ~(_BV(IO(PINDEF_ENCA)) | _BV(IO(PINDEF_ENCB)));
  // Init is called before interrupts are enabled.
  set_encoder_count_dangerous(0);
  set_encoder_speed_precise_dangerous(0);
  // Set up interrupt.
  PCMSK0 = _BV(PCINT0) | _BV(PCINT4);
  // Just in case.
  PCIFR = _BV(PCIF0);
  // Enable.
  PCICR = _BV(PCIE0);

  init_encoder_timer();
}

// Using timer 1
void init_encoder_timer() {
  // Turn on timer 1
  PRR0 |= _BV(PRTIM1);

  // Diconnect output pins
  TCCR1A = 0;

  // Prescale by 64
  // Tick frequency: 16MHz/64 = 250kHz
  // or 4 ticks per micro second
  // or 256 ticks per control loop
  // Timer will overflow every 256 control loops
  // or every 0.262144 seconds
  TCCR4B = _BV(CS11) | _BV(CS10);

  // Clear count
  TCNT1 = 0;

  // Set compare a some time less than 0.262144 seconds if needed
  // OCR1A = 65536;

  // Enable overflow interrupt
  TIMSK1 = _BV(TOIE1);
  // Enable compare a interrupt if needed
  // TIMSK1 |= _BV(OCIE1A);
}

// Indexed by (old_state << 2) | (new_state).
// Zero entries are somehow invalid (two transition or no transitions).
// When going forward, the signals look like:
//  a ___|‾‾‾|___|‾‾‾|___|‾‾‾|___
//  b   ___|‾‾‾|___|‾‾‾|___|‾‾‾|___
// Interrupts happen AFTER transitions, so the "forward" transitions are:
//   00 to 10, 10 to 11, 11 to 01, and 01 to 00
// When going backwards, the signals look like:
//   a   ___|‾‾‾|___|‾‾‾|___|‾‾‾|___
//   b ___|‾‾‾|___|‾‾‾|___|‾‾‾|___
// Interrupts happen AFTER transitions, so the "backward" transitions are:
//  00 to 01, 01 to 11, 11 to 10, and 10 to 00
const signed char encoder_transition_table[] = {
  0,
  -1,
  1,
  0,
  1,
  0,
  0,
  -1,
  -1,
  0,
  0,
  1,
  0,
  1,
  -1,
  0,
};

// Interrupt for encoder processing
// This gets called every time one of pin A or pin B changes
// (but you don't know which).
ISR(PCINT0_vect) {
  uint16_t time = TCNT1;
  TCNT1 = 0;  // Clear count
  // 256 as a fixed point number divided by time as an integer
  FIXED1616 speed = 0x1000 / time;  // Units: encoder ticks per control loop
  set_encoder_speed_precise_dangerous(speed);

  unsigned char ioport_copy = PIN(PINDEF_ENCA);
  static unsigned char old_state = 0;
  unsigned char new_state = 0;
  if (ioport_copy & _BV(IO(PINDEF_ENCA)))
    new_state |= 0b10;
  if (ioport_copy & _BV(IO(PINDEF_ENCB)))
    new_state |= 0b01;
  set_encoder_count_dangerous(get_encoder_count_dangerous() +
      encoder_transition_table[old_state << 2 | new_state]);
  old_state = new_state;
}

// Interrupt for precise speed timeout if speed is near zero
ISR(TIMER1_OVF_vect) {
  // Precise speed timer has timed out
  // Set speed measurement to zero
  set_encoder_speed_precise_dangerous(0);

  // Clear count
  TCNT1 = 0;
}
ISR(TIMER1_COMPA_vect, ISR_ALIASOF(TIMER1_OVF_vect));
