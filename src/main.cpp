#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MFRC522.h>
#include <RFIDAuth.h>
#include <SPI.h>
#include <WiFi.h>

constexpr uint8_t RST_PIN = 2;
constexpr uint8_t SS_PIN = 5;

constexpr uint8_t RED_LED_PIN = 21;
constexpr uint8_t GREEN_LED_PIN = 4;

HTTPClient client;
MFRC522 mfrc522(SS_PIN, RST_PIN);

AuthClient authClient(&client, &mfrc522);

const char *ssid = "VIVOFIBRA-4BA8";
const char *passwordWifi = "5E259B8056";

// Login Server
char httpLoginServer[] = "https://apiesp32.onrender.com/api/device/login";

AuthLoginResquest authLoginRequest;

// Refresh Token Server
char httpRefreshTokenServer[] =
    "https://apiesp32.onrender.com/api/auth/refresh-token/device";

// Check Card Server
char httpCheckCardServer[] = "https://apiesp32.onrender.com/api/card/check";

typedef struct {
  String RefreshToken;
  String AccessToken;
} AuthJwtTokens;

AuthJwtTokens authJwtTokens;

void checkCardCallback(CheckCardResponse checkCardResponse);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, passwordWifi);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    Serial.print(".");
    delay(200);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.print(".");
    delay(200);
  }
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  Serial.println("Connected to WiFi");

  // Login Parameters
  authLoginRequest.email = "teste@gmail.com";
  authLoginRequest.password = "teste123";
  authLoginRequest.deviceName = "TESTEAPI";
  authLoginRequest.deviceUid = "40e22efe2c491935e4a55c72fc153dad";

  String teste = "teste";
  if (teste == "teste")
    Serial.println("Teste");

  AuthResponse AuthResponse =
      authClient.loginJwtToken(authLoginRequest, httpLoginServer);
  if (AuthResponse.error) {
    Serial.println("Error: " + AuthResponse.errorMessage);
    return;
  }
  authJwtTokens.RefreshToken = AuthResponse.refreshToken;
  authJwtTokens.AccessToken = AuthResponse.accessToken;

  Serial.println("Refresh token: " + authJwtTokens.RefreshToken);
  Serial.println("Access token: " + authJwtTokens.AccessToken);
}

void loop() {
  String cardUid = authClient.getCardId();
  if (cardUid == "")
    return;
  Serial.println("Card UID: " + cardUid);
  CheckCardResponse Response =
      authClient.checkCard(authJwtTokens.AccessToken, cardUid,
                           authLoginRequest.deviceUid, httpCheckCardServer);

  checkCardCallback(Response);
}

void checkCardCallback(CheckCardResponse checkCardResponse) {
  // Put here the code that you want to execute when the callback is called

  if (checkCardResponse.error) {
    Serial.println("Error: " + checkCardResponse.message);
    return;
  }
  Serial.println("Message: " + checkCardResponse.message);
  if (checkCardResponse.message == "Blocked") {
    Serial.println("Blocked card");
    for (int i = 0; i <= 5; i++) {
      digitalWrite(RED_LED_PIN, HIGH);
      delay(150);
      digitalWrite(RED_LED_PIN, LOW);
      delay(150);
    }
    return;
  }
  Serial.println("Authorized card");
  for (int i = 0; i <= 5; i++) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    delay(150);
    digitalWrite(GREEN_LED_PIN, LOW);
    delay(150);
  }
  return;
};
