
#include <Servo.h>
#define CHANNELIN1 7
#define CHANNELIN2 8
#define CHANNELIN3 9

#define CHANNELOUT1 3
#define CHANNELOUT2 5
#define CHANNELOUT3 6

#define FWD_LED 16
#define DIVE_LED 17

#define DEFAULT_CH1 1500
#define DEFAULT_CH2 1500
#define DEFAULT_CH3 1500

#define SUB_STARTING 0
#define SUB_OFF1 1
#define SUB_ON1 2
#define SUB_IDLE 3

#define IDLE_TIME 5000
#define START_TIME 1000

#define THRESHOLD 100

#define DEBUGPRINT Serial.print
//#define DEBUGPRINT donothing

int sub_fwd_state = SUB_STARTING;
int sub_dive_state = SUB_STARTING;

int min_channel_value1 = 1500;
int min_channel_value2 = 1500;
int min_channel_value3 = 1500;


long dive_idle_start_time = 0;
long fwd_idle_start_time = 0;

long starting_start_time = 0;


Servo myservo_ch1, myservo_ch2, myservo_ch3;


#include "RunningMedian.h"

RunningMedian channel1_samples = RunningMedian(3);
RunningMedian channel2_samples = RunningMedian(3);
RunningMedian channel3_samples = RunningMedian(3);

 

int pwm_value_ch1 = DEFAULT_CH1, pwm_value_ch2 = DEFAULT_CH2, pwm_value_ch3 = DEFAULT_CH3 ;
void setup() {
  // put your setup code here, to run once:
  pinMode(CHANNELIN1, INPUT);
  pinMode(CHANNELIN2, INPUT);
  pinMode(CHANNELIN3, INPUT);

  pinMode(CHANNELOUT1, OUTPUT);
  pinMode(CHANNELOUT2, OUTPUT);
  pinMode(CHANNELOUT3, OUTPUT);

  pinMode(FWD_LED, OUTPUT);
  pinMode(DIVE_LED, OUTPUT);
  
 

    pwm_value_ch1 = DEFAULT_CH1;
    pwm_value_ch2 = DEFAULT_CH2;
    pwm_value_ch3 = DEFAULT_CH3;

    sub_fwd_state = SUB_STARTING;
    sub_dive_state = SUB_STARTING;

    starting_start_time = millis();

  //Serial.setTimeout(1000);
  Serial.begin(9600);
}



void readFromPWM() {
  pwm_value_ch1 = pulseIn(CHANNELIN1, HIGH);
  pwm_value_ch2 = pulseIn(CHANNELIN2, HIGH);
  pwm_value_ch3 = pulseIn(CHANNELIN3, HIGH);



 if((pwm_value_ch1>500) && (pwm_value_ch2>500) && (pwm_value_ch3>500)){
  channel1_samples.add(pwm_value_ch1);
  channel2_samples.add(pwm_value_ch2);
  channel3_samples.add(pwm_value_ch3);

  if( sub_fwd_state != SUB_STARTING){
     pwm_value_ch1 = channel1_samples.getMedian();
     pwm_value_ch2 = channel2_samples.getMedian();
     pwm_value_ch3 = channel3_samples.getMedian();
  }
  
   if (sub_fwd_state == SUB_STARTING && (millis() - starting_start_time > START_TIME)) {
      min_channel_value1 = pwm_value_ch1;
      min_channel_value2 = pwm_value_ch2;
      min_channel_value3 = pwm_value_ch3;
 
      sub_fwd_state = SUB_OFF1;
      sub_dive_state = SUB_OFF1;
    } 
    
   if (  (( abs(pwm_value_ch1 - min_channel_value1) > THRESHOLD ) || (abs(pwm_value_ch2 - min_channel_value2) > THRESHOLD ))   && sub_fwd_state == SUB_OFF1) {
      myservo_ch1.attach(CHANNELOUT1);
      myservo_ch2.attach(CHANNELOUT2);
      sub_fwd_state = SUB_ON1;
    } 
    
    if (  (abs(pwm_value_ch3 - min_channel_value3) > THRESHOLD )  && sub_dive_state == SUB_OFF1) {
      myservo_ch3.attach(CHANNELOUT3);
      sub_dive_state = SUB_ON1;
    }
  
  
    if (  (abs(pwm_value_ch1 - min_channel_value1) < THRESHOLD ) && (abs(pwm_value_ch2 - min_channel_value2) < THRESHOLD )    && sub_fwd_state == SUB_ON1) {
      sub_fwd_state = SUB_IDLE;
      fwd_idle_start_time = millis();
    }
    if ((abs(pwm_value_ch3 - min_channel_value3) < THRESHOLD )   && sub_dive_state == SUB_ON1) {
      sub_dive_state = SUB_IDLE;
      dive_idle_start_time = millis();
    }
  
  
    if ((abs(pwm_value_ch1 - min_channel_value1) < THRESHOLD ) && (abs(pwm_value_ch2 - min_channel_value2) < THRESHOLD )   && sub_fwd_state == SUB_IDLE && (millis() - fwd_idle_start_time > IDLE_TIME)) {
      sub_fwd_state = SUB_OFF1;
      myservo_ch1.detach();
      myservo_ch2.detach();
  
    } 
    if (   (abs(pwm_value_ch3 - min_channel_value3) < THRESHOLD )   && (sub_dive_state == SUB_IDLE) && (millis() - dive_idle_start_time > IDLE_TIME)) {
      sub_dive_state = SUB_OFF1;
      myservo_ch3.detach();
  
    }

 }

}


void donothing(String s) {}
void donothing(String s, int arg) {}
void donothing(byte b, int arg) {}

void loop() {

  readFromPWM();
  
  myservo_ch1.writeMicroseconds(pwm_value_ch1);
  myservo_ch2.writeMicroseconds(pwm_value_ch2);
  myservo_ch3.writeMicroseconds(pwm_value_ch3);

  Serial.print(pwm_value_ch1);
  Serial.print(",");
  Serial.print(pwm_value_ch2);
  Serial.print(",");
  Serial.println(pwm_value_ch3);




}

