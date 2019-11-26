// GwOutput_mqtt.h

#ifndef _GWOUTPUT_MQTT_h
#define _GWOUTPUT_MQTT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "GwOutput_generic.h"
#include "dstrootca.h"
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <EnigmaIOTGateway.h>
#include <lib/helperFunctions.h>
#include <lib/debug.h>

#ifdef ESP32
#include <WiFi.h> // Comment to compile for ESP8266
#include <AsyncTCP.h> // Comment to compile for ESP8266
#include <SPIFFS.h>
#include "esp_system.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
//#include <ESPAsyncTCP.h> // Comment to compile for ESP32
#include <Hash.h>
#include <SPI.h>
#include <PubSubClient.h>
#ifdef SECURE_MQTT
#include <WiFiClientSecure.h>
#else
#include <WiFiClient.h>
#endif // SECURE_MQTT
#endif // ESP32

#include <FS.h>

// DOWNLINK MESSAGES
#define GET_VERSION      "get/version"
#define GET_VERSION_ANS  "result/version"
#define GET_SLEEP        "get/sleeptime"
#define GET_SLEEP_ANS    "result/sleeptime"
#define SET_SLEEP        "set/sleeptime"
#define SET_OTA          "set/ota"
#define SET_OTA_ANS      "result/ota"
#define SET_IDENTIFY     "set/identify"
#define SET_RESET_CONFIG "set/reset"
#define SET_RESET_ANS    "result/reset"
#define GET_RSSI         "get/rssi"
#define GET_RSSI_ANS     "result/rssi"

constexpr auto CONFIG_FILE = "/mqtt.conf";

typedef struct {
	char mqtt_server[41];
#ifdef SECURE_MQTT
	int mqtt_port = 8883;
#else
	int mqtt_port = 1883;
#endif //SECURE_MQTT
	char mqtt_user[21];
	char mqtt_pass[41];
} mqttgw_config_t;


class GwOutput_MQTT: GatewayOutput_generic {
 protected:
	 AsyncWiFiManagerParameter* mqttServerParam;
	 AsyncWiFiManagerParameter* mqttPortParam;
	 AsyncWiFiManagerParameter* mqttUserParam;
	 AsyncWiFiManagerParameter* mqttPassParam;

#ifdef ESP32
	 esp_mqtt_client_config_t mqtt_cfg;
	 esp_mqtt_client_handle_t client;
#endif // ESP32

	 mqttgw_config_t mqttgw_config;
	 bool shouldSaveConfig = false;

#ifdef ESP8266
#ifdef SECURE_MQTT
	 BearSSL::X509List certificate (DSTroot_CA);
	 WiFiClientSecure secureClient;
	 PubSubClient client (secureClient);
#else
	 WiFiClient unsecureClient;
	 PubSubClient client (unsecureClient);
#endif // SECURE_MQTT
#endif // ESP8266

	 bool saveMQTTConfig ();
#ifdef ESP32
	 static esp_err_t mqtt_event_handler (esp_mqtt_event_handle_t event);
#endif // ESP32
	 static void onDlData (GwOutput_MQTT* gw, const char* topic, char* payload, unsigned int length);
#ifdef SECURE_MQTT
	 void setClock ();
#endif // SECURE_MQTT
#ifdef ESP8266
	 void reconnect ()
#endif
	 static bool publishMQTT (GwOutput_MQTT* gw, char* topic, char* payload, size_t len, bool retain = false);

 public:
	 void configManagerStart (EnigmaIOTGatewayClass* enigmaIotGw);
	 void configManagerExit (boolean status);
	 bool begin ();
	 bool loadConfig ();
	 bool outputSend (char* address, uint8_t *data, uint8_t length);
};

extern GwOutput_MQTT GwOutput_mqtt;

#endif

