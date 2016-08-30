#include <Ethernet.h>
#include <EthernetUdp.h>
#include <EthernetServer.h>
#include <Dns.h>
#include <Dhcp.h>
#include <EthernetClient.h>

#include "RCEncoder.h"
// requires USB HOST library v2 from: https://github.com/felis/USB_Host_Shield_2.0.git
#include <usbhid.h>
#include <hiduniversal.h>
#include <usbhub.h>

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

#include "hidjoystickrptparser.h"


#define PRIGHT 1900
#define PLEFT 1700
#define PBRAKE 1500
#define PBRAG 1300
#define PNORMAL 1100




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

boolean braking = false;
int xval = 0, yval = 0;

void setup ()
{

  pinMode(A0,INPUT);
        Serial.begin(115200);
  encoderBegin(OUTPUT_PIN);
  encoderWrite(0, map(50, 0,100, MIN, MAX));

  encoderWrite(1, map(50, 0,100, MIN, MAX));
  encoderWrite(2, PNORMAL);
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
  int xval = evt->X + (evt->Y & 0x03)*0xFF;
  xval = (xval - 500)*0.5 + 500;
  Serial.println(xval);


  int trim = map(analogRead(A0),0,1024, -30,30);
  Serial.println(trim);

  // Steering
  encoderWrite(0, MIN+MAX-map(xval,0,1000,MIN,MAX));

  // Acc + Brake
  int yval = evt->Z2+trim;

  // turbo mode with button, else speed limited
  int bragbutton = evt->Z1 & 0x1;
  if (bragbutton != 1) {
    if (yval < 60) yval = 60;
  }

  if (yval > 100) {
    yval = 100;
  }

  Serial.println(yval);
  encoderWrite(1, map(yval,28,180,MIN,MAX));     

  if(yval>99){
    encoderWrite(2, PBRAKE); 
    Serial.println("Brake");
  } else  if(bragbutton == 1){
    encoderWrite(2, PBRAG); 
    Serial.println("Brag");
  } else if(xval>600){
    encoderWrite(2, PRIGHT);
    Serial.println("R"); 
  } else if(xval<400){
    encoderWrite(2, PLEFT);
    Serial.println("L"); 
  } else {
    encoderWrite(2, PNORMAL);
    Serial.println("N");     
  }
  
  
  

}



void EncoderEvents::OnHatSwitch(uint8_t hat) {
}

void EncoderEvents::OnButtonUp(uint8_t but_id) {
}

void EncoderEvents::OnButtonDn(uint8_t but_id) {
}
