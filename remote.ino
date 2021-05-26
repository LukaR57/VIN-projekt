#define DS_pin 4
#define STCP_pin 5
#define SHCP_pin 6

//For transmitter
#include <SPI.h>  
#include "RF24.h"
#ifdef RH_HAVE_HARDWARE_SPI
#endif



boolean registers[16]; // default values = false ( = 0 )
boolean presses[16];
int time1 = 10;         // set delay time
char analogs[4];

RF24 myRadio (7, 8);
byte addresses[][100] = {"0"};

void writeReg() {
  digitalWrite(STCP_pin, LOW);        // fixed, SHCP_pin ==> STCP_pin

  for (int i = 15; i >= 0; i--)                 // An array, register[8] for example, has 8 indexes noted as 0-7. Thanks rtcinema
  {
    digitalWrite(SHCP_pin, LOW);     // fixed, SHCP_pin ==> STCP_pin
    digitalWrite(DS_pin, registers[i]);
    digitalWrite(SHCP_pin, HIGH);    // fixed, SHCP_pin ==> STCP_pin
  }
  digitalWrite(STCP_pin, HIGH);      // fixed, SHCP_pin ==> STCP_pin
}


struct package
{
  int id=1;
  float temperature = 18.3;
  char  text[16] = "0000000000000000";
};


typedef struct package Package;
Package data;


void setup() {
  // put your setup code here, to run once:
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
  pinMode(3, INPUT);
  writeReg(); // write the default set of registers[] to DS_pin, one by one
  
  //Transmitter
  Serial.begin(115200);
  delay(1000);
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openWritingPipe( addresses[0]);
  delay(1000);
}

void loop() {

  char str[16] = "000000000000000";
  for ( int i = 0; i <= 15; i++)
  {
    for ( int j = 0; j <= 15; j++)
    {
      registers[j] = (i == j);
    }
    writeReg();
    //delay(1);
    if (i == 10) {
      analogs[0] = analogRead(A0) / 8;
      char neki = analogs[0];
      str[11] = neki;
           data.text[i] = neki;

    }
    else if (i == 12) {
      analogs[1] = analogRead(A1) / 8;
      str[12] = analogs[1];
      data.text[i] = analogs[1];
    }

    else if (i == 13) {
      analogs[2] = analogRead(A2) / 8;
      str[13] = analogs[2];
      data.text[i] = analogs[2];
    }

    else if (i == 11) {
      analogs[3] = analogRead(A3) / 8;
      str[14] = analogs[3];
      data.text[i] = analogs[3];
    }

    else {
      str[i] = digitalRead(3);
      presses[i] = digitalRead(3);
      char dr = digitalRead(3) + 48;
      data.text[i] = dr;
    }

  }
    for (int i = 0; i <= 10; i++) {
    Serial.print(String(presses[i]) + " ");
    }
    Serial.print(String(presses[15]) + " ");

    Serial.print((uint8_t) analogs[0]);
    Serial.print(" ");
    Serial.print((uint8_t) analogs[1]);
    Serial.print(" ");
    Serial.print((uint8_t) analogs[2]);
    Serial.print(" ");
    Serial.print((uint8_t) analogs[3]);

    Serial.println();
    
    //Transmitter
    myRadio.write(&data, sizeof(data)); 
    //Serial.print(data.id);
    //Serial.print("\n");
    //Serial.println(data.text);
    data.id = data.id + 1;
    data.temperature = data.temperature+0.1;



}
