/**
 * @file sensorCurrent.cpp
 * @author Nicolas Fourgheon
 * @page https://github.com/boby15000/SensorCurrent
 * @brief Bibliothèque pour mesurer l'intensité du courant via différents capteurs (ACS712, SCT-013, etc...).
 * @version v2.1.0
 * @date 2024-08-11
 */

#include "sensorCurrent.h"
#include <Arduino.h>

/**
 * @brief Initialise les paramètres du cpateur de courant.
 * @param pin_Capt Pin du capteur de courant.
 * @param sensibilite_Capt Sensibilité du capteur de courant en mV/A (ex SCT-013 : 100mV/A).
 * @param tensionAlim Tension d'alimentation du cpateur, par défaut 5,0V.
 * @param frequence Fréquence du réseau, par défaut 50hz.
 */
sensorCurrent::sensorCurrent(uint8_t pin_Capt, double sensibilite_Capt, double tensionAlim, double frequence){
    this->_PinSensor = pin_Capt;
    this->_sensibilite_Capt = sensibilite_Capt;
    this->_tensionAlimMilliVolt = tensionAlim*1000; // tension d'alimention en MilliVolt.
    this->_tensionAlimADC = round(tensionAlim*RESOLUTION_ADC)/TENSION_ALIM; // calcul la tension d'alimention en ADC (par défaut 5V soit 1023).
    this->_tensionMoyenneADC = round(this->_tensionAlimADC/2); // calcul la tension moyenne en ADC (par défaut 2.5V soit 512).
    this->_TpsDeMesure = (1000000.0 / max(frequence, FREQUENCE_RESEAU))*2; // Conversion en µs de la période x 2 , pour deux périodes (réseau minimum 50Hz).  
}


/**
 * @brief Effectue le calibrage du zéro.
 * @attention Cette fonction doit être appelée lorsque aucun courant ne circule, afin de calibrer le zéro (préférence dans le setup).
 */
void sensorCurrent::CalibrationZero(){
  long somme = 0;
  for (int i = 0; i < NBR_ECHANTILLON ; i++) {
    somme += analogRead(this->_PinSensor);
  }
  this->_tensionMoyenneADC = round(somme / NBR_ECHANTILLON);
}


/**
 * @brief Permet de corriger l'intensité mesuré
 * @param facteur utilisé pour corriger l'intensité mesurée (par défaut 1).
 * @details Un facteur de 1 signifie qu’aucune correction n’est appliquée à l’intensité mesurée.
 * @details Plage pour la valeur de Facteur : 0.1 à 3.0.
 * @details facteur = Valeur "Métrix" / Valeur Mesuré
 */
void sensorCurrent::Set_FacteurDeCorrection(double facteur){
  this->_FacteurDeCorrection = constrain(facteur, FACTEUR_MINI, FACTEUR_MAX);
}


/**
 * @brief Calcul la valeur Crête du Courant.
 * @return la valeur Crête du courant.
 */
double sensorCurrent::GetCourantCrete(bool FacteurDeCorrection){
  unsigned long start = micros();
  double maxCurrent = 0.0;

  while (micros() - start < this->_TpsDeMesure) {
    int adc = this->moyenneGlissante(abs(analogRead(this->_PinSensor)-this->_tensionMoyenneADC));
    double milliVolt = (((double)adc * this->_tensionAlimMilliVolt) / (double)this->_tensionAlimADC);
    double current = (milliVolt / this->_sensibilite_Capt);
    maxCurrent = max(current, maxCurrent);
  }
  maxCurrent = (maxCurrent >= (IntensiteMin*sq(2))) ? maxCurrent : 0.0;
  return (FacteurDeCorrection) ? maxCurrent * this->_FacteurDeCorrection : maxCurrent ;
}


/**
 * @brief Calcul la valeur Efficace du Courant.
 * @return la valeur Efficace du courant.
 */
double sensorCurrent::GetCourantEff(bool FacteurDeCorrection){
  unsigned long start = micros();
  double sumSq = 0.0;
  int count = 0;

  while (micros() - start < this->_TpsDeMesure) {
    int adc = this->moyenneGlissante(abs(analogRead(this->_PinSensor)-this->_tensionMoyenneADC));
    double milliVolt = (((double)adc * this->_tensionAlimMilliVolt) / (double)this->_tensionAlimADC);
    double current = (milliVolt / this->_sensibilite_Capt);
    sumSq += sq(current);
    count++;
  }
  double meanSq = sumSq / count;
  double Current = (sqrt(meanSq) >= IntensiteMin) ? sqrt(meanSq) : 0.0;
  return (FacteurDeCorrection) ? Current * this->_FacteurDeCorrection : Current ; 
}


/**
 * @brief Calcul la puissance apparente.
 * @return la puissance apparente (VA).
 * @details la puissance est calculé depuis l'intensité efficace avec le facteur de correction.
 */
double sensorCurrent::GetPuissanceApparente(int tension){
    return (double)tension * this->GetCourantEff();
}


/**
 * @brief Calcule le facteur de sensibilité du capteur (ex : SCT-0013 0XX)
 * @param intensiteMesure Intensité réelle de l'équipement mesuré par Pinceampéremètrique ou Metrix.
 * @param tensionCalcule Tension récupérer par la fonction "GetCourantToVolt" lorsque l'appareil est en charge équivalent à l'Intensité réelle de l'équipement (variable ci-dessus)
 * @return le facteur de sensibilité du capteur en MilliVolt 
 */
double  sensorCurrent::GetFacteurDeSensibilite(double intensiteMesure, double tensionCalcule){
  return (tensionCalcule*1000)/intensiteMesure;
}


/**
 * @brief Calcul la Tension image du Courant Efficace.
 * @return la Tension en Volt.
 */
double sensorCurrent::GetCourantToVolt(){
  unsigned long start = micros();
  double sumSq = 0.0;
  int count = 0;

  while (micros() - start < this->_TpsDeMesure) {
    int adc = this->moyenneGlissante(abs(analogRead(this->_PinSensor)-this->_tensionMoyenneADC));
    double milliVolt = round(((double)adc * this->_tensionAlimMilliVolt) / (double)this->_tensionAlimADC);
    sumSq += sq(milliVolt);
    count++;
  }
  double meanSq = sumSq / count;
  return sqrt(meanSq)/1000 ; 
}



/**
 * @brief Filtrage numérique au capteur de courant pour stabiliser la lecture et améliorer la précision, notamment en atténuant les pics aléatoires dus au bruit de mesure.
 * @return une valeur ADC Filtré.
 */
int sensorCurrent::moyenneGlissante(int nouvelleValeur) {
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