#include <sensorCurrent.h>

// DEFINITION DES PINS
uint8_t PIN_CAPTEUR_COURANT_SCT = A1;

// DEFINITION SENSIBILITE DES CAPTEURS
double SENSIBILITE_CAPT = 120;

sensorCurrent SCT013(PIN_CAPTEUR_COURANT_SCT, SENSIBILITE_CAPT);

void setup() {
  // Pour le Debug
  Serial.begin(9600);
  
  // Calibration du Capteur 
  SCT013.CalibrationZero(); // A réaliser hors circulation de courant

  // Définition du seuil de détection de courant (par défaut : 0.10 A)
  // Toute valeur mesurée en dessous de ce seuil sera considérée comme nulle.
  SCT013.IntensiteMin = 0.15;

  // Modification du Facteur de Correction (si besoin)
  //SCT013.Set_FacteurDeCorrection(1.0); //valeur à corriger selon la formule : facteur = Valeur "Métrix" / Valeur Mesuré
}

void loop() {
  
/*
  *** Détermination du facteur de sensibilité du capteur ***
  - La sensibilité réelle d’un capteur peut légèrement différer des spécifications constructeur.
  - Utiliser cette fonction lorsque le courant réel est connu (ex. mesuré avec un multimètre).
  - Passer ce courant réel en paramètre (par exemple : 8.6 A) afin de calculer la sensibilité effective en mV/A.
  - La valeur de tension mesurée est obtenue via la fonction GetCourantToVolt().

  Exemple :
  Serial.print("Facteur de sensibilité mesuré : ");
  Serial.println(SCT013.GetFacteurDeSensibilite(8.6, SCT013.GetCourantToVolt()));
*/
  
  // Affiche la valeur du courant
  Serial.print("Valeur du Courant Crête : ");
  Serial.print(SCT013.GetCourantCrete());
  Serial.println(" A");
  Serial.print("Valeur du Courant Efficace : ");
  Serial.print(SCT013.GetCourantEff());
  Serial.println(" A");
  Serial.print("Valeur de la Puissance : ");
  Serial.print(SCT013.GetPuissanceApparente(230));
  Serial.println(" W");

  Serial.println("-----------------");
  Serial.println("-----------------");

  delay(2000);
}