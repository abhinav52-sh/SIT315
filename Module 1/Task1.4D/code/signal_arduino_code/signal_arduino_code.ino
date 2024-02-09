/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Unit: SIT315 Programming Paradigms
* Task: M1_T4D
*/

#define signalPin 7

// constant variable to start timer1 counter from
const uint16_t timer1Init = 0;
// constant variable to store value (500 ms) used for comparing with the timer1 counter's value
const uint16_t timer1Compare = 31250;

// boolean variable to store current state of the signal
bool signalState = 0;

void setup()
{
  pinMode(signalPin, OUTPUT);
  
  // Resetting Timer1 Control Resister A
  TCCR1A = 0;
 
  // Setting prescaler to 256 (changing CS12 bit to 1)
  TCCR1B = B00000100;
  
  // Reset timer counter
  TCNT1 = timer1Init;
  OCR1A = timer1Compare;
  
  // Enabling Timer1 compare interrupt by changing OCIE1A to 1
  TIMSK1 = (1 << OCIE1A);
}

void loop()
{
 delay(500);
}

// ISR activates when the value in counter register for timer1
// is equal to 1st compare register for timer 1 
ISR(TIMER1_COMPA_vect)
{
  // resetting Timer1 counter to 0
  TCNT1 = timer1Init;
  // inverting signalState boolean
  signalState = !signalState;
  digitalWrite(signalPin, signalState);
}
