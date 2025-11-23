#include<Arduino.h>

#include<WiFi.h>




uint8_t arac_durum= 0;
uint8_t disko_asama= 0;
#define gecikme 100
#define SINYAL (1<<0)
#define ENGEL_GORDUMU (1<<1)
#define MOTOR_CALISIYORMU (1<<2)
#define DISKO (1<<3)
#define LAMBA (1<<4)
#define VITES (1<<5)

typedef struct{

  int EN;
  int in1_3;
  int in2_4;

}Motor;

typedef struct{

  int ledR;
  int ledG;
  int ledB;

}RGB;

typedef struct{

  int Trig;
  int Echo;

}MESAFE;

MESAFE Sonar = {26,25};
Motor sol_motor = {14,23,22};
Motor sag_motor = {12,19,18};
RGB leds = {13,5,15};
void motor_baslat(Motor *s);
void led_baslat(RGB *s);
void sonar_baslat(MESAFE *s);
void renk_ayarla(RGB *s, int r, int g, int b);
void hiz_ayarla(Motor *s, float hiz);
void moveForward();
void moveDownward();
void Stop();
void TurnRIGHT();
void TurnLEFT();
void TurnRIGHT_reverse();
void TurnLEFT_reverse();
void TANK();
float MESAFE_OLCER(MESAFE *s);

int defaultSpeed=150;

unsigned long onceki_zaman;

unsigned long engelZamani = 0;
unsigned long simdiki_zaman=0;
unsigned long Onceki_zaman=0;
int aralik=500;
unsigned long Onceki_zaman1=0;
unsigned long simdiki_zaman1=0;


const char *ssid="AKADEMI MYHOME";
const char *password="Akademi@22";

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
void motor_baslat(Motor *s){
  
  pinMode(s->EN,OUTPUT);
  pinMode(s->in1_3,OUTPUT);
  pinMode(s->in2_4,OUTPUT);

}

void led_baslat (RGB *s){

  pinMode(s->ledR,OUTPUT);
  pinMode(s->ledG,OUTPUT);
  pinMode(s->ledB,OUTPUT);

}

void sonar_baslat(MESAFE *s){

  pinMode(s->Echo,INPUT);
  pinMode(s->Trig,OUTPUT);

}

void renk_ayarla (RGB *s,int r, int g, int b){

  analogWrite(s->ledR,r);

  analogWrite(s->ledG,g);

  analogWrite(s->ledB,b);

}

void hiz_ayarla(Motor *s , float hiz){

  analogWrite(s->EN,hiz);

}

void moveForward(){
    hiz_ayarla(&sol_motor,defaultSpeed);

    hiz_ayarla(&sag_motor,defaultSpeed);
    
    digitalWrite(sol_motor.in1_3,HIGH);
    
    digitalWrite(sol_motor.in2_4,LOW);
    digitalWrite(sag_motor.in1_3,LOW);
    digitalWrite(sag_motor.in2_4,HIGH);
    renk_ayarla(&leds,0,0,255);

    arac_durum&= ~SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
    arac_durum&= ~ DISKO;
}

void moveDownward(){
  hiz_ayarla(&sol_motor,defaultSpeed);
  hiz_ayarla(&sag_motor,defaultSpeed);
  digitalWrite(sol_motor.in1_3,LOW);
  digitalWrite(sol_motor.in2_4,HIGH);
  digitalWrite(sag_motor.in1_3,HIGH);
  digitalWrite(sag_motor.in2_4,LOW);        
  renk_ayarla(&leds,0,255,0);
        
  arac_durum&= ~SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
  arac_durum&= ~ DISKO;

}

void Stop(){
  hiz_ayarla(&sol_motor,0);
  hiz_ayarla(&sag_motor,0);
  digitalWrite(sol_motor.in1_3,LOW);
  digitalWrite(sol_motor.in2_4,LOW);
  digitalWrite(sag_motor.in1_3,LOW);
  digitalWrite(sag_motor.in2_4,LOW);        
  renk_ayarla(&leds,0,0,0);

  arac_durum&= ~SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
  arac_durum&= ~ DISKO;
}

void TurnRIGHT(){
  //motorun hızı yarıya düşsse bile en az 40 olucak zaten bu motorlar çok düşük pwm de dönmez 
  int slowspeed=defaultSpeed*0.6;
  hiz_ayarla(&sol_motor,defaultSpeed);
  hiz_ayarla(&sag_motor,slowspeed);
  digitalWrite(sol_motor.in1_3,HIGH);
  digitalWrite(sol_motor.in2_4,LOW);
  digitalWrite(sag_motor.in1_3,LOW);
  digitalWrite(sag_motor.in2_4,HIGH);    

  arac_durum|= SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
  arac_durum&= ~ DISKO; 
}

void TurnLEFT(){
  int slowspeed=defaultSpeed*0.6;
  hiz_ayarla(&sol_motor,slowspeed);
  hiz_ayarla(&sag_motor,defaultSpeed);
  digitalWrite(sol_motor.in1_3,HIGH);
  digitalWrite(sol_motor.in2_4,LOW);
  digitalWrite(sag_motor.in1_3,LOW);
  digitalWrite(sag_motor.in2_4,HIGH);  
       
  arac_durum|= SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
  arac_durum&= ~ DISKO;       
}

float MESAFE_OLCER(MESAFE *s){
  digitalWrite(s->Trig,LOW);
  delayMicroseconds(2);
  digitalWrite(s->Trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(s->Trig,LOW);
  float time=pulseIn(s->Echo,HIGH);
  float mesafe=time*0.0343/2;
  return mesafe;
}

void TurnRIGHT_reverse(){//sağ ters in3 in4 
  hiz_ayarla(&sol_motor,defaultSpeed);
  hiz_ayarla(&sag_motor,defaultSpeed);
  digitalWrite(sol_motor.in1_3,LOW);
  digitalWrite(sol_motor.in2_4,HIGH);
  digitalWrite(sag_motor.in1_3,LOW);
  digitalWrite(sag_motor.in2_4,LOW);        

  arac_durum|= SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
  arac_durum&= ~ DISKO;  
}

void TurnLEFT_reverse(){
  hiz_ayarla(&sol_motor,defaultSpeed);
  hiz_ayarla(&sag_motor,defaultSpeed);
  digitalWrite(sol_motor.in1_3,LOW);
  digitalWrite(sol_motor.in2_4,LOW);
  digitalWrite(sag_motor.in1_3,HIGH);
  digitalWrite(sag_motor.in2_4,LOW);        
  
  arac_durum|= SINYAL;// 0000 0000 & 1111 1110  = 0000 0000
  arac_durum&= ~ DISKO;  
}

void TANK(){
  hiz_ayarla(&sol_motor,defaultSpeed);
  hiz_ayarla(&sag_motor,defaultSpeed);
  digitalWrite(sol_motor.in1_3,HIGH);
  digitalWrite(sol_motor.in2_4,LOW);
  digitalWrite(sag_motor.in1_3,HIGH);
  digitalWrite(sag_motor.in2_4,LOW);        
  
  arac_durum&= ~ SINYAL;// 
  arac_durum|= DISKO; // 0000 0000 | 0000 1000 = 0000 1000
}





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

  motor_baslat(&sol_motor);

  motor_baslat(&sag_motor);

  led_baslat(&leds);

  sonar_baslat(&Sonar);

  renk_ayarla(&leds,255,199,31);


}

void loop() {
  //1.öncelik mesafe 
  if(!(arac_durum&ENGEL_GORDUMU) && millis()-onceki_zaman>gecikme){//0000 0000 & 0000 0001 = 0000 0000 engel yok ama 0 olduğu için if dönmez o yüzden ! 
    onceki_zaman=millis();
    float mesafe=MESAFE_OLCER(&Sonar);
    if(mesafe>0 && mesafe<=30){
      hiz_ayarla(&sol_motor,defaultSpeed/2);
      hiz_ayarla(&sag_motor,defaultSpeed/2);

      delay(200);
      moveDownward(); 

      renk_ayarla(&leds,255,0,0);
       
      
      arac_durum|=ENGEL_GORDUMU;// 0000 0001 

      engelZamani=millis();//hangi ms de engeli gördüyse engel zamanına o ms yi yazar
      
    }
  }
  if(arac_durum&ENGEL_GORDUMU && millis()-engelZamani>=1000){// 0000 0001 & 0000 0001 = 0000 0001 engel görüdyse 
    Stop();
    // durum = 0000 0001 engel gordu mu 0000 0001
    arac_durum^=ENGEL_GORDUMU; //0000 0001---0000 0001 = 0000 0000
  }

 
  if(arac_durum&SINYAL){
    // 0000 0001 durum 0000 0000 disko 0000 0010
    arac_durum &= ~DISKO;
  }
  if(arac_durum&DISKO){

    arac_durum &= ~SINYAL;
  }


  if(SINYAL&arac_durum){
    simdiki_zaman=millis();
    if(simdiki_zaman - Onceki_zaman >= aralik){
      Onceki_zaman=simdiki_zaman;//150 msde bir yakıp sönsün
      if(!( arac_durum & LAMBA)){// lamba açık değilse açılsın !(0000 0000 & 0000 1000 ) !(0)
        arac_durum|=LAMBA;// durum= 0000 1000

        renk_ayarla(&leds,255,0,255);
        
      }
      else{
        arac_durum^=LAMBA;

        renk_ayarla(&leds,0,0,0);

      }
  }
}
  if(DISKO & arac_durum){
    simdiki_zaman1=millis();
    if(simdiki_zaman1 - Onceki_zaman1 >= gecikme){
      Onceki_zaman1=simdiki_zaman1;

      disko_asama++;

      if(disko_asama>3) disko_asama=1;

      switch (disko_asama)
      {
      case 1:
        renk_ayarla(&leds,255,0,0);
        break;
      
      case 2:
        renk_ayarla(&leds,0,255,0);
        break;
      case 3:
        renk_ayarla(&leds,0,0,255);
        break;
      }

    }
  }
  else{
    disko_asama=0;
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

          if(!(arac_durum&ENGEL_GORDUMU)&& arac_durum&VITES){// arac durum engel görmedi ve vites 1 
            if(currentLine.indexOf("GET /ileri")>=0){
              moveForward();
            }
            else if(currentLine.indexOf("GET /gear")>=0){
              arac_durum ^= VITES; // 0 ı 1 yapma
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
          else if(!(arac_durum&ENGEL_GORDUMU) && !(VITES&arac_durum)){
            if(currentLine.indexOf("GET /ileri")>=0){
              moveForward();
            }
            else if(currentLine.indexOf("GET /gear")>=0){
              arac_durum ^= VITES;
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
