# SensorCurrent

![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform: Arduino](https://img.shields.io/badge/platform-arduino-orange.svg)
![Version](https://img.shields.io/badge/version-4.1.0-blue)

> 📟 **SensorCurrent** est une bibliothèque Arduino permettant de mesurer le courant alternatif (AC) avec des capteurs analogiques tels que **ACS712** ou **SCT-013**.  
> Elle permet de mesurer l’intensité **crête**, l’intensité **efficace (RMS)**, la **puissance apparente** (mesurée) et une **estimation** de puissance active/réactive, avec **filtrage logiciel** et **calibration du zéro**.

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
  boby15000/SensorCurrent@^4.1.0


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
```

---

## ⌚ Méthodes principales

| Méthode                            | Échantillonnage | Description |
|-------------------------------------|:---:|-------------|
| `calibrerZero()`                    | ✅ (adapté à la fréquence, ~20 périodes) | Calibrage du point milieu, hors circulation de courant. |
| `lireCourant()`                     | ✅ (adapté à la fréquence, 10 périodes) | Mesure crête **et** efficace en un seul passage (`MesureCourant{crete, efficace}`). À privilégier quand les deux valeurs sont utiles. |
| `lireCourantCrete()`                | ✅ (adapté à la fréquence, 10 périodes) | Indique le courant crête (A) seul. |
| `lireCourantEfficace()`             | ✅ (adapté à la fréquence, 10 périodes) | Indique le courant efficace / RMS (A) seul. |
| `lirePuissanceApparente(tension)`   | ✅ (via `lireCourantEfficace`) | Puissance apparente **mesurée** (VA) = V × I efficace, tension par défaut 220V. |
| `lirePuissanceActive(tension, cosPhi)` | ✅ (via `lireCourantEfficace`) | Puissance active **estimée** (W) = V × I efficace × cosPhi, `cosPhi` par défaut 0.93. |
| `lirePuissanceReactive(tension, cosPhi)` | ✅ (via `lireCourantEfficace`) | Puissance réactive **estimée** (VAR), déduite du même `cosPhi`. |

> ⚡ **Efficacité** : `lireCourantCrete()`, `lireCourantEfficace()`, `lirePuissanceApparente()`,
> `lirePuissanceActive()` et `lirePuissanceReactive()` effectuent chacune leur propre passage
> d'échantillonnage (~200ms à 50Hz). Si tu as besoin de plusieurs de ces valeurs, appelle
> `lireCourant()` une seule fois et calcule le reste à partir du résultat
> (`tension * mesure.efficace [* cosPhi]`) plutôt que d'enchaîner les appels.

> ⚠️ **Puissance active / réactive : ESTIMATION, pas une mesure.** Un capteur de courant seul ne
> mesure pas le déphasage réel tension/courant. `lirePuissanceActive()`/`lirePuissanceReactive()`
> supposent un `cosPhi` (par défaut 0.93, valeur usuelle pour une habitation française à chauffage
> résistif dominant) — à ajuster si le cos(phi) réel de l'installation est connu, sous peine
> d'un résultat trompeur pour une charge très différente (moteurs sans compensation, électronique
> à découpage...). Pour une vraie mesure, il faut un capteur de tension échantillonné en
> synchronisation avec le courant (hors de portée de cette bibliothèque). `lirePuissanceApparente()`
> (S = V × I), elle, reste une valeur réellement mesurée.

---

## 📌 Calibration de la sensibilité

La bibliothèque n'expose ni facteur de correction réglable au runtime, ni tension d'alimentation
en paramètre séparée : la sensibilité (mV/A) se règle directement via le paramètre
`sensibiliteCapteur` du constructeur, et absorbe aussi bien un écart de mesure du capteur qu'un
Vcc réel différent de 5V (le point milieu est de toute façon déterminé en comptes ADC bruts par
`calibrerZero()`, indépendamment de toute tension de référence — seul le rapport entre Vcc réel
et `sensibiliteCapteur` compte, donc les deux ne sont pas distinguables par calibration externe).

Si un écart systématique est constaté par rapport à une référence (pince ampèremétrique,
multimètre), corrige la constante `SENSIBILITE_CAPT` de ton sketch :

![Formule](https://latex.codecogs.com/png.image?\large&space;\dpi{110}{\color{Green}\textbf{nouvelleSensibilite}=\textbf{sensibiliteCapteur}\times\frac{courantLuParLeCapteur}{courantReel})

---

## 🧪 Exemples fournis

- `examples/mesureCurrent/mesureCurrent.cpp` – Exemple de base avec un SCT-013
- `examples/mesureCourantACS712/mesureCourantACS712.cpp` – Exemple de base avec un ACS712

---

## 🧰 Compatibilité

- ATmega328 (Uno, Nano)
- ATtiny85 
- ESP8266, ESP32 (résolution ADC détectée automatiquement : 10 bits sur AVR/ESP8266, 12 bits sur ESP32)
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
