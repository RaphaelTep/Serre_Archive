#ifndef ACTIONNEURS_H
#define ACTIONNEURS_H

#include <Arduino.h>

class Actionneurs {
private:
    int pin;

public:
    Actionneurs(int pin);
    void init();             // Initialiser l'actionneur
    void allumer();          // Allumer l'actionneur
    void eteindre();         // Éteindre l'actionneur
};

#endifini