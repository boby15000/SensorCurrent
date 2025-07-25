#include <sensorCurrent.h>

// DEFINITION DES PINS
uint8_t PIN_CAPTEUR_COURANT_ACS = A0;
uint8_t PIN_CAPTEUR_COURANT_SCT = A1;

// DEFINITION SENSIBILITE DES CAPTEURS
uint8_t SENSIBILITE_CAPT = 100;


sensorCurrent ACS712(PIN_CAPTEUR_COURANT_ACS, SENSIBILITE_CAPT);
//sensorCurrent ACS712(PIN_CAPTEUR_COURANT_ACS, SENSIBILITE_CAPT,4.60, 50);
sensorCurrent SCT013(PIN_CAPTEUR_COURANT_SCT, SENSIBILITE_CAPT); 

void setup() {
  // Pour le Debug
  Serial.begin(9600);
  
  // Calibration du Capteur 1
  ACS712.CalibrationZero(); // A réaliser hors circulation de courant
  ACS712.Set_FacteurDeCorrection(1.23); // valeur à corriger via : facteur = Valeur "Métrix" / Valeur Mesuré
 
  // Calibration du Capteur 2
  SCT013.CalibrationZero(); // A réaliser hors circulation de courant
  SCT013.Set_FacteurDeCorrection(2.15); // valeur à corriger via : facteur = Valeur "Métrix" / Valeur Mesuré
}

void loop() {
   

    Serial.print("ACS712 Courant : ");
    Serial.print(ACS712.GetCourantEff());
    Serial.println(" A");
    Serial.print("ACS712 Puissance : ");
    Serial.print(ACS712.GetPuissanceApparente(230));
    Serial.println(" W");

    Serial.println(" --- ");

    Serial.print("SCT013 Courant : ");
    Serial.print(SCT013.GetCourantEff());
    Serial.println(" A");
    Serial.print("SCT013 Puissance : ");
    Serial.print(SCT013.GetPuissanceApparente(230));
    Serial.println(" W");

    Serial.println("-----------------");
    Serial.println("-----------------");

    delay(2000);
}