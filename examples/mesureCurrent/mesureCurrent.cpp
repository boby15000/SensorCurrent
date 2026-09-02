#include <sensorCurrent.h>

// DEFINITION DES PINS
uint8_t PIN_CAPTEUR_COURANT_SCT = A1;

// DEFINITION SENSIBILITE DU CAPTEUR
// SCT-013-010 : sortie 1V RMS pour 10A RMS (résistance de charge intégrée) => 1000mV / 10A = 100 mV/A
double SENSIBILITE_CAPT = 100;

sensorCurrent SCT013(PIN_CAPTEUR_COURANT_SCT, SENSIBILITE_CAPT);

void setup() {
  // Pour le Debug
  Serial.begin(9600);

  // Calibration du Capteur
  SCT013.calibrerZero(); // A réaliser hors circulation de courant

  // Correction d'un écart systématique constaté par rapport à une référence (pince ampèremétrique) :
  // ajuster directement SENSIBILITE_CAPT ci-dessus, plutôt qu'un facteur appliqué après coup.
  // nouvelleSensibilite = SENSIBILITE_CAPT x (courantLuParLeCapteur / courantReel)
}

void loop() {

  // Une seule mesure combinée (crête + efficace) : deux fois plus rapide que deux appels séparés.
  auto mesure = SCT013.lireCourant();
  double puissanceApparente = 220.0 * mesure.efficace;

  // ESTIMATION (pas une mesure) : suppose cos(phi) = FACTEUR_PUISSANCE_DEFAUT (0.93, profil
  // résidentiel français). À ajuster si le cos(phi) réel de l'installation est connu.
  double puissanceActive = puissanceApparente * FACTEUR_PUISSANCE_DEFAUT;

  // Affiche la valeur du courant
  Serial.print("Valeur du Courant Crête : ");
  Serial.print(mesure.crete);
  Serial.println(" A");
  Serial.print("Valeur du Courant Efficace : ");
  Serial.print(mesure.efficace);
  Serial.println(" A");
  Serial.print("Valeur de la Puissance Apparente : ");
  Serial.print(puissanceApparente);
  Serial.println(" VA");
  Serial.print("Valeur de la Puissance Active (estimée, cosphi=0.93) : ");
  Serial.print(puissanceActive);
  Serial.println(" W");

  Serial.println("-----------------");
  Serial.println("-----------------");

  delay(2000);
}