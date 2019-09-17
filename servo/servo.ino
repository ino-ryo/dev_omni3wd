
#include <Servo.h>

#define MOTOR_B 9
#define DIR_B 8
#define MOTOR_R 10
#define DIR_R 11
#define MOTOR_L 3
#define DIR_L 2

char input[1];
int pos = 0; // サーボのポジション（変数）
int target = 0; // サーボのポジション（変数）
String inString = ""; // 受信文字列用のバッファ

int pwm=50;
float val = 1/sqrt(3);
float sld = pwm * val;

void goAhead(){
digitalWrite(DIR_L,HIGH);
digitalWrite(DIR_R,LOW);
analogWrite(MOTOR_L, pwm);
analogWrite(MOTOR_R, pwm);
analogWrite(MOTOR_B, 0);
}

void getBack(){
digitalWrite(DIR_L,LOW);
digitalWrite(DIR_R,HIGH);
analogWrite(MOTOR_L, pwm);
analogWrite(MOTOR_R, pwm);
analogWrite(MOTOR_B, 0);
}

void goRight(){
digitalWrite(DIR_L,HIGH);
digitalWrite(DIR_R,HIGH);
digitalWrite(DIR_B,LOW);
analogWrite(MOTOR_L, pwm);
analogWrite(MOTOR_R, pwm);
analogWrite(MOTOR_B, sld);
} 

void goLeft(){
digitalWrite(DIR_L,LOW);
digitalWrite(DIR_R,LOW);
digitalWrite(DIR_B,HIGH);
analogWrite(MOTOR_L, pwm);
analogWrite(MOTOR_R, pwm);
analogWrite(MOTOR_B, sld);
} 

void rotateRight(){
digitalWrite(DIR_L,HIGH);
digitalWrite(DIR_R,HIGH);
digitalWrite(DIR_B,HIGH);
analogWrite(MOTOR_L, pwm);
analogWrite(MOTOR_R, pwm);
analogWrite(MOTOR_B, pwm);
}

void rotateLeft(){
digitalWrite(DIR_L,LOW);
digitalWrite(DIR_R,LOW);
digitalWrite(DIR_B,LOW);
analogWrite(MOTOR_L, pwm);
analogWrite(MOTOR_R, pwm);
analogWrite(MOTOR_B, pwm);
}

void stopAll(){
  digitalWrite(DIR_L,LOW);
  digitalWrite(DIR_R,LOW);
  digitalWrite(DIR_B,LOW);
  analogWrite(MOTOR_L, 0);
  analogWrite(MOTOR_R, 0);
  analogWrite(MOTOR_B, 0);
}

void setup(){

Serial.begin(9600);

}
int flag = 0;
//int dl = 400;
int dl = 0;
void loop(){
  
  int inputchar; //入力状態の読み取りに使う
  if (Serial.available() > 0) {
             
    inputchar = Serial.read(); //シリアル通信で送信された値を読み取る
    //Serial.write(inputchar);dw
    
  //if(inputchar!=-1){
    switch(inputchar){
      //前進
      case 'w':
      goAhead();
      Serial.print("w");
      delay(dl);
      break;
      //左折
      case 'a':
      goLeft();
      Serial.print("a");
      delay(dl);
      break;
      //後進
      case 's':
      getBack();
      Serial.print("s");
      delay(dl);
      break;
      //右折
      case 'd':
      goRight();
      Serial.print("d");
      delay(dl);
      break;
      
      case 'x':
      stopAll();
      Serial.print("x");
      break;
      case 'q':
      rotateLeft();
      Serial.print("q");
      delay(dl);
      break;
      case 'e':
      rotateRight();
      Serial.print("e");
      delay(dl);
      break;
      }
  flag = 0; 
  }
  //flag++;
  //Serial.print(flag);
  //if(flag > 50000){stopAll();}
  //stopAll();
}
