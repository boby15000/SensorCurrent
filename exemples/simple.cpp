#include <Arduino.h>
#include <sensorCurrent.h>

// DEFINITION DES PINS
byte PIN_CAPTEUR_COURANT = A0;
byte SENSIBILITE_CAPT_ASC = 100;

sensorCurrent ACS712(PIN_CAPTEUR_COURANT, SENSIBILITE_CAPT_ASC, 1); 

void setup() {
  Serial.begin(9600);
}

void loop() {

  Serial.print("ADC Value : ");
  Serial.println(ACS712.GetADC());
  Serial.print("Courant Max : ");
  Serial.print(ACS712.GetCourantCrete());
  Serial.println("A");
  Serial.print("Courant Eff : ");
  Serial.print(ACS712.GetCourantEff());
  Serial.println("A");  
  Serial.print("Puissance : ");
  Serial.print(ACS712.GetPuissance());
  Serial.println("W");
  Serial.println("-----------------");
  delay(2000);
}