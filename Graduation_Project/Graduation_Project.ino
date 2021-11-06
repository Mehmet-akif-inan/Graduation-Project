//ARDUINO TRANSMITTER CODE

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
struct MyData {
byte ileri;
byte geri;
byte sag;
byte sol;
int y;
int x;};
MyData data;

void setup() {
pinMode(A3, INPUT);
pinMode(A2, INPUT);
Serial.begin(9600);
radio.begin();
radio.openWritingPipe(pipe);}

void loop() {
int pina3 = analogRead(A3); // geri // orta = 512,511 1003-6 // Y
int pina2 = analogRead(A2); // sol 0 orta= 516,517 1004-7 // X
int rev3 = 1023 - pina3 ; // ileri // rev orta = 528,529 50-1017
int rev2 = 1023 - pina2 ; // sağ // sağ 1023 ort= 520,521 46,1017
int ileri = map(rev3, 528, 1017, 0, 255);
int geri = map(pina3, 512, 1005, 0, 255);
int sag = map(rev2, 520, 1017, 0, 255);
int sol = map(pina2, 516, 1004, 0, 255)
ileri = constrain(ileri, 0, 255);
geri = constrain(geri, 0, 255);
sag = constrain(sag, 0, 255);
sol = constrain(sol, 0, 255);
data.ileri = ileri;
data.geri = geri;
data.sol = sol;
data.sag = sag;
data.y = analogRead(A3);
data.x = analogRead(A2);
radio.write(&data, sizeof(data) );
}

//ARDUINO RECEIVER CODE

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
struct MyData {
byte ileri;
byte geri;
byte sag;
byte sol;
int y;
int x;};
MyData data;
int STBY = 3;
//Motor A
int PWMA = 6; //Speed control
int AIN1 = 7; //Direction
int AIN2 = 8; //Direction
//Motor B
int PWMB = 5; //Speed control
int BIN1 = 2; //Direction
int BIN2 = 4; //Direction

void setup() {
pinMode(STBY, OUTPUT);
pinMode(A0, OUTPUT);
pinMode(PWMA, OUTPUT);
pinMode(AIN1, OUTPUT);
pinMode(AIN2, OUTPUT);
pinMode(PWMB, OUTPUT);
pinMode(BIN1, OUTPUT);
pinMode(BIN2, OUTPUT);
Serial.begin(9600);
radio.begin();
radio.openReadingPipe(1,pipe);
radio.startListening();}

void loop() {
if (radio.available()) {
bool done = false;
while(!done) {
done = radio.read(&data, sizeof(MyData) );
digitalWrite(A0,LOW);
if (data.y < 506 && data.x < 530 && data.x > 490){
move(1, data.ileri, 1); //
move(2, data.ileri, 1); //
Serial.print("ILERI "); }
if (data.y > 506 && data.x < 530 && data.x > 490){
move(1, data.geri, 2); //
move(2, data.geri, 2); //
Serial.print("GERI "); }
if (data.x > 513 && data.y < 530 && data.y > 490){
move(1, data.sol, 2); // sol
move(2, data.sol, 1); //
Serial.print("SOL ");}
if (data.x < 513 && data.y < 530 && data.y > 490){
move(1, data.sag, 1); // sag
move(2, data.sag, 2); //
Serial.print("SAG ");}
if (data.x < 450 && data.y < 450 ){
move(1, data.ileri, 1);
move(2, data.ileri - 50 , 1);
Serial.print("SAG ILERI ");}
if (data.x < 450 && data.y > 570){
move(1, data.ileri - 50, 1);
move(2, data.ileri, 1);
Serial.print("SOL ILERI ");}
if (data.x > 570 && data.y < 450 ){
move(1, data.geri, 2);
move(2, data.geri- 50, 2);
Serial.print("SAG GERI ");}
if (data.x > 570 && data.y > 570){
move(1, data.geri - 50, 2);
move(2, data.geri, 2);
Serial.print("SOL GERI ");}
if (data.x > 505 && data.x < 515 && data.y > 505 && data.y < 515){
move(1, 0, 1);
move(2, 0, 1);
Serial.print("DUR ");}}
Serial.println("");}
else {Serial.println("Sinyal bulunamadı");
digitalWrite(A0,HIGH);
move(1, 0, 1);
move(2, 0, 1);
delay(100); }}
void move(int motor, int speed, int direction){
//motor: 0 for B 1 for A
//speed: 0 is off, and 255 is full speed
//direction: 0 clockwise, 1 counter-clockwise
digitalWrite(STBY, HIGH); //disable standby
boolean inPin1 = LOW;
boolean inPin2 = HIGH;
if(direction == 1){
inPin1 = HIGH;
inPin2 = LOW;}
if(motor == 1){
digitalWrite(AIN1, inPin1);
digitalWrite(AIN2, inPin2);
analogWrite(PWMA, speed);}
else{
digitalWrite(BIN1, inPin1);
digitalWrite(BIN2, inPin2);
analogWrite(PWMB, speed);
}
}
