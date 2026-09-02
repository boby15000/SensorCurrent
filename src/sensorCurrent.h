/**
 * @file sensorCurrent.h
 * @author Nicolas Fourgheon
 * @page https://github.com/boby15000/SensorCurrent
 * @brief Bibliothèque pour mesurer l'intensité du courant via différents capteurs (ACS712, SCT-013, etc...).
 * @version v4.1.0
 * @date 2026-09-02
 */

#ifndef SENSORCURRENT_H
#define SENSORCURRENT_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32) || defined(ESP32)
  #define RESOLUTION_ADC 4095  // ESP32 : ADC 12 bits par défaut (0-4095).
#else
  #define RESOLUTION_ADC 1023  // AVR, ESP8266, ATtiny... : ADC 10 bits (0-1023).
#endif

#define TENSION_REF_MILLIVOLT   5000.0 // Référence utilisée pour convertir l'ADC en mV avant application de sensibiliteCapteur.
#define FREQUENCE_RESEAU_DEFAUT 50.0  // Fréquence réseau par défaut, et plancher minimum autorisé (Hz).
#define TENSION_RESEAU_DEFAUT   220   // Tension réseau par défaut pour le calcul de puissance (V).
#define FACTEUR_PUISSANCE_DEFAUT 0.93 // cos(phi) par défaut : valeur usuelle pour une habitation française à chauffage résistif dominant. Approximation, pas une mesure — à ajuster selon l'installation réelle.
#define NB_PERIODES_MESURE      10    // Nombre de périodes réseau échantillonnées pour la crête/l'efficace (~200ms à 50Hz).
#define NB_PERIODES_CALIBRATION 20    // Nombre de périodes réseau échantillonnées pour calibrer le zéro.
#define N_MOYENNE                5    // Nombre d'échantillons pour la moyenne glissante.

class sensorCurrent {
  public:

    /**
     * @brief Résultat d'une mesure combinée (crête + efficace).
     */
    struct MesureCourant {
      double crete;     ///< Valeur crête du courant (A).
      double efficace;  ///< Valeur efficace (RMS) du courant (A).
    };

    /**
     * @brief Initialise les paramètres du capteur de courant.
     * @param pinCapteur Pin analogique du capteur de courant.
     * @param sensibiliteCapteur Sensibilité du capteur de courant en mV/A (ex SCT-013 : 100mV/A),
     *        pour une référence ADC de 5V.
     * @param frequence Fréquence du réseau, par défaut et au minimum 50Hz.
     * @note Pour corriger un écart systématique constaté par rapport à une référence (pince
     *       ampèremétrique, multimètre) — y compris un Vcc réel différent de 5V — ajustez
     *       directement sensibiliteCapteur plutôt que d'appliquer un facteur après coup :
     *       \n nouvelleSensibilite = sensibiliteCapteur x (courantLuParLeCapteur / courantReel)
     * @details Le point milieu du signal (offset) est déterminé en comptes ADC bruts par
     *          calibrerZero(), indépendamment de toute tension de référence : seul le rapport
     *          entre la tension d'alimentation réelle et sensibiliteCapteur compte, d'où
     *          l'absence d'un paramètre de tension d'alimentation séparé.
     */
    sensorCurrent(uint8_t pinCapteur, double sensibiliteCapteur, double frequence = FREQUENCE_RESEAU_DEFAUT);

    /**
     * @brief Calibre le point milieu (zéro) du capteur.
     * @attention À appeler lorsqu'aucun courant ne circule (idéalement dans le setup).
     * @details La durée d'échantillonnage s'adapte à la fréquence réseau (NB_PERIODES_CALIBRATION périodes).
     * @note Ne pas rappeler automatiquement pendant le fonctionnement (par ex. dès que le courant
     *       mesuré devient faible) : une partie du signal utile pourrait alors être absorbée dans
     *       l'offset. Une calibration unique au démarrage, charge coupée, suffit.
     */
    void calibrerZero();

    /**
     * @brief Mesure simultanément le courant crête et efficace, en un seul passage d'échantillonnage.
     * @return crête et efficace (A), calculés sur les mêmes échantillons.
     * @details À privilégier par rapport à lireCourantCrete() + lireCourantEfficace() appelées
     *          séparément : les deux valeurs sont dérivées du même passage ADC, donc deux fois
     *          moins de temps d'échantillonnage pour obtenir les deux.
     */
    MesureCourant lireCourant();

    /**
     * @brief Mesure la valeur crête du courant.
     * @return la valeur crête du courant (A).
     * @note Effectue son propre passage d'échantillonnage. Si l'efficace est aussi nécessaire,
     *       préférer lireCourant() qui mesure les deux en une seule fois.
     */
    double lireCourantCrete();

    /**
     * @brief Mesure la valeur efficace (RMS) du courant.
     * @return la valeur efficace du courant (A).
     * @note Effectue son propre passage d'échantillonnage. Si la crête est aussi nécessaire,
     *       préférer lireCourant() qui mesure les deux en une seule fois.
     */
    double lireCourantEfficace();

    /**
     * @brief Mesure la puissance apparente.
     * @param tension Tension du réseau utilisée pour le calcul, par défaut 220V.
     * @return la puissance apparente (VA).
     * @note Effectue son propre passage d'échantillonnage (via lireCourantEfficace()). Si le
     *       courant efficace est déjà disponible (ex. via lireCourant()), il est plus efficace
     *       de calculer directement : puissance = tension x mesure.efficace.
     * @note Un capteur de courant seul ne mesure pas le déphasage tension/courant : la puissance
     *       active et la puissance réactive ne peuvent donc pas être calculées de façon fiable
     *       sans capteur de tension synchronisé, et ne sont volontairement pas proposées ici.
     */
    double lirePuissanceApparente(double tension = TENSION_RESEAU_DEFAUT);

    /**
     * @brief Estime la puissance active, à partir d'un facteur de puissance supposé.
     * @param tension Tension du réseau utilisée pour le calcul, par défaut 220V.
     * @param facteurPuissance cos(phi) supposé de l'installation, par défaut 0.93 (valeur usuelle
     *        pour une habitation française à chauffage résistif dominant).
     * @return la puissance active estimée (W).
     * @warning C'est une ESTIMATION, pas une mesure : un capteur de courant seul ne peut pas
     *          déterminer le déphasage réel tension/courant. Le résultat n'est valable que dans
     *          la mesure où facteurPuissance reflète la charge réelle — à ajuster si connu, ou à
     *          ignorer si l'installation est très différente d'un profil résidentiel classique.
     *          Pour une vraie mesure, il faut un capteur de tension échantillonné en
     *          synchronisation avec le courant (hors de portée de cette bibliothèque).
     * @note Effectue son propre passage d'échantillonnage (via lireCourantEfficace()). Si le
     *       courant efficace est déjà disponible (ex. via lireCourant()), il est plus efficace
     *       de calculer directement : puissance = tension x mesure.efficace x facteurPuissance.
     */
    double lirePuissanceActive(double tension = TENSION_RESEAU_DEFAUT, double facteurPuissance = FACTEUR_PUISSANCE_DEFAUT);

    /**
     * @brief Estime la puissance réactive, à partir du même facteur de puissance supposé.
     * @param tension Tension du réseau utilisée pour le calcul, par défaut 220V.
     * @param facteurPuissance cos(phi) supposé de l'installation, par défaut 0.93.
     * @return la puissance réactive estimée (VAR).
     * @warning Même limite que lirePuissanceActive() : une ESTIMATION basée sur facteurPuissance,
     *          pas une mesure du déphasage réel.
     */
    double lirePuissanceReactive(double tension = TENSION_RESEAU_DEFAUT, double facteurPuissance = FACTEUR_PUISSANCE_DEFAUT);


  private:
    uint8_t _pinCapteur;                     ///< Pin du signal du capteur de courant.
    double _sensibiliteCapteur;              ///< Sensibilité du capteur de courant en mV/A.
    int _tensionMoyenneADC;                  ///< Point milieu du signal (par défaut 1/2 de la pleine échelle ADC).
    unsigned long _dureeMesureMicros;        ///< Durée de mesure en µs pour NB_PERIODES_MESURE périodes réseau.
    unsigned long _dureeCalibrationMicros;   ///< Durée de mesure en µs pour NB_PERIODES_CALIBRATION périodes réseau.

    int _bufferADC[N_MOYENNE] = {0};    ///< Buffer pour moyenne glissante.
    uint8_t _indexBuffer = 0;           ///< Index du buffer.
    bool _bufferRempli = false;         ///< Indique si le buffer est rempli (Si vrai, buffer rempli).

    int filtrerMoyenneGlissante(int nouvelleValeur);   ///< Retourne la valeur ADC filtrée (moyenne glissante).
};
#endif
