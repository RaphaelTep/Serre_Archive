#ifndef CAPTEURS_H
#define CAPTEURS_H

#include <Arduino.h>

class Capteurs {
public:
    Capteurs(int pin);
    void init();
    int lireHumiditeSol();  // Renvoie la valeur analogique d'humidité du sol

private:
    int pin;
};

#endif