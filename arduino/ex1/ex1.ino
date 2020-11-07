#include <SoftwareSerial.h>
SoftwareSerial sim(12, 14);

String number = "+639555730503"; 

//note wait the red blink in gsm will
//3 secsduration

const int us_trig_pin = 16;
const int us_echo_pin = 5;

const int led_green = 4;
const int led_red = 0;
const int led_blue = 2;

const int buzzer = 13;

const int wl_a_pin = A0;
bool isBlink = false;
bool isSend = false;
bool isUSDanger = false;
bool isWlDanger = false;
void setup() {
  Serial.begin(9600);
  Serial.println("Wait few seconds...");
  delay(5000);
  Serial.println("Sistem Started...");
  sim.begin(9600);
  delay(1000);
  
//  US config
  pinMode(us_trig_pin,OUTPUT);
  pinMode(us_echo_pin,INPUT);
  
//  pinMode(led_green,OUTPUT);
  pinMode(led_red,OUTPUT);
//  pinMode(led_blue,OUTPUT);
  
  pinMode(buzzer,OUTPUT); 
  digitalWrite(led_green, HIGH);
}

void loop() { 
  isUSDanger = false;
  isWlDanger = false;
  
  calUs();
  calWl(); 
  if(isWlDanger && isUSDanger){
    danger();
  }else{
    normal();
  }
  if (sim.available() > 0)
    Serial.write(sim.read());
  delay(1000);
}

void calUs(){
  //4 - 50 above that is error
  long duration;
  int distance;
  // Clears the trigPin
  digitalWrite(us_trig_pin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(us_trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(us_trig_pin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(us_echo_pin, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  if(distance <= 15){
    isUSDanger = true;
  }
}

void calWl(){
  int resval = analogRead(wl_a_pin);
  if(resval <= 100){
    Serial.println("Wl : empty"); 
    isWlDanger = false;
  }else if(resval > 100 && resval <= 300){
    Serial.println("Wl : low"); 
    isWlDanger = false;
  }else if(resval > 300 && resval <= 330){
    Serial.println("Wl : medium"); 
   isWlDanger = false;
  }else if(resval > 330){
    Serial.println("Wl : high");
    isWlDanger = true;
  }
  Serial.println(resval);
}

void normal(){
  isBlink = false;
  isSend = false;
  digitalWrite(led_red, HIGH);
  digitalWrite(led_blue, LOW);
  digitalWrite(buzzer, LOW);
}

void danger(){
  if(isBlink){
    digitalWrite(led_red, LOW);
    digitalWrite(led_blue, HIGH);
  }else{ 
    digitalWrite(led_blue, LOW);
    digitalWrite(led_red, HIGH);
  }
  isBlink = !isBlink;
  digitalWrite(buzzer, HIGH);
  if(!isSend){
    notifyMsg();
  }
}
void notifyMsg()
{
  Serial.println("NOTIFICATION SENT");
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  String SMS = "HIGH FLOOD DETECTED";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);
  delay(1000);
  isSend = true;
}
