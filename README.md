# SensorCurrent

![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform: Arduino](https://img.shields.io/badge/platform-arduino-orange.svg)
![Version](https://img.shields.io/badge/version-2.0.0-blue)

> 📟 **SensorCurrent** est une bibliothèque Arduino permettant de mesurer le courant alternatif (AC) avec des capteurs analogiques tels que **ACS712** ou **SCT-013**.  
> Elle permet de mesurer l’intensité **crête**, l’intensité **efficace (RMS)** et la **puissance apparente**, avec **filtrage logiciel** et **correction de mesure**.

---

## 🔧 Capteurs compatibles

- **ACS712** (5A / 20A / 30A)
- **SCT-013** (000 ou versions à sortie analogique)
- Tous les capteurs analogiques centrés sur Vcc/2

---

## 📦 Installation

### 🚀 PlatformIO

Ajoutez à votre fichier `platformio.ini` :

```ini
lib_deps =
  boby15000/SensorCurrent@^2.0.0


---

### 📦 Installation

1. Télécharge ou clone ce dépôt :
   ```bash
   git clone https://github.com/boby15000/SensorCurrent.git
   ```

2. Copie le dossier `SensorCurrent` dans ton répertoire `libraries/` d’Arduino :
   - Sous Windows : `Documents/Arduino/libraries`
   - Sous Linux/macOS : `~/Arduino/libraries/`

3. Redémarre l’IDE Arduino.

> La librairie existe normalement dans le gestionnaire Arduino et Platformio.

---

## 🛠️ Utilisation de base

```cpp
#include <sensorCurrent.h>

// DEFINITION DES PINS
uint8_t PIN_CAPTEUR_COURANT_ACS = A0;
uint8_t PIN_CAPTEUR_COURANT_SCT = A1;

// DEFINITION SENSIBILITE DES CAPTEURS
uint8_t SENSIBILITE_CAPT = 100;


sensorCurrent ACS712(PIN_CAPTEUR_COURANT_ACS, SENSIBILITE_CAPT);
//sensorCurrent ACS712(PIN_CAPTEUR_COURANT_ACS, SENSIBILITE_CAPT,4.60, 50);
sensorCurrent SCT013(PIN_CAPTEUR_COURANT_SCT, SENSIBILITE_CAPT); 

void setup() {
  // Pour le Debug
  Serial.begin(9600);
  
  // Calibration du Capteur 1
  ACS712.CalibrationZero(); // A réaliser hors circulation de courant
  ACS712.Set_FacteurDeCorrection(1.23); // valeur à corriger via : facteur = Valeur "Métrix" / Valeur Mesuré
 
  // Calibration du Capteur 2
  SCT013.CalibrationZero(); // A réaliser hors circulation de courant
  SCT013.Set_FacteurDeCorrection(2.15); // valeur à corriger via : facteur = Valeur "Métrix" / Valeur Mesuré
}

void loop() {
   

    Serial.print("ACS712 Courant : ");
    Serial.print(ACS712.GetCourantEff());
    Serial.println(" A");
    Serial.print("ACS712 Puissance : ");
    Serial.print(ACS712.GetPuissanceApparente(230));
    Serial.println(" W");

    Serial.println(" --- ");

    Serial.print("SCT013 Courant : ");
    Serial.print(SCT013.GetCourantEff());
    Serial.println(" A");
    Serial.print("SCT013 Puissance : ");
    Serial.print(SCT013.GetPuissanceApparente(230));
    Serial.println(" W");

    Serial.println("-----------------");
    Serial.println("-----------------");

    delay(2000);
}
```

---

## ⌚ Méthodes principales

| Méthode                       | Description |
|-------------------------------|-------------|
| `CalibrationZero()`                         | Calibrage hors circulation de courant. |
| `Set_FacteurDeCorrection(facteur)`          | Permet d'affiner la valeur du courant. |
| `GetCourantCrete(FacteurDeCorrection)`      | Indique le courant crête. |
| `GetCourantEff(FacteurDeCorrection)`        | Indique le courant efficace. |
| `GetPuissanceApparente()`                   | Indique le courant puissance. |

---

## 📌 Set_FacteurDeCorrection

Permet de corriger l'intensité mesurée.
```cpp
void Set_FacteurDeCorrection(facteur);
```
Un facteur de 1.0 signifie qu’aucune correction n’est appliquée.
La valeur du facteur doit être comprise entre 0.1 et 3.0.
Le facteur doit être déterminé en comparant la mesure réelle (référence) à celle fournie par le capteur :
![Formule](https://latex.codecogs.com/png.image?\large&space;\dpi{110}\textbf{facteur}=\frac{Valeur\:reference}{Valeur\:mesuree})

---

## 🧪 Exemples fournis

- `mesureCurrent.cpp` – Exemple de base

---

## 🧰 Compatibilité

- ATmega328 (Uno, Nano)
- ATtiny85 
- ESP8266, ESP32
- Arduino Mega

---

## 📄 Licence

MIT – Utilisation libre avec attribution.

---

## 🤝 Contribuer

Les contributions sont les bienvenues ! Forkez le dépôt, créez une branche, puis ouvrez une *pull request*.

---

> Mainteneur : Nicolas Fourgheon
>  
> Pour toute question, ouvrez un [Issue](https://github.com/boby15000/SensorCurrent/issues).