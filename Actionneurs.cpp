#include "Actionneurs.h"

Actionneurs::Actionneurs(int pin)
    : pin(pin) {}

void Actionneurs::init() 
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW); // Par défaut éteint
}

void Actionneurs::allumer() 
{
    digitalWrite(pin, HIGH);
}

void Actionneurs::eteindre() 
{
    digitalWrite(pin, LOW);
}