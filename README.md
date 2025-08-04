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

![Formule](https://latex.codecogs.com/png.image?\large&space;\dpi{110}{\color{Green}\textbf{facteur}=\frac{Valeur\:reference}{Valeur\:mesuree})

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
