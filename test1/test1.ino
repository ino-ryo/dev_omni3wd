void setup() {
  Serial.begin(9600);
}

void loop() {
  int inputchar; //入力状態の読み取りに使う
  
  inputchar = Serial.read(); //シリアル通信で送信された値を読み取る
  Serial.println(inputchar);
}
