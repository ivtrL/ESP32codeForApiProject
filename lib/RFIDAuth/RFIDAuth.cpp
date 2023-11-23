#include "RFIDAuth.h"
#include <ArduinoJson.h>

AuthClient::AuthClient(HTTPClient *http, MFRC522 *mfrc522) {
  this->http = http;
  this->mfrc522 = mfrc522;
}

AuthResponse AuthClient::loginJwtToken(AuthLoginResquest authLoginRequest,
                                       char *httpLoginServer) {
  AuthResponse authResponse;
  authResponse.error = false;
  char jsonOutput[768];

  Serial.println("Logging in");
  http->useHTTP10(true);
  http->begin(httpLoginServer);
  http->addHeader("Content-Type", "application/json");

  const int capacity = JSON_OBJECT_SIZE(48);
  StaticJsonDocument<capacity> doc;
  doc["email"] = authLoginRequest.email;
  doc["password"] = authLoginRequest.password;
  doc["deviceName"] = authLoginRequest.deviceName;
  doc["deviceUid"] = authLoginRequest.deviceUid;

  serializeJson(doc, jsonOutput);
  int httpCode = http->POST(String(jsonOutput));
  auto error = deserializeJson(doc, http->getStream());
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    http->end();
    authResponse.error = true;
    authResponse.errorMessage = error.c_str();
    return authResponse;
  }
  if (httpCode == 200) {
    authResponse.accessToken = doc["accessToken"].as<String>();
    authResponse.refreshToken = doc["refreshToken"].as<String>();
    http->end();
    return authResponse;
  }
  authResponse.error = true;
  authResponse.errorMessage = doc["message"].as<String>();
  Serial.println("Error: " + httpCode);
  Serial.println(authResponse.errorMessage);
  http->end();
  return authResponse;
}

AuthResponse AuthClient::refreshJwtToken(String refreshToken,
                                         char *httpRefreshTokenServer) {
  char jsonOutput[768];
  AuthResponse authResponse;
  authResponse.error = false;

  Serial.println("Refreshing access token");
  http->useHTTP10(true);
  http->begin(httpRefreshTokenServer);
  http->addHeader("Content-Type", "application/json");

  const int capacity = JSON_OBJECT_SIZE(48);
  StaticJsonDocument<capacity> doc;
  doc["refreshToken"] = refreshToken;

  serializeJson(doc, jsonOutput);
  int httpCode = http->POST(String(jsonOutput));
  auto error = deserializeJson(doc, http->getStream());
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    http->end();
    authResponse.error = true;
    authResponse.errorMessage = error.c_str();
    return authResponse;
  }
  if (httpCode == 200) {
    authResponse.accessToken = doc["accessToken"].as<String>();
    Serial.println("Access token: " + authResponse.accessToken);
    http->end();
    return authResponse;
  }
  authResponse.error = true;
  authResponse.errorMessage = doc["message"].as<String>();
  Serial.println(doc["message"].as<String>());
  http->end();
  return authResponse;
}

CheckCardResponse AuthClient::checkCard(String accessToken, String cardUid,
                                        String deviceUid,
                                        char *httpCheckCardServer) {
  char jsonOutput[768];
  CheckCardResponse cardUidResponse;
  cardUidResponse.error = false;

  Serial.println("Checking card");
  http->useHTTP10(true);
  http->begin(httpCheckCardServer);
  http->addHeader("Content-Type", "application/json");
  http->addHeader("Authorization", "Bearer " + accessToken);

  const int capacity = JSON_OBJECT_SIZE(10);
  StaticJsonDocument<capacity> doc;
  doc["cardUid"] = cardUid;
  doc["deviceUid"] = deviceUid;

  serializeJson(doc, jsonOutput);
  int httpCode = http->POST(String(jsonOutput));
  auto error = deserializeJson(doc, http->getStream());
  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
    http->end();
    cardUidResponse.error = true;
    cardUidResponse.message = error.c_str();
    return cardUidResponse;
  }
  cardUidResponse.message = doc["message"].as<String>();
  Serial.println(doc["message"].as<String>());
  http->end();
  return cardUidResponse;
}

String AuthClient::getCardId() {
  String cardId = "";
  if (!mfrc522->PICC_IsNewCardPresent()) {
    return cardId;
  }
  if (!mfrc522->PICC_ReadCardSerial()) {
    return cardId;
  }
  for (byte i = 0; i < mfrc522->uid.size; i++) {
    cardId.concat(String(mfrc522->uid.uidByte[i] < 0x10 ? " 0" : " "));
    cardId.concat(String(mfrc522->uid.uidByte[i], HEX));
  };

  cardId.toUpperCase();
  return cardId;
}