#include<WiFi.h>
int defaultSpeed=150;
const int ena = 14;
const int enb =12;
const int in1= 23;
const int in2= 22;
const int in3 =19;
const int in4= 18;
////
const int trig= 26;
const int  echo= 25;
///
const int red=13;
const int blue=15;
const int green=5;
const int leds[]={13,15,5};
//


unsigned long onceki_zaman;
bool engeldenKaciyor = false;
unsigned long engelZamani = 0;
//
//
unsigned long simdiki_zaman=0;
unsigned long Onceki_zaman=0;
int aralik=500;
boolean durum=false;
boolean sinyal=false;

boolean disko=false;
unsigned long Onceki_zaman1=0;
unsigned long simdiki_zaman1=0;
boolean stage1=true;
boolean stage2=false;
boolean stage3=false;

const char *ssid="AKADEMI MYHOME";
const char *password="Akademi@22";

#define gecikme 100
int isMotorWorking=0;
bool animasyon1=0;

//

const char KONTROL_SAYFASI[] PROGMEM = R"EOF(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Araba Kontrol</title>
    <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
    <style>
        body { background-color: #2c3e50; color: #ecf0f1; font-family: Arial, sans-serif; text-align: center; padding-top: 20px; }
        h1 { color: #3498db; margin-bottom: 10px; }
        
        /* YENI EKLENDI - Durum Kutusu */
        .status-box { background-color: #34495e; padding: 10px; margin: 0 auto 15px auto; border-radius: 8px; max-width: 300px; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }
        .status-item { font-size: 18px; margin: 5px; color: #ecf0f1; }
        .status-item span { color: #1abc9c; font-weight: bold; }
        
        .btn { background-color: #3498db; color: white; border: none; padding: 20px 25px; font-size: 24px; margin: 8px; border-radius: 10px; user-select: none; -webkit-user-select: none; min-width: 80px; }
        .btn:active { background-color: #2980b9; }
        #btn-dur { background-color: #e74c3c; }
        #btn-dur:active { background-color: #c0392b; }
        .btn-speed { background-color: #1abc9c; }
        .btn-speed:active { background-color: #16a085; }
        .btn-special { background-color: #f1c40f; }
        .btn-special:active { background-color: #f39c12; }
        .kontrol-grubu { margin-bottom: 10px; display: flex; justify-content: center; align-items: center; flex-wrap: wrap; }
    </style>
</head>
<body>
    <h1>ESP32 Kumanda</h1>

    <div class="status-box">
        <div class="status-item">Vites: <span id="vites-durum">%%VITES_DURUM%%</span></div>
        <div class="status-item">Hız: <span id="hiz-deger">%%HIZ_DEGER%%</span></div>
    </div>

    <div class="kontrol-grubu">
        <button id="btn-acc" class="btn btn-speed">+</button>
        <button id="btn-tank" class="btn btn-special">TANK</button>
        <button id="btn-dec" class="btn btn-speed">-</button>
    </div>
    <div class="kontrol-grubu">
        <button id="btn-ileri" class="btn">ILERI</button>
    </div>
    <div class="kontrol-grubu">
        <button id="btn-sol" class="btn">SOL</button>
        <button id="btn-dur" class="btn">DUR</button>
        <button id="btn-sag" class="btn">SAG</button>
    </div>
    <div class="kontrol-grubu">
        <button id="btn-geri" class="btn">GERI</button>
    </div>
    <div class="kontrol-grubu">
        <button id="btn-gear" class="btn btn-special">VITES</button>
    </div>

    <script>
        // Javascript (Degismedi)
        function komutGonder(komut) {
            console.log("Komut: /" + komut);
            fetch('/' + komut).catch(err => console.error(err));
        }
        ['btn-ileri', 'btn-geri', 'btn-sol', 'btn-sag', 'btn-tank'].forEach(id => {
            const btn = document.getElementById(id);
            const komut = id.split('-')[1];
            btn.addEventListener('touchstart', (e) => { e.preventDefault(); komutGonder(komut); }, { passive: false });
            btn.addEventListener('touchend', (e) => { e.preventDefault(); komutGonder('stop'); });
            btn.addEventListener('mousedown', () => komutGonder(komut));
            btn.addEventListener('mouseup', () => komutGonder('stop'));
        });
        document.getElementById('btn-dur').addEventListener('click', () => komutGonder('stop'));
        document.getElementById('btn-acc').addEventListener('click', () => komutGonder('acc'));
        document.getElementById('btn-dec').addEventListener('click', () => komutGonder('dec'));
        document.getElementById('btn-gear').addEventListener('click', () => komutGonder('gear'));
        document.getElementById('btn-dur').addEventListener('touchstart', (e) => { e.preventDefault(); komutGonder('stop'); }, { passive: false });
    </script>
</body>
</html>
)EOF";
// d ve p butonları d 1 olsun p ye basınca d 0 olsun 0ken geri sağ sol olsun tekrar basınca d 1 olsun ve ileri sağ sol gelsin
boolean gear = true;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println();
  // Serial.print("Access Point baslatiliyor... SSID: ");
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  Serial.print("Wifi'ye baglanıyor.");
  // WiFi.begin() yerine WiFi.softAP() kullaniyoruz
  while (WiFi.status()!=WL_CONNECTED) {
    Serial.println(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Wifi'ye baglanildi");
  Serial.print("AP IP adresi (Tarayiciya bu adresi gir): ");
  Serial.println(WiFi.localIP()); 

  server.begin(); 

  // Motor A (ENA, IN1, IN2)
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);
  analogWrite(red,231);
  analogWrite(blue,31);
  analogWrite(green,199);
}

void loop() {
  //1.öncelik mesafe 
  if(!engeldenKaciyor && millis()-onceki_zaman>gecikme){//0.1 snde bir ve engelden kacmiyorsa olcüm yapsın
    onceki_zaman=millis();
    float mesafe=MESAFE_OLCER();
    if(mesafe>0 && mesafe<=30){
      analogWrite(ena,defaultSpeed/2);
      analogWrite(enb,defaultSpeed/2);
      delay(200);
      moveDownward(); 
      analogWrite(green,0);
      analogWrite(red,255); 
      analogWrite(blue,0); 
      engeldenKaciyor=true;
      engelZamani=millis();//hangi ms de engeli gördüyse engel zamanına o ms yi yazar
      animasyon1=false;
    }
  }
  if(engeldenKaciyor && millis()-engelZamani>=1000){//1 sn geri gel
    Stop();
    engeldenKaciyor=false;
  }
  if(sinyal) disko=false;
  if(disko) sinyal=false;

  if(sinyal){
    simdiki_zaman=millis();
    if(simdiki_zaman - Onceki_zaman >= aralik){
      Onceki_zaman=simdiki_zaman;//150 msde bir yakıp sönsün
      if(!durum){
        durum=true;
        analogWrite(blue,255);
        analogWrite(red,255);
        analogWrite(green,0);
      }
      else{
        durum=false;
        analogWrite(blue,0);
        analogWrite(red,0);
        analogWrite(green,0);
      }
  }
}
  if(disko){
    simdiki_zaman1=millis();
    if(simdiki_zaman1 - Onceki_zaman1 >= gecikme){
      Onceki_zaman1=simdiki_zaman1;
      if(stage1==true && stage2==false && stage3==false){
        analogWrite(blue,255);
        analogWrite(red,0);
        analogWrite(green,0);
        stage1=false;
        stage2=true;
        stage3=false;
      }
      else if(stage1==false && stage2==true && stage3==false){
        analogWrite(blue,0);
        analogWrite(red,255);
        analogWrite(green,0);
        stage1=false;
        stage2=false;
        stage3=true;
      }
      else if(stage1==false && stage2==false && stage3==true){
        analogWrite(blue,0);
        analogWrite(red,0);
        analogWrite(green,255);
        stage1=true;
        stage2=false;
        stage3=false;
      }
    }
  }
  /////////////
  //2.öncelik WiFi
  WiFiClient client = server.available();

  if(!client){
    return;
  }
  Serial.println("Yeni istemci baglandı");
  String currentLine="";
  boolean firstLine= true;

  while(client.connected()){
    if(client.available()){
      char c = client.read();
      Serial.write(c);

      if(c=='\n'){
        if(firstLine){

          if(!engeldenKaciyor && gear==true){
            if(currentLine.indexOf("GET /ileri")>=0){
              moveForward();
            }
            else if(currentLine.indexOf("GET /gear")>=0){
              gear=!gear;
            }
            else if(currentLine.indexOf("GET /geri") >= 0){
              moveDownward();
            }
            else if(currentLine.indexOf("GET /sol") >= 0){
              TurnLEFT();
            }
            else if(currentLine.indexOf("GET /sag")>=0){
              TurnRIGHT();
            }
            else if(currentLine.indexOf("GET /stop")>=0){
              Stop();
            }
            else if(currentLine.indexOf("GET /tank")>=0){
              TANK();
            }
            else if(currentLine.indexOf("GET /acc")>=0){
              if(defaultSpeed>=255){
                defaultSpeed=255;
              }
              else{
                 defaultSpeed+=10;
              }
            }
            else if(currentLine.indexOf("GET /dec")>=0){
              if(defaultSpeed <= 95){ // Minimum hiz 95 ise
                defaultSpeed = 95;
              } else {
                defaultSpeed -= 10; // Degilse 10 azalt
              }
            }

          }
          else if(!engeldenKaciyor && gear==false){
            if(currentLine.indexOf("GET /ileri")>=0){
              moveForward();
            }
            else if(currentLine.indexOf("GET /gear")>=0){
              gear=!gear;
            }
            else if(currentLine.indexOf("GET /geri") >= 0){
              moveDownward();
            }
            else if(currentLine.indexOf("GET /sol") >= 0){
              TurnLEFT_reverse();
            }
            else if(currentLine.indexOf("GET /sag")>=0){
              TurnRIGHT_reverse();
            }
            else if(currentLine.indexOf("GET /tank")>=0){
              TANK();
            }
            else if(currentLine.indexOf("GET /stop")>=0){
              Stop();
            }
            else if(currentLine.indexOf("GET /acc")>=0){
              if(defaultSpeed>=255){
                defaultSpeed=255;
              }
              else{
                 defaultSpeed+=10;
              }
            }
            else if(currentLine.indexOf("GET /dec")>=0){
              if(defaultSpeed <= 95){ // Minimum hiz 95 ise
                defaultSpeed = 95;
              } else {
                defaultSpeed -= 10; // Degilse 10 azalt
              }
            }
          }
          firstLine=false;
        }

        if(currentLine.length()==0){
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type: text/html");
          client.println();
          client.print(KONTROL_SAYFASI);
          client.println();
          break;
        } else{
          currentLine = "";
        }
      } else if(c!='\r'){
        currentLine +=c;
      }
    }
  }
  delay(1);
  client.stop();
  Serial.println("Istemci baglantisi kesildi");
}
void moveForward(){
          analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
       digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
        analogWrite(blue,255);
        analogWrite(green,0);
        analogWrite(red,0);
        sinyal=false;
        disko=false;
}
void moveDownward(){
        analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
        analogWrite(blue,0);
        analogWrite(green,255);
        analogWrite(red,0);
        sinyal=false;
        disko=false;

}
void Stop(){
  analogWrite(ena,0);
        analogWrite(enb,0);
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
        sinyal=false;
        analogWrite(red, 0);
        analogWrite(blue, 0);
        analogWrite(green, 0);
        disko=false;
}
void TurnRIGHT(){
  //motorun hızı yarıya düşsse bile en az 40 olucak zaten bu motorlar çok düşük pwm de dönmez 
    int slowspeed=defaultSpeed*0.4;
     analogWrite(ena,defaultSpeed);
   analogWrite(enb,slowspeed);
   digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
   digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  sinyal=true;
        disko=false;     
}
void TurnLEFT(){
     int slowspeed=defaultSpeed*0.4;
        analogWrite(ena,slowspeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
    sinyal=true;
        disko=false;       
}
float MESAFE_OLCER(){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  float time=pulseIn(echo,HIGH);
  float mesafe=time*0.0343/2;
  return mesafe;
}
void TurnRIGHT_reverse(){//sağ ters in3 in4 
        analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        digitalWrite(in3,LOW);
        digitalWrite(in4,LOW);
  sinyal=true;
        disko=false;   
}
void TurnLEFT_reverse(){
        analogWrite(ena,defaultSpeed);
        analogWrite(enb,defaultSpeed);
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
  sinyal=true;
        disko=false;      
}
void TANK(){
  
  analogWrite(ena,defaultSpeed);
  analogWrite(enb,defaultSpeed);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
  sinyal=false;
        disko=true;
}