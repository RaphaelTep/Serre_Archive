#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <DHT.h>
#include <utility>

// Structure de retour pour encapsuler les données + validité
struct DHTResult {
    float temperature;
    float humidity;
    bool valid;
};

class DHTSensor {
public:
    DHTSensor(int pin, int type);
    void init(int pin);

 
    DHTResult readTemperatureAndHumidity();

private:
    DHT dht;
};

#endif