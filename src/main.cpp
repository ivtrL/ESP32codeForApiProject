#include <SPI.h>
#include <WiFi.h>
#include <RFIDAuth.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 2;
constexpr uint8_t SS_PIN = 5;     

HTTPClient client;
MFRC522 mfrc522(SS_PIN, RST_PIN);

AuthClient authClient(&client, &mfrc522);

const char* ssid = "VIVOFIBRA-4BA8";
const char* passwordWifi = "5E259B8056";

// Login Server

char httpLoginServer[] = "https://apiesp32.onrender.com/api/device/login";

AuthLoginResquest authLoginRequest;

// Refresh Token Server
char httpRefreshTokenServer[] = "https://apiesp32.onrender.com/api/auth/refresh-token/device";

// Check Card Server
char httpCheckCardServer[] = "https://apiesp32.onrender.com/api/card/check";


String RefreshToken;
String AccessToken;

void setup() {
  Serial.begin(9600);

  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, passwordWifi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  authLoginRequest.email = "teste@gmail.com";
  authLoginRequest.password = "teste123";
  authLoginRequest.deviceName = "TESTEAPI";
  authLoginRequest.deviceUid = "40e22efe2c491935e4a55c72fc153dad";
  
  authClient.loginJwtToken(authLoginRequest, httpLoginServer);
}

void loop() {
}
