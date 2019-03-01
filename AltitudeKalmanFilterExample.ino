#include <SimpleKalmanFilter.h>
#include <SFE_BMP180.h>
#include <math.h>

/*
 This sample code demonstrates how the SimpleKalmanFilter object can be used with a
 pressure sensor to smooth a set of altitude measurements. 
 This example needs a BMP180 barometric sensor as a data source.
 https://www.sparkfun.com/products/11824

 SimpleKalmanFilter(e_mea, e_est, q);
 e_mea: Measurement Uncertainty 
 e_est: Estimation Uncertainty 
 q: Process Noise
 */
SimpleKalmanFilter pressureKalmanFilter(1, 1, 0.1);

SFE_BMP180 pressure;

// Serial output refresh time
const long SERIAL_REFRESH_TIME = 100;
long refresh_time;
long buzzerTime=0;

float baseline; // baseline pressure
float maxAltitude=0;
float oldAltitude=-1000;
bool azaliyor=0;
bool buzzer=0;
bool para=0;

double getPressure() {
  char status;
  double T,P;
  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0) {
      status = pressure.startPressure(3);
      if (status != 0) {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0) {
          return(P);
        }
      } 
    }  
  } 
}

void setup() {

  Serial.begin(115200);

  // BMP180 Pressure sensor start
  if (!pressure.begin()) {
    Serial.println("BMP180 Pressure Sensor Error!");
    while(1); // Pause forever.
  }
  baseline = getPressure();

  pinMode(3,OUTPUT);
}

void loop() {
  
  float p = getPressure();
  float altitude = pressure.altitude(p,baseline);
  float estimated_altitude = pressureKalmanFilter.updateEstimate(altitude);
  maxAltitude= (estimated_altitude<maxAltitude) ? maxAltitude : estimated_altitude;

  if(!buzzer&&azaliyor&&estimated_altitude<maxAltitude-1){
    digitalWrite(3,HIGH);
    buzzerTime=millis();
    buzzer=1;
    //delay(2000);
    //digitalWrite(3,LOW);
    }

  if(!para&&buzzer&&estimated_altitude<4.0) {
    digitalWrite(3,HIGH);
    delay(200);
    digitalWrite(3,LOW);
    delay(500);
    digitalWrite(3,HIGH);
    delay(200);
    digitalWrite(3,LOW);
    para=1;
    }
  
  if(estimated_altitude<oldAltitude) {
    azaliyor=1;
    }
    else {
     azaliyor=0;
    }
  oldAltitude=estimated_altitude;

  if(buzzer&&millis()>buzzerTime+2000) {
    digitalWrite(3,LOW);
    }
  
  if (millis() > refresh_time) {
    Serial.print(altitude,6);
    Serial.print(",");
    Serial.print(estimated_altitude,6);
    Serial.println();
    refresh_time=millis()+SERIAL_REFRESH_TIME;
  }
  
}
