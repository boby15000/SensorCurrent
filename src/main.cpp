#include <Arduino.h>
#include <sensorCurrent.h>

// DEFINITION DES PINS
byte PIN_CAPTEUR_COURANT = A0;
byte PIN_CAPTEUR_COURANT_ACS = A1;
byte SENSIBILITE_CAPT_SCT = 10;
byte SENSIBILITE_CAPT_ASC = 100;

sensorCurrent ACS712(PIN_CAPTEUR_COURANT_ACS, SENSIBILITE_CAPT_ASC, 1);
sensorCurrent SCT013(PIN_CAPTEUR_COURANT, SENSIBILITE_CAPT_SCT, 0); 


void setup() {
  Serial.begin(9600);
  SCT013.FacteurDeSensibilite=10;
  SCT013.FacteurDeCorrection = 1.48;
}

void loop() {

  Serial.print("Courant Eff ACS : ");
  Serial.print(ACS712.GetCourantEff());
  Serial.println("A");
  Serial.println("-----------------");
  Serial.print("Courant Eff SCT : ");
  Serial.print(SCT013.GetCourantEff());
  Serial.println("A");
  Serial.println("-----------------");
  Serial.println("-----------------");

  delay(2000);
}