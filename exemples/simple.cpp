#include <Arduino.h>
#include <sensorCurrent.h>

// DEFINITION DES PINS
byte PIN_CAPTEUR_COURANT = A0;
byte SENSIBILITE_CAPT_ASC = 100;

sensorCurrent ACS712(PIN_CAPTEUR_COURANT, SENSIBILITE_CAPT_ASC, ACS712.MILLIVOLT_PAR_AMPERE); 

void setup() {
  Serial.begin(9600);
  ACS712.Etalonnage(); // Permet d'étalonner le Zéro du capteur.
  //ACS712.FacteurDeCorrectionDuZero(4); A utiliser pour corriger le Zéro du capteur. Utiliser la fonction ACS712.GetADC(true) pour obtenir la valeur de correction à utiliser. Par défaut 0
  //ACS712.FacteurDeCorrectionACharge(1.85); A utiliser pour corriger l'intensité lu par le capteur. Par défaut 1 ; intensité lu x 1
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