/*
* Name: Abhinav Sharma
* Student ID: 2210994752
* Unit: SIT315 Programming Paradigms
* Task: M1_T1P
*/

// pin no. for hardware devices
#define led_out 7
#define pir_in 2

// global variables
int detected;
int motionTime = 0;
int prevTime = 0;

void setup()
{
  pinMode(led_out, OUTPUT);
  pinMode(pir_in, INPUT);
  Serial.begin(9600);         // enabling serial communication
}

void loop()
{
  // setting flag value based on sensor input
  detected = digitalRead(2) ? 1 : 0;
 
  prevTime = millis();
  
  // switching on LED when motion is detected
  if(detected)
  {
    Serial.print("Motion Detected ");
    digitalWrite(led_out, 1);
    Serial.println("LED: ON");
    motionTime = millis() - prevTime;
  }
  
  // turning off the LED when there has been no motion for more than 50 milliseconds 
  if(!detected && motionTime < 50)
  {
    digitalWrite(led_out, 0);
    Serial.println("LED: OFF");
  }
}
      