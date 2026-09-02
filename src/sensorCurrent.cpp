/**
 * @file sensorCurrent.cpp
 * @author Nicolas Fourgheon
 * @page https://github.com/boby15000/SensorCurrent
 * @brief Bibliothèque pour mesurer l'intensité du courant via différents capteurs (ACS712, SCT-013, etc...).
 * @version v4.1.0
 * @date 2026-09-02
 */

#include "sensorCurrent.h"
#include <Arduino.h>

/**
 * @brief Initialise les paramètres du capteur de courant.
 */
sensorCurrent::sensorCurrent(uint8_t pinCapteur, double sensibiliteCapteur, double frequence){
    this->_pinCapteur = pinCapteur;
    this->_sensibiliteCapteur = sensibiliteCapteur;
    this->_tensionMoyenneADC = round(RESOLUTION_ADC / 2.0); // point milieu par défaut (Vcc/2).

    double frequenceEffective = max(frequence, (double)FREQUENCE_RESEAU_DEFAUT); // réseau minimum 50Hz.
    this->_dureeMesureMicros = (unsigned long)((1000000.0 / frequenceEffective) * NB_PERIODES_MESURE);
    this->_dureeCalibrationMicros = (unsigned long)((1000000.0 / frequenceEffective) * NB_PERIODES_CALIBRATION);
}


/**
 * @brief Calibre le point milieu (zéro) du capteur.
 */
void sensorCurrent::calibrerZero(){
  unsigned long start = micros();
  long somme = 0;
  long nbEchantillons = 0;

  while (micros() - start < this->_dureeCalibrationMicros) {
    somme += analogRead(this->_pinCapteur);
    nbEchantillons++;
  }
  this->_tensionMoyenneADC = round((double)somme / nbEchantillons);
}


/**
 * @brief Mesure simultanément le courant crête et efficace, en un seul passage d'échantillonnage.
 */
sensorCurrent::MesureCourant sensorCurrent::lireCourant(){
  unsigned long start = micros();
  int adcMax = 0;
  unsigned long long sommeCarresADC = 0;
  long nbEchantillons = 0;

  // Tout se calcule en comptes ADC bruts (entiers, sans division) ; la conversion en Ampères
  // ne s'applique qu'une seule fois à la fin, sur la crête et l'efficace déjà calculés.
  while (micros() - start < this->_dureeMesureMicros) {
    int adcFiltre = this->filtrerMoyenneGlissante(analogRead(this->_pinCapteur) - this->_tensionMoyenneADC);
    int adcAbs = abs(adcFiltre);
    adcMax = max(adcAbs, adcMax);
    sommeCarresADC += (unsigned long long)adcAbs * adcAbs;
    nbEchantillons++;
  }

  double facteurConversion = TENSION_REF_MILLIVOLT / (RESOLUTION_ADC * this->_sensibiliteCapteur); // A par compte ADC

  MesureCourant mesure;
  mesure.crete = adcMax * facteurConversion;
  mesure.efficace = sqrt((double)sommeCarresADC / nbEchantillons) * facteurConversion;
  return mesure;
}


/**
 * @brief Mesure la valeur crête du courant.
 */
double sensorCurrent::lireCourantCrete(){
  return this->lireCourant().crete;
}


/**
 * @brief Mesure la valeur efficace (RMS) du courant.
 */
double sensorCurrent::lireCourantEfficace(){
  return this->lireCourant().efficace;
}


/**
 * @brief Mesure la puissance apparente.
 * @details S = tension x intensité efficace.
 */
double sensorCurrent::lirePuissanceApparente(double tension){
  return tension * this->lireCourantEfficace();
}


/**
 * @brief Estime la puissance active, à partir d'un facteur de puissance supposé.
 * @details P = tension x intensité efficace x cos(phi). Approximation, pas une mesure.
 */
double sensorCurrent::lirePuissanceActive(double tension, double facteurPuissance){
  return tension * this->lireCourantEfficace() * facteurPuissance;
}


/**
 * @brief Estime la puissance réactive, à partir du même facteur de puissance supposé.
 * @details Q = tension x intensité efficace x sin(phi), avec sin(phi) déduit de cos(phi).
 */
double sensorCurrent::lirePuissanceReactive(double tension, double facteurPuissance){
  double sinPhi = sqrt(max(0.0, 1.0 - sq(facteurPuissance)));
  return tension * this->lireCourantEfficace() * sinPhi;
}


/**
 * @brief Filtrage numérique du capteur de courant pour stabiliser la lecture et améliorer la
 *        précision, notamment en atténuant les pics aléatoires dus au bruit de mesure.
 * @return une valeur ADC filtrée.
 */
int sensorCurrent::filtrerMoyenneGlissante(int nouvelleValeur) {
  _bufferADC[_indexBuffer++] = nouvelleValeur;

  if (_indexBuffer >= N_MOYENNE) {
    _indexBuffer = 0;
    _bufferRempli = true;
  }

  int somme = 0;
  int taille = _bufferRempli ? N_MOYENNE : _indexBuffer;

  for (int i = 0; i < taille; i++) {
    somme += _bufferADC[i];
  }

  return somme / taille;
}
