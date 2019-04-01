#include <SimpleKalmanFilter.h>
#include <SFE_BMP180.h>
#include <math.h>
#include <SPI.h>
#include <SD.h>

class Parasut{
  public:
    long baslangicZamani;
    int otmeSayisi;
    int otmeSuresi;
    int beklemeSuresi;

    Parasut(int otmeSayi, int otmeSure, int beklemeSure) {
      this->baslangicZamani=0;
      this->otmeSayisi=otmeSayi;
      this->otmeSuresi=otmeSure;
      this->beklemeSuresi=beklemeSure;
    }

    void start(){
      digitalWrite(3,HIGH);
      baslangicZamani=millis();
    }

    void update(){
      if(millis()>otmeSayisi*otmeSuresi+(otmeSayisi-1)*beklemeSuresi+baslangicZamani||baslangicZamani==0){
        digitalWrite(3,LOW);
      }
      else if((millis()-baslangicZamani)%(otmeSuresi+beklemeSuresi)<otmeSuresi){
        digitalWrite(3,HIGH);
      }
      else{
        digitalWrite(3,LOW);
      }
    }
};

SimpleKalmanFilter filtre(1,1,0.1); // Kalman filtresi
SFE_BMP180 barometre; //Barometre
File dosya;
Parasut para1(1,1000,0);
Parasut para2(2,500,300);
int durum = 0;

const long YINELEME_HIZI = 100; //Seri haberleşme yenileme hızı
long yinelemeZamani; 
long zilZamani=0; // Zil kaç milisaniyedir ötüyor
float baslangicBasinci;
float maksIrtifa;
float eskiIrtifa; 
bool azaliyor=0; // Irtifa azaliyor mu?
bool zil=0; // Zil ottu mu?
bool parasut=0; // Parasut açildl mi?

double basincOku() {
  double sicaklik, basinc;
  delay(barometre.startTemperature());
  barometre.getTemperature(sicaklik);
  delay(barometre.startPressure(3));
  barometre.getPressure(basinc,sicaklik);
  return basinc;
}

void setup() {
  Serial.begin(9600);

  // Barometre baslangic
  if (!barometre.begin()) {
    Serial.println("Barometre hatası!");
    while(1); // Programı durdur.
  }
  baslangicBasinci = basincOku();

  // SD kart baslangic
  Serial.print("SD kart baslatiliyor...");
  if (!SD.begin(4)) {
    Serial.println("Baslangic basarisiz!");
    //while (1);
  }
  else {
    Serial.println("Baslangic basarili.");
  }

  pinMode(3,OUTPUT); // Parasutu simgeleyen buzzer
}

void loop() {
  float basinc = basincOku();
  float irtifa = barometre.altitude(basinc,baslangicBasinci);
  float filtreliIrtifa = filtre.updateEstimate(irtifa);
  maksIrtifa= (filtreliIrtifa<maksIrtifa) ? maksIrtifa : filtreliIrtifa;

  para1.update();
  para2.update();
  
  if(!zil&&azaliyor&&filtreliIrtifa<maksIrtifa-1){
    para1.start();
    zil=1;
    durum=1;
    }

  if(!parasut&&zil&&filtreliIrtifa<4.0) {
    para2.start();
    parasut=1;
    durum=2;
    }

  if(filtreliIrtifa<eskiIrtifa) {
    azaliyor=1;
    }
  else {
     azaliyor=0;
    }
  eskiIrtifa=filtreliIrtifa;

  if (millis() > yinelemeZamani) {
    dosya= SD.open("ucus.txt", FILE_WRITE);
    Serial.print(irtifa,6);
    Serial.print(",");
    Serial.print(filtreliIrtifa,6);
    Serial.print(",");
    Serial.print(durum);
    Serial.println();
    dosya.print(millis());
    dosya.print(",");
    dosya.print(irtifa,6);
    dosya.print(",");
    dosya.print(filtreliIrtifa,6);
    dosya.print(",");
    dosya.print(durum);
    dosya.println();
    yinelemeZamani=millis()+YINELEME_HIZI;
    dosya.close();
  }
}
