#include <EEPROM.h>
#include "fuzzy_table.h"
#include "PID_Beta6.h"
#include "PinChangeIntConfig.h"
#include "PinChangeInt.h"
#include "MotorWheel.h"
#include "Omni3WD.h"

#define _NAMIKI_MOTOR   //for Namiki 22CL-103501PG80:1

// Motors

irqISR(irq1,isr1);
MotorWheel wheel1(9,8,6,7,&irq1);        // Pin9:PWM, Pin8:DIR, Pin6:PhaseA, Pin7:PhaseB

irqISR(irq2,isr2);
MotorWheel wheel2(10,11,14,15,&irq2);    // Pin10:PWM, Pin11:DIR, Pin14:PhaseA, Pin15:PhaseB

irqISR(irq3,isr3);
MotorWheel wheel3(3,2,4,5,&irq3);        // Pin3:PWM, Pin2:DIR, Pin4:PhaseA, Pin5:PhaseB

Omni3WD Omni(&wheel1,&wheel2,&wheel3);

// setup()
void setup() {
    TCCR1B=TCCR1B&0xf8|0x01;    // Pin9,Pin10 PWM 31250Hz
    TCCR2B=TCCR2B&0xf8|0x01;    // Pin3,Pin11 PWM 31250Hz
    Omni.PIDEnable(0.26,0.02,0,10);
    Serial.begin(9600);
}
unsigned int spdmmps = 50;
unsigned int stopms = 100;
void loop(){
  int inputchar; //入力状態の読み取りに使う
  if (Serial.available() > 0) {
             
    inputchar = Serial.read(); //シリアル通信で送信された値を読み取る
    //Serial.write(inputchar);
    
  //if(inputchar!=-1){
    switch(inputchar){
      //前進
      case 'w':
      //if(Omni.getCarStat()!=Omni3WD::STAT_ADVANCE) Omni.setCarSlow2Stop(stopms);
      Omni.setCarAdvance(spdmmps);
      Omni.PIDRegulate();
      Serial.write("w");
      break;
      //左折
      case 'a':
      //if(Omni.getCarStat()!=Omni3WD::STAT_LEFT) Omni.setCarSlow2Stop(stopms);
      Omni.setCarLeft(spdmmps);
      Omni.PIDRegulate();
      Serial.write("a");
      break;
      //後進
      case 's':
      //if(Omni.getCarStat()!=Omni3WD::STAT_BACKOFF) Omni.setCarSlow2Stop(stopms);
      Omni.setCarBackoff(spdmmps);
      Omni.PIDRegulate();
      Serial.write("s");
      break;
      //右折
      case 'd':
      //if(Omni.getCarStat()!=Omni3WD::STAT_RIGHT) Omni.setCarSlow2Stop(stopms);
      Omni.setCarRight(spdmmps);
      Omni.PIDRegulate();
      Serial.write("d");
      break;
      
      case 'x':
      Omni.setCarStop();
      Omni.PIDRegulate();
      Serial.write(inputchar);
      break;
      }
  
  
  }
}
