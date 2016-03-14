#include <dht.h>

const int interruptA = 0;
// Temp-humidity
#define dht_apin A0
// photo resistor
#define phr_apin A1
// small reed switch
#define small_reed_apin 5
// large reed switch
#define large_reed_apin 6
// IR/avoidance distance sensor
#define avoidance_pin 7
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

const int temp_read = 30;
const int send_seconds = 15;
const int reset = 30;

/* This was origally here so the Arduberry could decide whether the 
 * light is on. */
int light_threshold = 400;
int count = 0;
int seconds = 0;
dht DHT;

/* So, if I just pass around pointers to an instance of this struct,
 * then it can be volitile-ish and I can read/print asynchronously.
 */
struct data_collection {
  double humidity;
  double temperature;
  double light_level;
  double presence;
  bool pir;
  bool small_reed_switch;
  bool large_reed_switch;
};


void readdata(struct data_collection* d);
void readDHT(struct data_collection* d, int seconds);
void read_light(struct data_collection* d, int seconds);
void read_presence(struct data_collection* d, int seconds);
void printdata(struct data_collection* d);
struct data_collection *dc;

void setup(){
  attachInterrupt(0, RoteStateChanged, FALLING);
  while (!Serial);
  pinMode(CLK, INPUT);
  digitalWrite(CLK, HIGH);  // Pull High Restance
  pinMode(DAT, INPUT);
  digitalWrite(DAT, HIGH);  // Pull High Restance
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);  // Pull High Restance
  
  pinMode(small_reed_apin, INPUT);
  pinMode(large_reed_apin, INPUT);
  pinMode(avoidance_pin, INPUT);
  pinMode(PIR, INPUT);
  pinMode(onboard_led, OUTPUT);
  Serial.begin(9600);
  delay(1000); // Wait before accessing Sensor
  readDHT(dc, 30); // First read
}

void loop(){
  if (!digitalRead(SW)) {
    count = 0;
    light_threshold = 400;
    delay(2000);
  }
  light_off();
  read_light(dc, seconds);
  read_presence(dc, seconds);
  readDHT(dc, seconds);
 
  readdata(dc);
  light_on();

  if (seconds % send_seconds == 0) {
    printdata(dc);
  }

  /* if (seconds % temp_read == 0) { */
  /*   seconds = 0; */
  /* } */
  
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

void printdata(struct data_collection *d) {
  // millis:presence:lrs:srs:ll:hum:temp:lt:pir
  Serial.print(millis()); Serial.print(":");
  Serial.print(d->presence, DEC); Serial.print(":");
  Serial.print(d->large_reed_switch); Serial.print(":");
  Serial.print(d->small_reed_switch); Serial.print(":");
  Serial.print(d->light_level * -1.0, DEC); Serial.print(":");
  Serial.print(d->humidity, DEC); Serial.print(":");
  Serial.print(d->temperature, DEC); Serial.print(":");
  Serial.print(light_threshold, DEC); Serial.print(":");
  Serial.println(d->pir, DEC);
}

void readdata(struct data_collection *d) {
  d->small_reed_switch = !digitalRead(small_reed_apin);
  d->large_reed_switch = digitalRead(large_reed_apin);
  d->presence = (analogRead(avoidance_pin) * 5.0) / 1024.0;
  d->pir = digitalRead(PIR);
}

void read_light(struct data_collection* d, int seconds) {
  if (seconds % reset != 0) {
    d->light_level = (d->light_level + analogRead(phr_apin)) / 2;
  } else {
    d->light_level = analogRead(phr_apin);
  }
}

void read_presence(struct data_collection* d, int seconds) {
  double rdg = (analogRead(avoidance_pin) * 5.0) / 1024.0;
  if (seconds % reset != 0) {
    d->presence = (d->presence + rdg) / 2;
  } else {
    d->presence = rdg;
  }
}

void readDHT(struct data_collection *d, int seconds) {
  if (seconds % temp_read == 0) {
    DHT.read11(dht_apin);
    d->humidity = DHT.humidity;
    d->temperature = ((9.0/5.0) * DHT.temperature) + 32;
  }
}

void RoteStateChanged() {
  int v = digitalRead(DAT);
  light_on();
  delay(100);
  light_off();
  if (v < 1) {
    count++;
    if (light_threshold < 512) {
      light_threshold += 10;
    }
  } else {
    count--;
    if (light_threshold > 0) {
      light_threshold -= 10;
    }
  }
  //Serial.println(count);
}

