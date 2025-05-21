#include "Capteurs.h"

Capteurs::Capteurs(int pin) : pin(pin) {}

void Capteurs::init() 
{
    pinMode(pin, INPUT);
}

int Capteurs::lireHumiditeSol() {
    return analogRead(pin);
}