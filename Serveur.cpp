#include "Serveur.h"

Serveur::Serveur(int port, const char* indexHtml) : server(port), indexHtml(indexHtml) {}

void Serveur::init() {
    server.begin();
    Serial.println("Serveur HTTP démarré !");
}

void Serveur::definirRoutes(float& temperature, float& hygrometrie, int& hum_plante_1, int& hum_plante_2) {
    server.on("/", HTTP_GET, [&temperature, &hygrometrie, &hum_plante_1, &hum_plante_2, this](AsyncWebServerRequest* request) {
        String page = indexHtml;
        page.replace("%TEMPERATURE%", String(temperature));
        page.replace("%HYGROMETRIE%", String(hygrometrie));
        page.replace("%HUM_PLANTE_1%", String(hum_plante_1));
        page.replace("%HUM_PLANTE_2%", String(hum_plante_2));
        request->send(200, "text/html", page);
    });

    server.on("/temperature", HTTP_GET, [&temperature](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(temperature));
    });

    server.on("/hygrometrie", HTTP_GET, [&hygrometrie](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(hygrometrie));
    });

    server.on("/hum_plante_1", HTTP_GET, [&hum_plante_1](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(hum_plante_1));
    });

    server.on("/hum_plante_2", HTTP_GET, [&hum_plante_2](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", String(hum_plante_2));
    });

    server.on("/mesures", HTTP_GET, [&temperature, &hygrometrie, &hum_plante_1, &hum_plante_2](AsyncWebServerRequest* request) {
        String json = "{";
        json += "\"temperature\":" + String(temperature) + ",";
        json += "\"hygrometrie\":" + String(hygrometrie) + ",";
        json += "\"hum_plante_1\":" + String(hum_plante_1) + ",";
        json += "\"hum_plante_2\":" + String(hum_plante_2) + ",";
        json += "}";
        request->send(200, "application/json", json);
    });
}
