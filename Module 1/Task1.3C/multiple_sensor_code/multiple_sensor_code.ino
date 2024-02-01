/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Unit: SIT315 Programming Paradigms
* Task: M1_T3C
*/

// Note: Sensors used - (PIR sensor) and (Photoresistor along with 0.7 kohm resistor to read a digital value)

// pin no. for hardware devices
#define led1 7
#define led2 8
#define pir 2
#define photoresistor 3

// global variable
volatile bool pirDetected;
volatile bool presDetected;

void setup()
{
  // defining pins' interaction modes
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(pir, INPUT);
  pinMode(photoresistor, INPUT);

  // setting up interrupts
  attachInterrupt(digitalPinToInterrupt(pir), pirChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(photoresistor),presChange, CHANGE);

  // enabling serial communication
  Serial.begin(9600);         
  pirDetected = digitalRead(pir);
  presDetected = digitalRead(photoresistor);
}

void loop()
{  
  // printing current status of system
  if(pirDetected) Serial.println("PIR Detected         | LED 1: ON");
  else  Serial.println("PIR NOT Detected     | LED 1: OFF");
  
  if(presDetected) Serial.println("Light Detected       | LED 2: ON");
  else  Serial.println("Light NOT Detected   | LED 2: OFF");
  
  delay(2000);
}

// ISR for PIR sensor 
void pirChange()
{
  pirDetected = !pirDetected;
  digitalWrite(led1, pirDetected); 	
}

// ISR for Photo Resistor
void presChange()
{
  presDetected = !presDetected;
  digitalWrite(led2, presDetected); 	
}
      
