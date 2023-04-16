#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "config.h"


#define STATUS_LED_PIN 2


void StatusLedOn()
{
    digitalWrite(STATUS_LED_PIN, LOW);
}

void StatusLedOff()
{
    digitalWrite(STATUS_LED_PIN, HIGH);
}

IPAddress IpLocal;
IPAddress IpGateway;
IPAddress IpMask;
IPAddress IpDns1;
IPAddress IpDns2;

void setup() {
    pinMode(STATUS_LED_PIN, OUTPUT);
    StatusLedOn();

    IpLocal.fromString(IP_LOCAL);
    IpGateway.fromString(IP_GATEWAY);
    IpMask.fromString(IP_MASK);
    IpDns1.fromString(DNS_1);
    IpDns2.fromString(DNS_2);

    if (!WiFi.config(IpLocal, IpGateway, IpMask, IpDns1, IpDns2)) {
        return;
    }

    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    const uint32_t delayTimeMs = 5;
    uint32_t timeout = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(delayTimeMs);
        timeout += delayTimeMs;
        if (timeout > WIFI_CONNECT_TIMEOUT_MS) {
            return;
        }
    }

    WiFiClient client;
    HTTPClient http;
    String requestBaseUrl = REQUEST_BASE_URL;
    uint64_t seckey = (uint64_t)ESP.random() * (uint64_t)PRESHARED_KEY;
    String seckeyStr = String(seckey);
    String url = requestBaseUrl + "?seckey=";
    url += seckeyStr;
    http.begin(client, url.c_str());
    int httpResponseCode = http.GET();
    
    if (httpResponseCode < 200 || httpResponseCode >= 300) {
        http.end();
        return;
    }
    http.end();

    StatusLedOff();
    ESP.deepSleep(0);
}

void loop() {
    static uint32_t blinkCount;
    StatusLedOn();
    delay(50);
    StatusLedOff();
    delay(50);
    blinkCount++;
    if (blinkCount > 50) {
        ESP.deepSleep(0);
    }
}
