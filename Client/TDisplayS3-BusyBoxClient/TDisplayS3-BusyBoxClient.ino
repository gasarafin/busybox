/***************************************************************************************
  BusyBox Server for LilyGo T-Display S3  (WORK IN PROGRESS)

  Created:   23 Jan 2024

  Hardware
    - Microcontroller       LilyGo T-Display S3
  
  Description: This is a painlessMesh client (viewer) for the BusyBox network. This is
  written for an ESP32 device with a screen and will display either the free or busy
  message. The device is not able to change on it's own - it only changes when connected
  to a BusyBox server device (compatible with any in the server folder). If you are
  looking for a standalone BusyBox that is only changed locally using the onboard
  buttons, look in the standalone folder of this project.

 **************************************************************************************/

//TODO Works overall, but not pretty. Must do following:
// 1. Fix fonts and text spacing
// 2. Tooltip buttons
// 3. Hibernate function

#include "TFT_eSPI.h"
#include "Free_Fonts.h"
#include "BusyBoxClient.h"
#include "painlessMesh.h"
#include <Button2.h>

#if defined __has_include
#if __has_include("creds.h")
#include "creds.h"
#else

// BusyBox Mesh Network Settings - Change to create private network
#define MESH_SSID       "BusyBox"
#define MESH_PASSWORD   "P@ssw0rd"
#define MESH_PORT       5555
// End BusyBox Mesh Network Settings

#endif
#endif

#define BUTTON_1                 0
#define BUTTON_2                 14

Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

painlessMesh mesh;

TFT_eSPI tft = TFT_eSPI();

byte meshStatusY = 0;

const char* statusMessage[] = {"Free", "Busy", "Knock"};
enum status {FreeStatus, BusyStatus, KnockStatus, HibernateStatus}; 
int currentState = status::HibernateStatus;


void button_init() {
    btn1.setClickHandler([](Button2 &b) {
        freeHandler();
    });

    btn1.setDoubleClickHandler([](Button2 &b) {
        knockHandler();
    });

    btn2.setClickHandler([](Button2 &b) {
        busyHandler();
    });

    btn2.setDoubleClickHandler([](Button2 &b) {
        meshHibernate();
    });
}


void button_loop() {
    btn1.loop();
    btn2.loop();
}


void sendMessage() {
    mesh.sendBroadcast(String(currentState));
}


// Tooltips
//TODO stubbed from standalone
void toolTips() {
    tft.setRotation(1);

    // Set Busy Line
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Busy -->", tft.width(), 0);

    // Set Free Line
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);
    tft.setTextDatum(BR_DATUM);
    tft.drawString("Free -->", tft.width(), tft.height());
    tft.setRotation(0);
}


void meshHibernate() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FSB18);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Sleep", tft.width() / 2, tft.height() / 7);

    // Not Implemented Message
    tft.setFreeFont(FS9);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Not Implemented Yet", tft.width() / 2, (tft.height() / 7) * 3);
    // End Not Implemented Message

    currentState = status::HibernateStatus;
}


// Busy Handler
void busyHandler() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FSB18);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Busy", tft.width() / 2, tft.height() / 7);

    currentState = status::BusyStatus;

    sendMessage();

    meshStatusY = (tft.height() / 4) + 40;
}


// Free Handler
void freeHandler() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_GREEN);
    tft.setTextColor(TFT_WHITE);
    tft.setFreeFont(FSB18);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Free", tft.width() / 2, tft.height() / 7);

    currentState = status::FreeStatus;

    sendMessage();

    meshStatusY = (tft.height() / 4) + 40;
}


// Free Handler
void knockHandler() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_YELLOW);
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(FSB18);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Knock", tft.width() / 2, tft.height() / 7);

    currentState = status::KnockStatus;

    sendMessage();

    meshStatusY = (tft.height() / 4) + 40;
}


void receivedCallback(uint32_t from, String &msg) {
    meshStatusHeader();

    meshStatusCheck(from, msg);
}


void meshStatusHeader() {
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(FS9);

    tft.setCursor(0, (tft.height() / 4) + 20);
    tft.println("Mesh Status");
    tft.println("-----------");
}


void meshStatusCheck(uint32_t from, String &msg) {
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(FS9);

    tft.setCursor(0, meshStatusY);
    tft.printf("%u: %s", from, statusMessage[msg.toInt()]);

    meshStatusY += 10;
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
    // TODO Splash screen image wrong size
    tft.pushImage(18, 2, 204, 132, BusyBoxClient);
    delay(4000);

    button_init();

    // Uncomment for connection debugging
    // Serial.begin(115200);
    // mesh.setDebugMsgTypes(ERROR | CONNECTION | STARTUP);

    // Mesh Init
    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);

    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK);
    tft.setFreeFont(FSB18);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Init", tft.width() / 2, tft.height() / 7);
}


void loop() {
    button_loop();
    mesh.update();
}