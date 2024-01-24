/***************************************************************************************
  BusyBox Server for LilyGo T-Display

  Created:   7 Dec 2023

  Hardware
    - Microcontroller       LilyGo T-Display

  Description: This is a painlessMesh client (viewer) for the BusyBox network. This is
  written for an ESP32 device with a screen and will display either the free or busy
  message. The device is not able to change on it's own - it only changes when connected
  to a BusyBox server device (compatible with any in the server folder). If you are
  looking for a standalone BusyBox that is only changed locally using the onboard
  buttons, look in the standalone folder of this project.

 **************************************************************************************/

#include <TFT_eSPI.h>
#include "BusyBoxClient.h"
#include "painlessMesh.h"

#if defined __has_include
#if __has_include("creds.h")
#include "creds.h"
#else

// BusyBox Mesh Network Settings - Change to create private network
#define MESH_SSID "BusyBox"
#define MESH_PASSWORD "P@ssw0rd"
#define MESH_PORT 5555
// End BusyBox Mesh Network Settings

#endif
#endif

painlessMesh mesh;

TFT_eSPI tft = TFT_eSPI(135, 240);

enum status { FreeStatus, BusyStatus, KnockStatus, WaitingStatus };
int currentState = status::WaitingStatus;


// Busy Handler
void busyHandler() {
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.drawString("BUSY", tft.width() / 2, tft.height() / 2);
    currentState = status::BusyStatus;
}


// Free Handler
void freeHandler() {
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.drawString("FREE", tft.width() / 2, tft.height() / 2);
    currentState = status::FreeStatus;
}


// Knock Handler
void knockHandler() {
    tft.fillScreen(TFT_YELLOW);
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.drawString("KNOCK", tft.width() / 2, tft.height() / 2);
    currentState = status::KnockStatus;
}


void sendStatusConfirmation(uint32_t from) {
    String msg = String(currentState);
    mesh.sendSingle(from, msg);
}


void receivedCallback(uint32_t from, String &msg) {
    if (msg.toInt() != currentState) {
        if (msg.toInt() == 0) {
            freeHandler();
        } else if (msg.toInt() == 1) {
            busyHandler();
        } else if (msg.toInt() == 2) {
            knockHandler();
        }
    }

    sendStatusConfirmation(from);
}


void setup() {
    // Screen Init
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(1);
    tft.setSwapBytes(true);

    // Splash Screen
    tft.pushImage(18, 2, 204, 132, BusyBoxClient);
    delay(4000);

    // Uncomment for connection debugging
    // Serial.begin(115200);
    // mesh.setDebugMsgTypes(ERROR | CONNECTION | STARTUP);

    // Mesh Init
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);

    mesh.setContainsRoot(true);

    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Waiting", tft.width() / 2, tft.height() / 2 - 12);
    tft.drawString("for Init.", tft.width() / 2, tft.height() / 2 + 12);
}


void loop() {
    mesh.update();
}