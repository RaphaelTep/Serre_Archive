#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <time.h>
#include <DHT.h>
#include <EEPROM.h>
#include "DhtSensor.h"
#include "Serveur.h"
#include "Actionneurs.h"
#include "Capteurs.h"
#include <esp_system.h> 

#define EEPROM_SIZE 12



// Définition du nom et mot de passe du Point d'Accès
const char *ssid = "ESP32_AP";
const char *passphrase = "123456789";

// Définition de l'adresse IP du serveur
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);  
IPAddress subnet(255,255,255,0);

// Page HTML avec mise à jour automatique
// Pour changer la vitesse d'actualisation, il faut changer le nombre après CONTENT= à la ligne 41 (en secondes)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
    <title>ESP32 Web Server</title>
    <META HTTP-EQUIV="Refresh" CONTENT="30;">  
    <script>
        function updateData() {
            fetch("/temperature")
                .then(response => response.json()) // Utilisation de .json() pour parser la réponse
                .then(data => {
                    document.getElementById("tempValue").innerText = data.temperature; // Assurez-vous que la clé correspond à la réponse JSON
                });
            fetch("/hum_plante_1")
                .then(response => response.json())    
                .then(data => {
                    document.getElementById("HumValue").innerText = data.hum_plante_1;
                });
            fetch("/hum_plante_2")
                .then(response => response.json())
                .then(data => {
                    document.getElementById("HumValue2").innerText = data.hum_plante_2;
                });
            fetch("/hygrometrie")
                .then(response => response.json())
                .then(data => {
                    document.getElementById("hygValue").innerText = data.hygrometrie;
                });
        }
        setInterval(updateData, 2000);
    </script>
</head>
<body>
    <h1>Informations de la serre</h1>
    <h1>Temperature         : <span id="tempValue">%TEMPERATURE%</span>°C</h1>
    <h1>Humidite dans l'air : <span id="HygValue">%HYGROMETRIE%</span>%</h1>
    <h1>Humidite au sol plante 1 : <span id="HumValue">%HUM_PLANTE_1%</span></h1>
    <h1>Humidite au sol plante 2 : <span id="HumValue2">%HUM_PLANTE_2%</span></h1>
</body>
</html>
)rawliteral";

// Déclaration du serveur Web
Serveur serveur(80, index_html);

// Variables de capteur
int hum_plante_1; // humidité au sol
int hum_plante_2;
int hum_min = 1000; // Humidité minimale très élevée

#define HUM_PIN_1 33// Le pin de l'ESP32 qui va servir à lire le taux d'humidité au sol (plante 1)
Capteurs terre_1(HUM_PIN_1);
Capteurs terre_2(HUM_PIN_2);

#define HUM_PIN_2 19 // Le pin de l'ESP32 qui va servir à lire le taux d'humidité au sol (plante 2)
#define SEUIL 500000 // Seuil du capteur d'humidité

float temperature; // Variable pour la température
float temp_min = 100.0; // Température minimale très élevée
float temp_max = 25.0;
float hygrometrie; // Variable pour le taux d'hygrométrie (humidité dans l'air)
float hygrometrie_min = 100.0; // Hygrométrie minimale très élevée

#define TEMP_HYG_PIN 4 // Utiliser la constante définie dans Capteurs.h
#define DHTTYPE DHT11 //On utilisera le DHT11

#define POMPE_PIN 12
#define BRUMISATEUR_PIN 13
#define VENTILATEUR_PIN 32
#define TEMP_HYG_PIN 4 // Utiliser la constante définie dans Capteurs.h
DHTSensor dhtSensor(TEMP_HYG_PIN,DHT11);

// Déclaration de l'objet Actionneurs
Actionneurs pompe(POMPE_PIN);
Actionneurs brumisateur(BRUMISATEUR_PIN);
Actionneurs ventilateur(VENTILATEUR_PIN);
// Actionneurs moteur(MOTEUR_PIN);

time_t departTimer = time(NULL); // Correction: Ensure proper declaration and initialization

void setup() {
    Serial.begin(9600);

    EEPROM.begin(EEPROM_SIZE);
    int boardaddress = 0;

    // Vérification de la raison du redémarrage
    if (esp_reset_reason() == ESP_RST_POWERON) { // Utiliser la fonction et la constante correctes
        Serial.println("Redémarrage après mise sous tension.");
        int boardID = 18;
        EEPROM.put(boardaddress, boardID);
        boardaddress += sizeof(boardID); // Mettre à jour la valeur de l'adresse
        float param = 26.5;
        EEPROM.put(boardaddress, param);
        EEPROM.commit();
    } else if (esp_reset_reason() == ESP_RST_SW) { // Utiliser la constante correcte
        Serial.println("Redémarrage après réinitialisation logicielle.");
    } else {
        Serial.println("Redémarrage pour une autre raison.");
    }

    // Configuration du réseau WiFi AP
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ssid, passphrase);
    delay(100);  // Ajout d'un court délai pour s'assurer que le WiFi démarre correctement
    Serial.println("Point d'accès WiFi activé !");
    Serial.print("IP du serveur : ");
    Serial.println(WiFi.softAPIP());

    // Vérification du mode AP
    Serial.println("Vérification du mode AP...");
    if (WiFi.getMode() == WIFI_AP) {
        Serial.println("L'ESP32 est bien en mode Point d'Accès !");
    } else {
        Serial.println("ERREUR : L'ESP32 n'est pas en mode AP !");
    }

    // Initialisation des capteurs
    // Removed erroneous calls to init() as Capteurs class does not have an init method

    // Initialisation des actionneurs
    pompe.init();
    brumisateur.init();
    ventilateur.init();
    dhtSensor.init(TEMP_HYG_PIN);
    pinMode(TEMP_HYG_PIN, INPUT);
    // moteur.init();


    terre_1.init();


    



    // Initialisation du serveur
    serveur.init();
    serveur.definirRoutes(temperature, hygrometrie, hum_plante_1, hum_plante_2);
}

void loop() 
{
    if (time(NULL) - departTimer >= 5) 
    {
        DHTResult result = dhtSensor.readTemperatureAndHumidity();

        if (!result.valid) 
        {

            Serial.println("Lecture invalide !");
            departTimer = time(NULL) + 3; // relance dans 2 secondes
        }
        else
        {
            temperature = result.temperature;
            delay(10);
            hygrometrie = result.humidity;
            delay(10);
            Serial.print("\n Température : ");
            Serial.print(temperature);
            Serial.print(" °C");
            delay(10);
            Serial.print("\n Humidité dans l'air : ");
            Serial.print(hygrometrie);
            Serial.print(" %");
            delay(10);
            departTimer = time(NULL);
        }

        // Si tu veux remettre les capteurs d’humidité du sol plus tard :
        
        hum_plante_1 = terre_1.lireHumiditeSol();   
        // hum_plante_2 = humidite.lireHumiditeSol(HUM_PIN_2);   

        Serial.print("\n Humidité au sol plante 1 : ");
        Serial.print(hum_plante_1);

        // Serial.print("\n Humidité au sol plante 2 : ");
        // Serial.print(hum_plante_2);
        
    }
    
}