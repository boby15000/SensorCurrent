#ifndef sensorCurrent_h
#include "sensorCurrent.h"
#endif

#include <AntiDelay.h>

AntiDelay Minuteur(50);
AntiDelay Minuteur2(20);

int valueADC;

/**
 * Constructeur
 * @pin_sensor : entrée analogique du capteur
 * @sensibilite : sensibilité du capteur
 * @type_sensibilite : Type de la sensibilité AMPERE_PAR_VOLT ou MILLIVOLT_PAR_AMPERE
 * @frequence : fréquence du réseau mesuré (par défaut 50hz)
 * @tension : tension du réseau mesuré (par défaut 230V)
*/
sensorCurrent::sensorCurrent(byte pin_sensor, double sensibilite_Capt, byte type_sensibilite, double frequence, double tension){
    this->_PinSensor = pin_sensor;
    this->_Sensibilite = sensibilite_Capt;
    this->_type_sensibilite = type_sensibilite;
    long Echantillonnage = abs((1/max(frequence, 50))*1000);
    Minuteur2.setInterval(Echantillonnage);
    this->_Tension = tension;
    this->_TensionRef = TENSION_MAX_ADC/2;
}

/**
 * Void
 * Etalonnage du zéro courant : A FAIRE HORS CHARGE
*/
void sensorCurrent::Etalonnage(){
    int valeur = 0;
    int valeurMax = 0;
    Minuteur.reset();
    while ( !Minuteur ){
        valeur = analogRead( this->_PinSensor );
        valeurMax = max(valeurMax , valeur);
    }
    this->_TensionRef = valeurMax;
}

/**
 * Void
 * Modifie le facteur de Sensibilité
*/
void sensorCurrent::FacteurDeCorrectionDuZero(byte value){
    this->_FacteurDeCorrectionDuZero = max(value, 0);
}

/**
 * Void
 * Modifie le facteur de Correction
*/
void sensorCurrent::FacteurDeCorrectionACharge(double value){
    this->_FacteurDeCorrectionACharge = max(value, 1);
}

/**
 * Void
 * Retourne la valeur de l'entrée analogique du capteur de courant (sans filtrage par défaut)
*/
int sensorCurrent::GetADC(bool filtrage){
    int valueTension = this->ReadingSensorAC();
    if ( filtrage ) return abs(valueTension-this->_TensionRef) ;
    return valueTension;
}

/**
 * Function
 * Return : la valeur Crete du Courant
 * Nota : Prend en compte le Facteur de Sensibilité pour ajuster le Zéro à vide et le Facteur de Correction par ajuster l'intensité en charge.
*/
double sensorCurrent::GetCourantCrete(){
    int tensionCaptADC = this->GetADC(true);
    if ( this->_type_sensibilite == this->MILLIVOLT_PAR_AMPERE )
    {
        return ((float(tensionCaptADC) * 5 / float(this->TENSION_MAX_ADC)) /   float(this->_Sensibilite/1000) * float(this->_FacteurDeCorrectionACharge)); /* Sensibilité en Millivolt par Ampère */
    }
    else
    {
        return ((float(tensionCaptADC) * 5 / float(this->TENSION_MAX_ADC)) * float(this->_Sensibilite) * this->_FacteurDeCorrectionACharge );  /* Sensibilité en Ampère par Volt */
    }
}

/**
 * Function
 * Return : la valeur Efficace du Courant
*/
double sensorCurrent::GetCourantEff(){
    return this->GetCourantCrete()/sqrt(2);
}

/**
 * Function
 * Return : puissance consommé
*/
double sensorCurrent::GetPuissance(){
    return (this->GetCourantEff() * this->_Tension);
}

/**
 * Function
 * Return : la valeur numérique du Courant
*/
int sensorCurrent::ReadingSensorAC(){
    int valeur = 0;
    int valeurMax = 0;
    Minuteur2.reset();
    while ( !Minuteur2 ){
        valeur = analogRead( this->_PinSensor );
        valeurMax = max(valeurMax , valeur);
    }
    if ( abs(valeurMax - valueADC) > this->_FacteurDeStabilisation ) valueADC = valeurMax ;
    if ( valueADC <= (this->_TensionRef+this->_FacteurDeCorrectionDuZero) ) valueADC = this->_TensionRef;
    return valueADC;
}