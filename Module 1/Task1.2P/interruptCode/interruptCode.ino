/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Unit: SIT315 Programming Paradigms
* Task: M1_T2P
*/

// pin no. for hardware devices
#define led_out 7
#define pir_in 2

// global variable
volatile bool detected;

void setup()
{
  pinMode(led_out, OUTPUT);
  pinMode(pir_in, INPUT);
  attachInterrupt(digitalPinToInterrupt(pir_in), motionDetected, CHANGE);
  Serial.begin(9600);         // enabling serial communication
  detected = digitalRead(pir_in);
}

void loop()
{  
  // printing current status of system
  if(detected) Serial.println("Motion Detected     | LED: ON");
  else  Serial.println("Motion NOT Detected | LED: OFF");
}

void motionDetected()
{
  detected = !detected;
  digitalWrite(led_out, detected); 	
}
      
