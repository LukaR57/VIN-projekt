#include <SPI.h>
#include "RF24.h"

RF24 myRadio (7, 8);
struct package
{
  int id = 0;
  float temperature = 0.0;
  char  text[100] = "empty";
};

byte addresses[][6] = {"0"};



typedef struct package Package;
Package data;


#define echoPin A0
#define trigPin A1
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
long lastPacket = 0;
String ultrasonic = "0";

void setup()
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  Serial.begin(115200);

  myRadio.begin();
  myRadio.setChannel(115);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  pinMode(2, OUTPUT); //motor 1 left
  pinMode(3, OUTPUT); //motor 1 pwm
  pinMode(4, OUTPUT); //motor 1 right
  pinMode(5, OUTPUT); //motor 2 left
  pinMode(6, OUTPUT); //motor 2 pwm
  pinMode(10, OUTPUT); //motor 2 right
  pinMode(9, OUTPUT); //beeper
}


void loop()
{

  if ( myRadio.available())
  {
    while (myRadio.available())
    {
      myRadio.read( &data, sizeof(data) );
      lastPacket = millis();
    }
    /*
      Serial.print("\nPackage:");
      Serial.print(data.id);
      Serial.print("\n");
      Serial.println(data.temperature);
    */
    unsigned char vrednost = data.text[12];
    unsigned int vrednost2 = data.text[13];
    Serial.println(vrednost);
    Serial.println(vrednost2);
    Serial.println();
    if (data.text[0] == '1') {
      digitalWrite(2, HIGH);
      analogWrite(3, 255);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      analogWrite(6, 255);
      digitalWrite(10, HIGH);
    }
    else if (data.text[15] == '1') {
      digitalWrite(2, LOW);
      analogWrite(3, 255);
      digitalWrite(4, HIGH);
      digitalWrite(5, HIGH);
      analogWrite(6, 255);
      digitalWrite(10, LOW);
    }
    else {
      Serial.println("IZVAJA");
      if (vrednost < 50) {
        digitalWrite(2, HIGH);
        digitalWrite(4, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(10, LOW);
        vrednost = abs(vrednost - 55) * 6;
      }
      else if (vrednost > 57) {
        digitalWrite(2, LOW);
        digitalWrite(4, HIGH);
        digitalWrite(5, LOW);
        digitalWrite(10, HIGH);
        vrednost = abs(vrednost - 55) * 6;
      }
      else {
        digitalWrite(2, LOW);
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
        digitalWrite(10, LOW);
        vrednost = 0;
      }

      float leftSpeed = 1;
      float rightSpeed = 1;
      if (vrednost2 < 24) {
        rightSpeed = ((((vrednost2 - 17) * (255 - 0)) / (25 - 17)) + 0) / 255;
      }
      else if (vrednost2 > 27) {
        leftSpeed = abs(255 - (((vrednost2 - 25) * (255 - 0)) / (68 - 25)) + 0) / 255;

      }


      for (int i = 0; i < 16; i++) {
        Serial.print(data.text[i]);
        Serial.print(" ");
      }

      int vrednostLevega = vrednost;
      int vrednostDesnega = vrednost;
      analogWrite(3, vrednostLevega * leftSpeed);
      analogWrite(6, vrednostDesnega * rightSpeed);

    }
    ultrasonic = data.text[14];
  }
  if (lastPacket  < millis() - 400) {
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(10, LOW);
  }

  //ULTRASONIC SENSOR
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 10000);
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm");
  if (distance <= 30  && ultrasonic == "1") {
    int pwm = abs(distance - 30);
    pwm = pwm * 6;
    //Serial.println(pwm);
    analogWrite(9, pwm);
  }
  else {
    analogWrite(9, 0);
  }

}
