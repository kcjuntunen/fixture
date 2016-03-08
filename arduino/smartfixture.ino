#include <dht.h>

const int interruptA = 0;

#define dht_apin A0
#define phr_apin A1
#define small_reed_apin 5
#define large_reed_apin 6
#define avoidance_pin 7
#define onboard_led 9
#define CLK 2
#define DAT 3
#define SW 4

int light_threshold = 400;
int count = 0;
dht DHT;

struct data_collection {
  double humidity;
  double temperature;
  double light_level;
  double presence;
  bool small_reed_switch;
  bool large_reed_switch;
};

void readdata(struct data_collection* d);
void printdata(struct data_collection* d);

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
  pinMode(onboard_led, OUTPUT);
  Serial.begin(9600);
  delay(1000);//Wait before accessing Sensor
}//end "setup()"

void loop(){
  if (!digitalRead(SW)) {
    count = 0;
    light_threshold = 400;
    delay(2000);
  }
  Serial.print(count);
  light_off();
  struct data_collection *dc;
  readdata(dc);
  printdata(dc);
  wait();
  //Fastest should be once every two seconds.
  delete dc;
}// end loop()

void wait() {
  for (int i = 0; i < 15; i++) {
    light_on();
    delay(500);
    light_off();
    delay(500);
  }
  light_on();
  delay(1000);  
}

void light_on() {
  for (int i = 0; i < 255; i += 5) {
    analogWrite(onboard_led, i);
  }
}

void light_off() {
  for (int i = 255; i > 0; i -= 5) {
    analogWrite(onboard_led, i);
  }
}

void printdata(struct data_collection *d) {
  Serial.print(millis()); 
  Serial.print(":");
  if (d->presence > 0) {
    Serial.print("1:");
    //digitalWrite(onboard_led, HIGH);
  } 
  else {
    Serial.print("0:");
    //digitalWrite(onboard_led, LOW);
  }
  Serial.print(d->large_reed_switch); 
  Serial.print(":");
  Serial.print(d->small_reed_switch); 
  Serial.print(":");
  //if (d->light_level < light_threshold) {
  //  Serial.print("1:");
  //} 
  //else {
  //  Serial.print("0:");
  //}
  Serial.print(d->light_level * -1); Serial.print(":");
  Serial.print(d->humidity); Serial.print(":");
  Serial.print(d->temperature); Serial.print(":");
  Serial.print(light_threshold); Serial.print(":");
  Serial.println(d->presence);
}

void readdata(struct data_collection *d) {
  DHT.read11(dht_apin);
  d->humidity = DHT.humidity;
  d->temperature = ((9.0/5.0) * DHT.temperature) + 32;
  d->light_level = analogRead(phr_apin);
  d->small_reed_switch = !digitalRead(small_reed_apin);
  d->large_reed_switch = digitalRead(large_reed_apin);
  d->presence = digitalRead(avoidance_pin);
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

