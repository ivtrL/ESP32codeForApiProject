#ifndef RFIDAuth_h
#define RFIDAuth_h

#include <HTTPClient.h>
#include <MFRC522.h>
#include <WString.h>

 typedef struct {
    String accessToken;
    String refreshToken;
    String errorMessage;
    int statusCode;
    bool error;
} AuthResponse;

typedef struct {
    const char* email;
    const char* password;
    const char* deviceName;
    const char* deviceUid;
} AuthLoginResquest;

typedef struct {
    String message;
    int statusCode;
    bool error;
} CheckCardResponse;

class AuthClient {
  public: 
    AuthClient(HTTPClient* http, MFRC522* mfrc522);
    HTTPClient* http;
    MFRC522* mfrc522;
    AuthResponse loginJwtToken(AuthLoginResquest authLoginResquest, char* httpLoginServer);
    AuthResponse refreshJwtToken(String refreshToken, char* httpRefreshTokenServer);
    CheckCardResponse checkCard(String accessToken, String cardId, char* httpCheckCardServer);
    String getCardId();
};

#endif