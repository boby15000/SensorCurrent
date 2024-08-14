#include <Arduino.h>
#include <sensorCurrent.h>

// DEFINITION DES PINS
byte PIN_CAPTEUR_COURANT_ACS = A0;
byte PIN_CAPTEUR_COURANT_SCT = A1;
byte SENSIBILITE_CAPT_ASC = 100;
byte SENSIBILITE_CAPT_SCT = 10;

sensorCurrent ACS712(PIN_CAPTEUR_COURANT_ACS, SENSIBILITE_CAPT_ASC, ACS712.MILLIVOLT_PAR_AMPERE);
sensorCurrent SCT013(PIN_CAPTEUR_COURANT_SCT, SENSIBILITE_CAPT_SCT, SCT013.AMPERE_PAR_VOLT); 


void setup() {
  Serial.begin(9600);
  ACS712.Etalonnage();
  SCT013.Etalonnage();
  ACS712.FacteurDeCorrectionDuZero(4);
  SCT013.FacteurDeCorrectionACharge(1.85);
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