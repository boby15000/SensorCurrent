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
    static const byte MILLIVOLT_PAR_AMPERE = 1 ;
    static const byte AMPERE_PAR_VOLT = 0 ;
    const int TENSION_MAX_ADC = 1024; 

    sensorCurrent(byte pin_sensor, double sensibilite_Capt, byte type_sensibilite, double frequence = 50, double tension = 230.0);

    int GetADC(bool filtrage = false);
    void Etalonnage();
    void FacteurDeCorrectionDuZero(byte value);
    void FacteurDeCorrectionACharge(double value);
    double GetCourantCrete();
    double GetCourantEff();
    double GetPuissance();

  private :
    byte _PinSensor;
    double _Sensibilite;
    byte _type_sensibilite;
    double _Tension;
    int _TensionRef;
    int _FacteurDeStabilisation = 5;
    int _FacteurDeCorrectionDuZero = 0;
    double _FacteurDeCorrectionACharge = 1.0;

    int ReadingSensorAC();
};

#endif