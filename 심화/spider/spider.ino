/* 

   (모터주소, 연결핀)
   __________ __________ _________________
  |(4,8)__(0,4)              (3,7) _(7,11)|
  |__|       |       ^        |        |__|
             |       앞       |
             |                |
             |                |
   _________ |                | __________
  |(5,9)__(1,5)__            (2,6)__(6,10)|
  |__|                                 |__|

*/

/*

memo

motor[0].read(),  motor[1].read(),  motor[2].read(),  motor[3].read(),  motor[4].read(),  motor[5].read(),  motor[6].read(),  motor[7].read()

*/

#include <Servo.h>

Servo motor[8];
Servo turret;

const int trig = 12;
const int echo = 13;

//마이크로초
const int speed = 500;

// limit[위치][모드] = {{앞쪽/서는방향한계, 뒤쪽/눕는방향한계}...}
// 측정필요
const int limit[8][2] =  { { 0, 160 }, { 10, 170 }, { 170, 10 }, { 170, 30 }, 
                           { 130, 30 }, { 45, 130 }, { 120, 50 }, { 45, 130 }};

void init_motor() {
  for (int i = 0; i < 8; i += 1) {
    motor[i].attach(i + 4);
  }
  turret.attach(3);
  init_position(speed);
}

void init_position(int speed){
  int init[8] = {(limit[0][0]+limit[0][1])/2,  (limit[1][0]+limit[1][1])/2,  (limit[2][0]+limit[2][1])/2,  (limit[3][0]+limit[3][1])/2,
                 (limit[4][0]+limit[4][1])/2,  (limit[5][0]+limit[5][1])/2,  (limit[6][0]+limit[6][1])/2,  (limit[7][0]+limit[7][1])/2 };
  move_arr(init, speed);
  turret.write(90);
}

void read_motor() {
  for (int i = 0; i < 8; i += 1) {
    String temp = String(i) + "th motor angle " + String(motor[i].read());
    Serial.println(temp);
  }
  Serial.println("turret angle " + String(turret.read()));
}

void lay(int speed) {
  int temp[8] = { motor[0].read(), motor[1].read(), motor[2].read(), motor[3].read(),
                  limit[4][1], limit[5][1], limit[6][1], limit[7][1] };
  move_arr(temp, speed);
}

void stand(int speed) {
  int temp[8] = { motor[0].read(), motor[1].read(), motor[2].read(), motor[3].read(),
                  limit[4][0], limit[5][0], limit[6][0], limit[7][0] };
  move_arr(temp, speed);
}

bool is_same_arr(int arr[]){
  for(int i=0; i<8; i+=1){
    if(motor[i].read() != arr[i]){
      return false;
    }
    return true;
  }
}

void move_arr(int arr[], int speed) {
  while (!is_same_arr(arr)) {
    for (int i = 0; i < 8; i += 1) {
      int temp = motor[i].read();
      if      (temp > arr[i]) motor[i].write(temp - 1);
      else if (temp < arr[i]) motor[i].write(temp + 1);
    }
    delayMicroseconds(speed);
  }
}

void move_legfoot(int num, int arr[], int speed) {
  bool check[2] = { false, false };
  while(!(check[0] && check[1])) {
    for (int i = 0; i < 2; i += 1) {
      int temp = motor[num + 4*i].read();
      if      (temp > arr[i]) motor[num + 4*i].write(temp - 1);
      else if (temp < arr[i]) motor[num + 4*i].write(temp + 1);
      else    check[i] = true;
    }
    delayMicroseconds(speed);
  }
}

void move_one(int num, int angle, int speed) {
  while (true) {
    int temp = motor[num].read();
    if      (temp > angle) motor[num].write(temp - 1);
    else if (temp < angle) motor[num].write(temp + 1);
    else return;
    delayMicroseconds(speed);
  }
}

void swim(int speed){
  int forward[8] = {limit[0][0],limit[1][0],limit[2][0],limit[3][0],
                    limit[4][0],limit[5][0],limit[6][0],limit[7][0]};
  int back[8]    = {limit[0][1],limit[1][1],limit[2][1],limit[3][1],
                    limit[4][1],limit[5][1],limit[6][1],limit[7][1]};
  move_arr(forward,speed);
  stand(speed);
  move_arr(back,speed);
  lay(speed);
  stand(speed);
  init_position(speed);
}

void squat(int speed){
  lay(speed);
  stand(speed);
}

void walk(int speed){
  int posi1[8]={ limit[0][0],  motor[1].read(),  limit[2][1],  motor[3].read(),
                 limit[4][1],  motor[5].read(),  limit[6][1],  motor[7].read() };
  
  int posi2[8]={ motor[0].read(),  limit[1][0],  motor[3].read(), limit[2][0],
                 motor[4].read(),  limit[5][1],  motor[6].read(), limit[7][1] };
  
  move_arr(posi1, speed);
  move_one(4, limit[4][0], speed);
  move_one(6, limit[6][0], speed);
  move_arr(posi2, speed);
  move_one(5, limit[5][0], speed);
  move_one(7, limit[7][0], speed);
  
  //stand(speed);

  init_position(speed);
}

void clock(int speed){
  int posi1[8]={ limit[0][0],  motor[1].read(),  limit[2][1], motor[3].read(),
                 limit[4][1],  motor[5].read(),  limit[6][1], motor[7].read() };
  
  int posi2[8]={ motor[0].read(),  limit[1][1],  motor[3].read(), limit[2][0],
                 motor[4].read(),  limit[5][1],  motor[6].read(), limit[7][1] };
  
  move_arr(posi1, speed);
  move_one(4, limit[4][0], speed);
  move_one(6, limit[6][0], speed);
  stand(speed);
  move_arr(posi2, speed);
  move_one(5, limit[5][0], speed);
  move_one(7, limit[7][0], speed);
  stand(speed);
  init_position(speed);
}

void anticlock(int speed){
  int posi1[8]={ limit[0][1],  motor[1].read(),  limit[2][0], motor[3].read(),
                 limit[4][1],  motor[5].read(),  limit[6][1], motor[7].read() };
  
  int posi2[8]={ motor[0].read(),  limit[1][0],  motor[3].read(), limit[2][1],
                 motor[4].read(),  limit[5][1],  motor[6].read(), limit[7][1] };
  
  move_arr(posi1, speed);
  move_one(4, limit[4][0], speed);
  move_one(6, limit[6][0], speed);
  stand(speed);
  move_arr(posi2, speed);
  move_one(5, limit[5][0], speed);
  move_one(7, limit[7][0], speed);
  stand(speed);
  init_position(speed);
}

void turret_move(){

}

void setup() {
  Serial.begin(9600);
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  init_motor();
}
void loop() {
  switch(receive()){
    case 'w':
      walk(speed);
      break;                                
    case 's':
      swim(speed);
      break;
    case 'q':
      squat(speed);
      break;
    case 'i':
      init_position(speed);
      break;
    case 'b':
      clock(speed);
      break;
    case 'n':
      anticlock(speed);
      break;
    case 'd':
      detect();
      break;
  }
}
char receive() {
  if (Serial.available()) {
    char temp = Serial.read();
    Serial.println("Received char : " + String(temp));
    return temp;
  }
}
void detect(){
  int angle = 0;
  float temp, result;
  for(int i=90; i<=180; i+=1){
    turret.write(i);
    temp = distance();
    if(result < temp){
      result = temp;
      angle = i;
    }
  }
  for(int i=180; i>=0; i-=1){
    turret.write(i);
    temp = distance();
    if(result < temp){
      result = temp;
      angle = i;
    }
  }
  turret.write(angle);
  Serial.println("distance : " + String(temp));
  delay(100);
  turret.write(90);
}
float distance(){
  float result;
  long duration;
  digitalWrite(trig, HIGH);         
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  result = ((float)(340 * duration) / 1000) / 2;
  return result;
}


























