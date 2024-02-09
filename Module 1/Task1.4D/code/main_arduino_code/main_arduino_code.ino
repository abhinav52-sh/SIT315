
/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Unit: SIT315 Programming Paradigms
* Task: M1_T4D
*/

// Note: Sensors used - 
//   1. PIR sensor
//   2. Photoresistor along with 0.7 kohm resistor to read a digital value
//   3. Another arduino sending digital signal at 0.5 second interval using terminals.

// pin no. for hardware devices
#define led1 11
#define led2 12
#define led3 13
#define pir 2
#define photoresistor 3
#define arduinoSignal 4

// global variable
volatile bool pirDetected;
volatile bool presDetected;
volatile bool ardSignalDetected;

void setup()
{
  // defining pins' interaction modes
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(photoresistor, INPUT);
  pinMode(arduinoSignal, INPUT);

  // setting up interrupts
  
  // enabling interrupts for PORT B (D0-D7)
  PCICR |= B00000100;
  // enabling interrupts for pins D2, D3, D4
  PCMSK2 |= B00011100;
  
  // enabling serial communication
  Serial.begin(9600);  
  
  // setting up initial state of the led state variables
  pirDetected = digitalRead(pir);
  presDetected = digitalRead(photoresistor);
  ardSignalDetected = digitalRead(arduinoSignal);
}

// main loop
void loop()
{  
  printStatus();
  glowLEDs();
}

// Printing status of the leds on the serial monitor
void printStatus()
{
  String systemStatus = "";
  systemStatus = "PIR: " + String(pirDetected) + "\t\tPhoto: " + String(presDetected) + "\tArdTimer: " + String(ardSignalDetected);
  Serial.println(systemStatus);
}

// function to change state of leds to their respective boolean state variables 
void glowLEDs()
{
  digitalWrite(led1, pirDetected);
  digitalWrite(led2, presDetected);
  digitalWrite(led3, ardSignalDetected);
}

// ISR when any change is detected on the connected hardware's signal/data pin
ISR(PCINT2_vect)
{
  pirDetected = (PIND & B00000100) ? HIGH : LOW;
  presDetected = (PIND & B00001000) ? HIGH : LOW;
  ardSignalDetected = (PIND & B00010000) ? HIGH : LOW;
}
      
