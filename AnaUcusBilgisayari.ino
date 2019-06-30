
//semruk-2 Ana ucus bilgisayari kaynak kodu

#include <SimpleKalmanFilter.h>
#include <SFE_BMP180.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <TinyGPS++.h>

#define YENILEME_HIZI 100

SimpleKalmanFilter filtre(1,1,0.1); // Kalman filtresi

SFE_BMP180 barometre;
TinyGPSPlus gps;
Servo motor1;
Servo motor2;
File dosya;

SoftwareSerial bluetoothSerial(3,4);
SoftwareSerial gpsSerial(5,6);

bool irtifaAzaliyor=0;
bool parasut1=0; // Suruklenme parasutu acildi mi?
bool parasut2=0; // Ana parasut acildi mi?

long yenilemeZamani;
long motor1Zaman;
long motor2Zaman;

float baslangicBasinci;
float maksIrtifa;
float eskiIrtifa;

int durum=0; // Parasut durumu (0: Acilmadi; 100: Suruklenme acildi; 200: Ikısı de acildi)

const int buzzer=2;


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
  bluetoothSerial.begin(9600);
  gpsSerial.begin(9600);

  pinMode(buzzer,OUTPUT);

  Serial.println("-----Yildiz Roket Takimi Semruk-2-----");


  // Barometre baslangic
  if (!barometre.begin()) {
    Serial.println("Barometre hatası!");
    digitalWrite(buzzer,HIGH);
    delay(3000);
    digitalWrite(buzzer,LOW);
    while(1); // Programı durdur.
  }
  else {
    Serial.println("Barometre baslatildi");
    digitalWrite(buzzer,HIGH);
    delay(1000);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
  }
  baslangicBasinci = basincOku();


  // SD kart baslangic
  Serial.println("SD kart baslatiliyor...");
  if (!SD.begin(8)) {
    Serial.println("SD Baslangici basarisiz!");
    digitalWrite(buzzer,HIGH);
    delay(3000);
    digitalWrite(buzzer,LOW);
  }
  else {
    Serial.println("SD Baslangici basarili.");
    digitalWrite(buzzer,HIGH);
    delay(1000);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
  }
}

void loop() {
  gpsSerial.listen();
  
  float basinc = basincOku();
  float irtifa = barometre.altitude(basinc,baslangicBasinci);
  float filtreliIrtifa = filtre.updateEstimate(irtifa);
  
  maksIrtifa= (filtreliIrtifa<maksIrtifa) ? maksIrtifa : filtreliIrtifa;

  if(!parasut1 && irtifaAzaliyor && filtreliIrtifa<maksIrtifa-15){
     motor1Zaman=millis();
     motor1.attach(9);
     motor1.write(0);
     durum=100;
     parasut1=1;
   }

  if(!parasut2 && parasut1 && filtreliIrtifa<600.0) {
    motor2Zaman=millis();
    motor2.attach(10);
    motor2.write(0);
    durum=200;
    parasut2=1;
  }

  if(parasut1 && motor1Zaman+500<millis()) {
    motor1.detach();
  }

  if(parasut2 && motor2Zaman+500<millis()) {
    motor2.detach();
  }

  irtifaAzaliyor = (filtreliIrtifa<eskiIrtifa) ? 1 : 0;
  eskiIrtifa=filtreliIrtifa;

  while (gpsSerial.available() > 0)
    gps.encode(gpsSerial.read());
    
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("GPS Bulunamadi, kablolamayi kontrol ediniz."));
    while(true);
  }

  if (millis() > yenilemeZamani) {
    bluetoothSerial.listen();
    
    dosya= SD.open("ucus.txt", FILE_WRITE);
    
    Serial.print(irtifa,6);
    Serial.print(",");
    Serial.print(filtreliIrtifa,6);
    Serial.print(",");
    Serial.print(durum);
    Serial.print(",");
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.println();
    
    dosya.print(millis());
    dosya.print(",");
    dosya.print(irtifa,6);
    dosya.print(",");
    dosya.print(filtreliIrtifa,6);
    dosya.print(",");
    dosya.print(durum);
    dosya.println();
    
    bluetoothSerial.print(filtreliIrtifa,6);
    bluetoothSerial.write(",");
    bluetoothSerial.print(gps.location.lat(), 6);
    bluetoothSerial.write(",");
    bluetoothSerial.print(gps.location.lng(), 6);
    bluetoothSerial.write(";");
    
    yenilemeZamani=millis() + YENILEME_HIZI;
    
    dosya.close();
  }
  
}
