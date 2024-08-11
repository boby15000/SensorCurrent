#if ARDUINO >= 100    
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifndef sensorCurrent_INCLUDED
#define sensorCurrent_INCLUDED


class sensorCurrent
{
  public :
    // contructeur
    sensorCurrent(byte pin_sensor, float sensibilite, byte unite_sensibilite, float frequence = 50, float tension = 230.0);

    float FacteurDeCorrection = 1.0;
    int FacteurDeSensibilite = 3;
    int GetADC();
    void Etalonnage();
    float GetCourantCrete();
    float GetCourantEff();
    float GetPuissance();

  private :
    byte _PinSensor;
    float _Sensibilite;
    byte _UniteSensibilite;
    int _Echantillonnage;
    float _Tension;
    int _TensionRef;

    int ReadingSensor();
};

#endif