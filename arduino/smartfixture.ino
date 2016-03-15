//#define DEBUG
#include <Wire.h>
#include <SFE_BMP180.h>
#include <dht.h>
#define ALTITUDE 195
// Temp-humidity                                                                                                                                                                    
#define dht_apin A0
// photo resistor                                                                                                                                                                   
#define phr_apin A1

// PIR motion                                                                                                                                                                       
#define PIR 4
// small reed switch                                                                                                                                                                
#define small_reed_apin 6
// large reed switch                                                                                                                                                                
#define large_reed_apin 5
// IR/avoidance distance sensor                                                                                                                                                     
#define avoidance_pin A2
/* Actually, 13 is the onboard led. But it                                                                                                                                          
 * Turns out the device will be "busy" if something is connected                                                                                                                    
 * to pin 13. */
#define onboard_led 9
/* Rotary encoder pins                                                                                                                                                              
 * It's clear I don't know what I'm doing with this. In the sameple code,                                                                                                           
 * the CLK pin was completely ignored as it is here. It's probably key                                                                                                              
 * to not getting 5 readings. */
#define CLK 2
#define DAT 3
#define SW 4

/* This was origally here so the Arduberry could decide whether the                                                                                                                 
 * light is on. */
int light_threshold = 400;
int count = 0;
int seconds = 0;
int temp_read = 30;
int send_seconds = 15;
int reset = 30;

dht DHT;
SFE_BMP180 pressure;


struct weather_t {
  float temperature;
  float humidity;
  float pressure;
};

void printdata();

bool read_switch() {
#ifdef DEBUG
    Serial.println("Gonna read switch");
#endif
  return !digitalRead(small_reed_apin);
}

float read_light() {
#ifdef DEBUG
    Serial.println("Gonna read light level");
#endif
  return analogRead(phr_apin);
}

float read_presence() {
#ifdef DEBUG
    Serial.println("Gonna check for presence");
#endif
  return (analogRead(avoidance_pin) * 5.0) / 1024.0;
}

float read_pressure() {
#ifdef DEBUG
    Serial.println("Gonna get hPa");
#endif
  char status;
  double T, P, p0, a;
  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0) {
      status = pressure.startPressure(3);
      if (status != 0) {
        delay(status);
        status = pressure.getPressure(P, T);

        p0 = pressure.sealevel(P, ALTITUDE);
      }
    }
  }
  return p0; 
}

void read_weather() {
#ifdef DEBUG
    Serial.println("Gonna get weather");
#endif
  DHT.read11(dht_apin);
}

void light_on() {
  for (int i = 0; i < 255; i += 10) {
    analogWrite(onboard_led, i);
    delay(20);
  }
}

void light_off() {
  for (int i = 255; i > 0; i -= 10) {
    analogWrite(onboard_led, i);
    delay(20);
  }
}


void setup(){
  while (!Serial);
  Serial.println("Setting up.");
  pinMode(small_reed_apin, INPUT);
  // pinMode(large_reed_apin, INPUT);
  pinMode(avoidance_pin, INPUT);
  // pinMode(PIR, INPUT);
  pinMode(onboard_led, OUTPUT);
  Serial.begin(9600);
  pressure.begin();
  delay(1000);
  light_off();
  light_on();
}

void loop(){
  light_off();
  if (seconds % send_seconds == 0) {
#ifdef DEBUG
    Serial.println("Gonna print");
#endif
    printdata();
  }

  if (seconds % temp_read == 0) {
    seconds = 0;
  }
  
  light_on();
  seconds++;  
}

void wait() {
  for (int i = 0; i < 15; i++) {
    light_on();
    light_off();
  }
  light_on();
  delay(1000);
}

void printdata() {
  // millis:presence:lrs:srs:ll:hum:temp:lt:pir
  DHT.read11(dht_apin);
  Serial.print(millis()); Serial.print(":");
  Serial.print(read_presence(), DEC); Serial.print(":"); //0?                                                                                                                           
  Serial.print(0, DEC); Serial.print(":");
  Serial.print(read_switch(), DEC); Serial.print(":");
  Serial.print(read_light() * -1.0, DEC); Serial.print(":"); //0?                                                                                                                       
  Serial.print(DHT.humidity, DEC); Serial.print(":");
  float t = (9.0/5.0) * DHT.temperature + 32;
  Serial.print(t); Serial.print(":");
  Serial.print(read_pressure(), DEC); Serial.print(":"); //0?                                                                                                                               
  Serial.print(light_threshold, DEC); Serial.print(":");
  Serial.println(0);
}
