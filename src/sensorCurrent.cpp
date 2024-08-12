#ifndef sensorCurrent_h
#include "sensorCurrent.h"
#endif

#include <Tempo.h>

const byte UNITE_VOLT = 1 ;
const byte UNITE_AMPERE = 2 ;
const int TENSION_MAX_ADC = 1024; 

Tempo minuteur(1);

int valueADC;

/**
 * Constructeur
 * @pin_sensor : entrée analogique du capteur
 * @sensibilite : sensibilité du capteur
 * @unite_sensibilite : unité de la sensibilité Volt=1 ou Ampère=0
 * @frequence : fréquence du réseau mesuré (par défaut 50hz)
 * @tension : tension du réseau mesuré (par défaut 230V)
*/
sensorCurrent::sensorCurrent(byte pin_sensor, float sensibilite, byte unite_sensibilite, float frequence, float tension){
   this->_PinSensor = pin_sensor;
   this->_Sensibilite= sensibilite;
   this->_UniteSensibilite = unite_sensibilite;
   long Echantillonnage = (1/max(frequence, 50))*1000;
   minuteur.SetSeuil(0,Echantillonnage);
   this->_Tension = tension;
   this->_TensionRef = TENSION_MAX_ADC/2;
}

/**
 * Void
 * Lecture de l'entrée analogique du capteur de courant sans circulation de courant
*/
void sensorCurrent::Etalonnage(){
    this->_TensionRef = this->ReadingSensor();
}

/**
 * Void
 * Retourne la valeur de l'entrée analogique brute du capteur de courant
*/
int sensorCurrent::GetADC(){
    return this->ReadingSensor();
}

/**
 * Function
 * Return : la valeur Crete du Courant
 * Nota : Prend en compte le Facteur de Sensibilité pour ajuster le Zéro à vide et le Facteur de Correction par ajuster l'intensité en charge.
*/
float sensorCurrent::GetCourantCrete(){
    int valueTension = this->ReadingSensor();
    int tensionCaptADC = abs(valueTension-this->_TensionRef);
    if ( tensionCaptADC <= this->FacteurDeSensibilite ) return 0;
    switch (this->_UniteSensibilite)
    {
    case 1:
        /* Sensibilité en Volt */
        //return tensionCaptADC;
        return (tensionCaptADC)/((this->_Sensibilite/1000)*TENSION_MAX_ADC/5)*this->FacteurDeCorrection;
        break;
        /* Sensibilité en Ampere */
    default:
        return (tensionCaptADC)*(this->_Sensibilite*5/TENSION_MAX_ADC)*this->FacteurDeCorrection;
        break;
    }
}

/**
 * Function
 * Return : la valeur Efficace du Courant
*/
float sensorCurrent::GetCourantEff(){
    return this->GetCourantCrete()/sqrt(2);
}

/**
 * Function
 * Return : puissance consommé
*/
float sensorCurrent::GetPuissance(){
    return (this->GetCourantEff() * this->_Tension);
}

/**
 * Function
 * Return : la valeur numérique du Courant
*/
int sensorCurrent::ReadingSensor(){
    int valeur = 0;
    int valeurMax = 0;
    minuteur.SetActif(0,true);
    while ( !minuteur.GetEnd(0) ){
        valeur = analogRead( this->_PinSensor );
        valeurMax = max(valeurMax , valeur);
    }
    if ( abs(valeurMax - valueADC) > this->FacteurDeSensibilite ) valueADC = valeurMax;
    return valueADC;
}