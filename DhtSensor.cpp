#include "DhtSensor.h"

DHTSensor::DHTSensor(int pin, int type) : dht(pin, type) {}

void DHTSensor::init(int pin) {
    dht.begin(pin);
}

DHTResult DHTSensor::readTemperatureAndHumidity() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    bool valid = !(isnan(temperature) || isnan(humidity)) &&
                 temperature >= 0 && temperature <= 60 &&
                 humidity >= 0 && humidity <= 100;
    return {temperature, humidity, valid};
}


