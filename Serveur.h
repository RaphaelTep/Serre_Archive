#ifndef SERVEUR_H
#define SERVEUR_H

#include <ESPAsyncWebServer.h>

class Serveur {
private:
    AsyncWebServer server;
    const char* indexHtml;

public:
    Serveur(int port, const char* indexHtml);

    void init(); // Initialiser le serveur
    void definirRoutes(float& temperature, float& hygrometrie, int& hum_plante_1, int& hum_plante_2);
};

#endif
