#include "RCEncoder.h"
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

#include "hidjoystickrptparser.h"







class EncoderEvents : public JoystickEvents {
public:
        void OnGamePadChanged(const GamePadEventData *evt);
        void OnHatSwitch(uint8_t hat);
        void OnButtonUp(uint8_t but_id);
        void OnButtonDn(uint8_t but_id);        
};



USB Usb;
USBHub Hub(&Usb);
HIDUniversal Hid(&Usb);
EncoderEvents JoyEvents;
//JoystickEvents JoyEvents;
JoystickReportParser Joy(&JoyEvents);

#define OUTPUT_PIN 2
#define MIN 1000
#define MAX 2000

long brake_start = 0;


void setup ()
{

  pinMode(A0,INPUT);
        Serial.begin(115200);
  encoderBegin(OUTPUT_PIN);
  encoderWrite(0, map(50, 0,100, MIN, MAX));

  encoderWrite(1, map(50, 0,100, MIN, MAX));
  encoderWrite(2, map(50, 0,100, MIN, MAX));
  encoderWrite(3, map(50, 0,100, MIN, MAX));
  
  //delay(5000);
#if !defined(__MIPSEL__)
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
        Serial.println("Start");

        if (Usb.Init() == -1)
                Serial.println("OSC did not start.");

        delay(200);

        if (!Hid.SetReportParser(0, &Joy))
                ErrorMessage<uint8_t > (PSTR("SetReportParser"), 1);
      encoderWrite(1, map(40, 0,100, MIN, MAX));
      delay(1000);

}

int oldtrim=0;
void loop ()
{

 Usb.Task();
/*
 for(int j=0; j<=0; j++){
  for(int i=0; i< 100; i++){
      encoderWrite(j, map(i, 0,100, MIN, MAX));
      delay(20);
      //Usb.Task();
  }    
  for(int i=100; i> 0; i--){
      encoderWrite(j, map(i, 0,100, MIN, MAX));
      delay(20);
      //Usb.Task();
  }    
 }
*/
 /* int trim = map(analogRead(A0),0,1024, 1200,1800);
  if(trim!=oldtrim){
     encoderWrite(0, trim);
     oldtrim=trim;
     Serial.println(trim);
  }
  
*/


}



void EncoderEvents::OnGamePadChanged(const GamePadEventData *evt){
  int xval = evt->x-512;
  Serial.print("{\"X\":");
  Serial.print(xval);

  int yval = (1023 - evt->y)-512;
  Serial.print(",\"Y\":");
  Serial.print(yval);

  int zval = (255-evt->slider - 128)*2;
  Serial.print(",\"Z\":");
  Serial.print(zval);

  int but = evt->buttons_a;
  Serial.print(",\"button\":");
  Serial.print(but);
  Serial.println("}");

  int channel0 = -(xval + zval);
  int channel1 = -(xval - zval);

  encoderWrite(0,map(channel0,-512,512,MIN,MAX));
  // Acc + Brake
  encoderWrite(1,  map(channel1,-512,512,MIN,MAX)); 

  encoderWrite(2,  map(yval,-512,512,MIN,MAX));
  encoderWrite(3,  map(yval,-512,512,MIN,MAX));
  
 /* Serial.print("X:");
  Serial.println(channel0);
   Serial.print("Y:");
  Serial.println(channel1); 
*/
}



void EncoderEvents::OnHatSwitch(uint8_t hat) {
}

void EncoderEvents::OnButtonUp(uint8_t but_id) {
}

void EncoderEvents::OnButtonDn(uint8_t but_id) {
}
