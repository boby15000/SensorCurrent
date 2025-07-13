# SensorCurrent

📟 Librairie Arduino pour mesurer le courant alternatif (AC) à l’aide de capteurs type SCT-013, ACS712, etc.

## ✅ Fonctionnalités

- Mesure du courant **crête**, **efficace (RMS)** et de la **puissance active**
- Prise en charge des capteurs à sensibilité **en mV/A** ou **A/V**
- Système **non bloquant** (basé sur AntiDelay)
- Compatible 5V et 3.3V
- Étalonnage automatique du zéro à vide

## 🧰 Installation

1. Copiez le dossier dans `Documents/Arduino/libraries/SensorCurrent/`
2. Ou installez via `.zip` dans l'IDE Arduino

## 🔌 Exemple simple

```cpp
#include <SensorCurrent.h>

SensorCurrent capteur(A0, 185, SensorCurrent::MILLIVOLT_PAR_AMPERE);

void setup() {
  Serial.begin(9600);
  capteur.Etalonnage();
}

void loop() {
  Serial.print("Courant RMS (A) : ");
  Serial.println(capteur.GetCourantEff());
  delay(1000);
}
