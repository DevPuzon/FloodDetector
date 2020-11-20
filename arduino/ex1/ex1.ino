#include <SoftwareSerial.h>
SoftwareSerial sim(12, 14);
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "prototypeproject-eeb91.firebaseio.com"
#define FIREBASE_AUTH "BImIm9llfvfendC2Z1IgjbU2yMNMr48cwswG2n8f"
#define WIFI_SSID "TP-LINK_2D36" //name
#define WIFI_PASSWORD "09107809998" //pass

String number = "+639555730503"; 

//note wait the red blink in gsm will
//3 secsduration

const int us_trig_pin = 16;
const int us_echo_pin = 5;
 
const int led_red = 0; 

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
   
  pinMode(led_red,OUTPUT); 
  
  pinMode(buzzer,OUTPUT);  
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.print("Starting ");
    for(int i = 0; i < 10; i++){
      Serial.print(".");
      digitalWrite(led_red,LOW);
      delay(500);
      digitalWrite(led_red,HIGH);
      delay(1000);
      }
      
  Serial.print("started "); 
  delay(2000);
}

void loop() { 
  isUSDanger = false;
  isWlDanger = false;
  
  calUs();
  calWl(); 
  if(isWlDanger && isUSDanger){
    danger();
    Firebase.setFloat("isdanger", true);
  }else{
    normal();
    Firebase.setFloat("isdanger", false);
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
  Firebase.setFloat("distance", distance); 
}

void calWl(){
  int resval = analogRead(wl_a_pin);
  if(resval <= 100){
    Serial.println("Wl : empty"); 
    isWlDanger = false;
  }else if(resval > 100 && resval <= 300){
    Serial.println("Wl : low");
    isWlDanger = true;
  }else if(resval > 300 && resval <= 330){
    Serial.println("Wl : medium"); 
    isWlDanger = true;
  }else if(resval > 330){
    Serial.println("Wl : high");
    isWlDanger = true;
  }
  Firebase.setFloat("water_level", resval); 
  Serial.println(resval);
}

void normal(){
  isBlink = false;
  isSend = false;
  digitalWrite(led_red, HIGH); 
  digitalWrite(buzzer, LOW); 
}

void danger(){
  if(isBlink){
    digitalWrite(led_red, LOW); 
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
