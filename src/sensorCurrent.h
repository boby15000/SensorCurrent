/**
 * @file sensorCurrent.h
 * @author Nicolas Fourgheon
 * @page https://github.com/boby15000/SensorCurrent
 * @brief Bibliothèque pour mesurer l'intensité du courant via différents capteurs (ACS712, SCT-013, etc.).
 * @version v2.0.0
 * @date 2024-08-11
 */

#ifndef SENSORCURRENT_H
#define SENSORCURRENT_H

#include <Arduino.h>

#define RESOLUTION_ADC    1023  // Résolution sur 10bits.
#define TENSION_ALIM      5.0   // Tension alimentation du capteur par défaut
#define FREQUENCE_RESEAU  50    // Tension alimentation du réseau par défaut.
#define TENSION_RESEAU    230   // Tension alimentation du réseau par défaut.
#define NBR_ECHANTILLON   500   // Nombre d'échantillons pour le calibrage du zéro.
#define N_MOYENNE         10    // Nombre d'échantillons pour la moyenne glissante.
#define FACTEUR_MINI      0.1   // Valeur minimum pour le facteur de correction.
#define FACTEUR_MAX       3.0   // Valeur maximum pour le facteur de correction.

class sensorCurrent {
  public:
    
    /**
     * @brief Initialise les paramètres du cpateur de courant.
     * @param pin_Capt Pin du capteur de courant.
     * @param sensibilite_Capt Sensibilité du capteur de courant en mV/A (ex SCT-013 : 100mV/A).
     * @param tensionAlim Tension d'alimentation du cpateur, par défaut 5,0V.
     * @param frequence Fréquence du réseau, par défaut et au minimum 50hz.
     */
    sensorCurrent(uint8_t pin_Capt, double sensibilite_Capt, double tensionAlim = TENSION_ALIM, double frequence = FREQUENCE_RESEAU);

    /**
     * @brief Effectue le calibrage du zéro.
     * @attention Cette fonction doit être appelée lorsque aucun courant ne circule, afin de calibrer le zéro (préférence dans le setup).
     */
    void CalibrationZero();

    /**
     * @brief Permet de corriger l'intensité mesurée.
     * @param facteur Facteur de correction appliqué à l'intensité mesurée (par défaut 1.0).
     * @details Un facteur de 1.0 signifie qu’aucune correction n’est appliquée.
     * @details La valeur du facteur doit être comprise entre 0.1 et 3.0.
     * @note La valeur minimale (0.1) permet de corriger une surestimation importante de la mesure.
     * @note Le facteur doit être déterminé en comparant la mesure réelle (référence) à celle fournie par le capteur :
     *        \n facteur = Valeur_référence / Valeur_mesurée
     */
    void Set_FacteurDeCorrection(double facteur = 1.0);

    /**
     * @brief Calcul la valeur Crête du Courant.
     * @return la valeur Crête du courant.
     */
    double GetCourantCrete(bool FacteurDeCorrection = true);

    /**
     * @brief Calcul la valeur Efficace du Courant.
     * @return la valeur Efficace du courant.
     */
    double GetCourantEff(bool FacteurDeCorrection = true);

    /**
     * @brief Calcul la puissance apparente.
     * @return la puissance apparente (VA).
     * @details la puissance est calculé depuis l'intensité efficace avec le facteur de correction.
     */
    double GetPuissanceApparente(int tension);  // tension en Volts


  private:
    uint8_t _PinSensor;                 ///< Pin du signal du capteur de courant.
    double _sensibilite_Capt;           ///< Sensibilité du capteur de courant en mV/A (ex SCT-013 : 100mV/A).
    double _FacteurDeCorrection = 1.0;  ///< Facteur de correction de l'intensité mesuré (entre 0.1 et 3.0).
    long _tensionAlimMilliVolt ;        ///< Tension d'alimentation du capteur en Volt.
    int _tensionMoyenneADC ;            ///< Par défaut 1/2 de Vcc.
    int _tensionAlimADC ;               ///< La tension Vcc transmise "tensionAlim"
    unsigned long _TpsDeMesure;         ///< Temps de mesure en µs correspondant à deux périodes (dépendant de la fréquence).

    int _bufferADC[N_MOYENNE] = {0};    ///< Buffer pour moyenne glissante.
    uint8_t _indexBuffer = 0;           ///< Index du buffer.
    bool _bufferRempli = false;         ///< Indique si le buffer est rempli (Si vrai, buffer rempli).
                         
    int moyenneGlissante(int nouvelleValeur);   ///< Retourne la valeur numérique moyenne glissante du courant
};
#endif