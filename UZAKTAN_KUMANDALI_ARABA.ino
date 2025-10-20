#include <IRremote.hpp>
//sağ geri yaptı sol ileri yaptı 
const int ena = 14;
const int enb =12;
const int in1= 23;
const int in2= 22;
const int in4= 18;
const int in3 =19;
////
const int IR_RECEIVER= 27;
const int trig= 26;
const int  echo= 25;
///
#define REMOTE_ONE    0xBA45FF00L 
#define REMOTE_TWO    0xB946FF00L
#define REMOTE_THREE  0xB847FF00L
#define REMOTE_FOUR   0xBB44FF00L
#define REMOTE_FIVE   0xBF40FF00L
#define REMOTE_SIX    0xBC43FF00L
#define REMOTE_SEVEN  0xFF807F00L 
#define REMOTE_EIGHT  0xEA15FF00L
#define REMOTE_NINE   0xF609FF00L
#define REMOTE_STAR   0xE916FF00L
#define REMOTE_ZERO   0xE619FF00L
#define REMOTE_SQR    0xF20DFF00L
#define REMOTE_UP     0xE718FF00L
#define REMOTE_LEFT   0xF708FF00L
#define REMOTE_RIGHT  0xA55AFF00L
#define REMOTE_DOWN   0xAD52FF00L
#define REMOTE_OK     0xE31CFF00L 
//
int isMotorWorking=0;
int defaultSpeed=150;
unsigned long lastCode = 0;
bool animasyon1=0;
bool animasyon2=0;
bool animasyon3=0;
bool lefttank=false;


//
void setup() {
  // put your setup code here, to run once:
// Motor A (ENA, IN1, IN2)
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVER,ENABLE_LED_FEEDBACK);
  // digitalWrite(in1, HIGH); // Motor A İleri
  // digitalWrite(in2, LOW);  
  // digitalWrite(in3, HIGH); // Motor B İleri
  // digitalWrite(in4, LOW); 

  // --- KRİTİK ADIM: MOTORLARI AKTİF ETME ---

  // ENA ve ENB pinlerini HIGH yaparak motorları tam güçte çalıştır
  // digitalWrite(ena, HIGH); // Motor A'yı (Sol Motorlar) aktif et
  // digitalWrite(enb, HIGH); // Motor B'yi (Sağ Motorlar) aktif et
}

void loop() {
  // put your main code here, to run repeatedly:
  if(IrReceiver.decode()){
    unsigned long receivedCode=IrReceiver.decodedIRData.decodedRawData;
    if(receivedCode==0xFFFFFFFF){
      receivedCode=lastCode;//eğer yukarı tuşu tekrarlanıyosa 
    }
    else{
      lastCode=receivedCode;
    }
     if(defaultSpeed<=0) defaultSpeed=0;
      if(defaultSpeed>=255) defaultSpeed=255;
      if(lastCode==REMOTE_ONE){
        //sadece motoru çalıştırsın 
        analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
        isMotorWorking=1;
      }
      else if(lastCode==REMOTE_ZERO){
        analogWrite(ena,0);
        analogWrite(enb,0);
       digitalWrite(in1,LOW);
      digitalWrite(in2,LOW);
      digitalWrite(in3,LOW);
      digitalWrite(in4,LOW);
      isMotorWorking=0;
      }
      else if(receivedCode==REMOTE_SQR){
        defaultSpeed+=25;
      }
      else if(receivedCode==REMOTE_STAR){
        defaultSpeed-=25;
      }
      else if(isMotorWorking==1 && defaultSpeed!=0){
        bool keepGoing=false;
        if(lastCode==REMOTE_UP){
           animasyon1=!animasyon1;
           
        }
        else if(lastCode==REMOTE_DOWN){
          animasyon2=!animasyon2;
        }
        else if(lastCode==REMOTE_RIGHT){
          TurnRIGHT();
        }
        else if(lastCode==REMOTE_LEFT){
      
        TurnLEFT();
        }
        else if(lastCode==REMOTE_NINE){
          //tek sağa tank dönüşü sağ geri sol ileri
          animasyon3=!animasyon3;
          
        }
        else if(lastCode==REMOTE_SEVEN){
          //tek sola tank dönüşü sağ ileri sol geri
          keepGoing=true;
          animasyon1=false; 
          animasyon2=false; 
          animasyon3=false;
         lefttank=true;
        }
        else if(!keepGoing){
       // Eğer gelen sinyal bir sürekli hareket komutu değilse, her şeyi durdur
       
       lefttank=false;
       Stop();
       }
  
      }
      else if(isMotorWorking==1 && defaultSpeed==0){
        //Motor çalışık bekleyecek
        analogWrite(ena,0);
        analogWrite(enb,0);
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
      }
    IrReceiver.resume();
  }
  if(animasyon1){
  moveForward();
}
if(animasyon2){
  moveDownward();
}
if(animasyon3){
  analogWrite(ena,defaultSpeed);
          analogWrite(enb,defaultSpeed);
          digitalWrite(in1,HIGH);
          digitalWrite(in2,LOW);
          digitalWrite(in3,HIGH);
          digitalWrite(in4,LOW);
}
if(lefttank){
  analogWrite(ena,defaultSpeed);
          analogWrite(enb,defaultSpeed);
          digitalWrite(in1,LOW);
          digitalWrite(in2,HIGH);
          digitalWrite(in3,LOW);
          digitalWrite(in4,HIGH);
}
}
void moveForward(){
          analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
       digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
}
void moveDownward(){
        analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
}
void Stop(){
  analogWrite(ena,0);
        analogWrite(enb,0);
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
}
void TurnRIGHT(){
  //motorun hızı yarıya düşsse bile en az 40 olucak zaten bu motorlar çok düşük pwm de dönmez
  int minspeed;
    if(defaultSpeed<=70){
      minspeed=max(20,defaultSpeed-20);
    }
    else{
     minspeed=defaultSpeed*0.5;
    }
    int slowspeed=minspeed;
     analogWrite(ena,defaultSpeed);
   analogWrite(enb,slowspeed);
   digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
   digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
}
void TurnLEFT(){
   int minspeed;
    if(defaultSpeed<=70){
       minspeed=max(20,defaultSpeed-20);
    }
    else{
     minspeed=defaultSpeed*0.5;
    }
    int slowspeed=minspeed;
        analogWrite(ena,slowspeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
}

