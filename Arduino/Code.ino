#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>

// Anzahl der LEDs und Pin definieren
#define LED_PIN 2       // GPIO-Pin für Datenleitung
#define NUM_LEDS 300      // Anzahl der LEDs im Streifen

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


// WLAN-Zugangsdaten
const char* ssid = "xxx";
const char* password = "xxx";

// MQTT HiveMQ Broker (Public Broker von HiveMQ oder eigener Server)
const char* mqtt_server = "xxx";  // Ändere auf deinen eigenen HiveMQ-Broker, falls benötigt
const int mqtt_port = 8883;  // TLS-Port für MQTT

// MQTT-Zugangsdaten (falls Authentifizierung nötig)
const char* mqtt_user = "xxx";  // Falls notwendig, sonst leer lassen
const char* mqtt_password = "xxx";  // Falls notwendig, sonst leer lassen

// MQTT-Themen
const char* mqtt_topic = "esp32/test";  

// TLS-Zertifikat (Root CA für HiveMQ oder eigenen Server)
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----

)EOF";

// WiFi- und MQTT-Clients erstellen
WiFiClientSecure espClient;
PubSubClient client(espClient);


// Touch-Pin und LED-Status
#define TOUCH_PIN T0  // Touch-Sensor-Pin
bool ledState = false;

void setup() {
    Serial.begin(115200);

    strip.begin();          // Initialisierung der LEDs
    strip.show();           // LEDs ausschalten (alle auf Schwarz setzen)
    
    
    // WLAN-Verbindung herstellen
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi verbunden");
    
     // Root-CA-Zertifikat setzen
    espClient.setCACert(root_ca);
    
    // MQTT-Client einrichten
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    reconnect();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    
    // Touch-Eingang auslesen
    if (touchRead(TOUCH_PIN) < 30) {  // Schwellenwert anpassen
        delay(200); // Entprellen
        ledState = !ledState;
        sendMqttStatus();
        controlWLED();
        delay(500); // Wartezeit, um versehentliches mehrfaches Umschalten zu verhindern
    }
}

// Callback-Funktion für empfangene Nachrichten
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Nachricht empfangen von: ");
    Serial.println(topic);
    
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println("Inhalt: " + message);
    if (message == "ON") {
      ledState = true;
      controlWLED();
    } 
    else if (message == "OFF") {
      ledState = false;
      controlWLED();
    }
}

void sendMqttStatus() {
    if (ledState) {
        client.publish(mqtt_topic, "ON");
    } else {
        client.publish(mqtt_topic, "OFF");
    }
}

void controlWLED() {
    if (ledState) {
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, strip.Color(255, 0, 0)); // Rot setzen
            }
            strip.show();           // Farben anzeigen
    } else {
        for (int i = 0; i < NUM_LEDS; i++) {
            strip.setPixelColor(i, strip.Color(0, 0, 0)); // Schwarz setzen
          }
          strip.show();
    }
}

// Verbindung zum MQTT-Server wiederherstellen
void reconnect() {
    while (!client.connected()) {
        Serial.print("Verbindung zu MQTT... ");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
            Serial.println("Verbunden!");
            client.subscribe(mqtt_topic);
        } else {
            Serial.print("Fehler, rc=");
            Serial.print(client.state());
            Serial.println(" Neuer Versuch in 5 Sekunden...");
            delay(5000);
        }
    }
}

