/***************************************************************************************
  BusyBox Server for LilyGo T-Display S3

  Created:   23 Jan 2024

  Hardware
    - Microcontroller       LilyGo T-Display S3
  
  Description: This is a painlessMesh server for the BusyBox network.
 **************************************************************************************/

#include "TFT_eSPI.h"
#include "Fonts/GFXFF/gfxfont.h"
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

    btn1.setDoubleClickTime(500);
    btn2.setDoubleClickTime(500);
}


void button_loop() {
    btn1.loop();
    btn2.loop();
}


void sendMessage() {
    mesh.sendBroadcast(String(currentState));
}


// Tooltips
void toolTips() {
    tft.setRotation(0);
    tft.setTextFont(1);
    tft.setTextSize(2);

    // Set Divider Line
    tft.fillRect(0, tft.height() - 40, tft.width(), tft.height(), TFT_LIGHTGREY);
    tft.drawLine(tft.width() / 2, tft.height(), tft.width() / 2, tft.height() - 40, TFT_BLACK);
    tft.drawLine(0, tft.height() - 40, tft.width(), tft.height() - 40, TFT_BLACK);

    // Set Free Line
    tft.setTextColor(TFT_DARKGREEN);
    tft.setTextDatum(BL_DATUM);
    tft.drawString("Free", 0, tft.height() - 20);

    // Set Busy Line
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(BR_DATUM);
    tft.drawString("Busy", tft.width(), tft.height() - 20);

    // Set Knock Line
    tft.setTextColor(TFT_GOLD);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Knock", 0, tft.height() - 20);

    // Set Hibernate Line
    tft.setTextColor(TFT_BLUE);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Sleep", tft.width(), tft.height() - 20);

    // Set Click Numbers
    tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    tft.setTextDatum(BC_DATUM);
    tft.drawString("(1x)", tft.width() / 2, tft.height() - 20);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("(2x)", tft.width() / 2, tft.height() - 20);
}


void meshHibernate() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    toolTips();
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_BLUE);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold24pt7b);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Sleep", tft.width() / 2, tft.height() / 8);

    // Not Implemented Message
    tft.setTextColor(TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Not Implemented Yet", tft.width() / 2, (tft.height() / 8) * 3);
    // End Not Implemented Message

    currentState = status::HibernateStatus;
}


// Busy Handler
void busyHandler() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    toolTips();
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold24pt7b);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Busy", tft.width() / 2, tft.height() / 8);

    currentState = status::BusyStatus;

    sendMessage();

    meshStatusY = (tft.height() / 4) + 50;
}


// Free Handler
void freeHandler() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    toolTips();
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_GREEN);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold24pt7b);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Free", tft.width() / 2, tft.height() / 8);

    currentState = status::FreeStatus;

    sendMessage();

    meshStatusY = (tft.height() / 4) + 50;
}


// Knock Handler
void knockHandler() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    toolTips();
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_YELLOW);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold24pt7b);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Knock", tft.width() / 2, tft.height() / 8);

    currentState = status::KnockStatus;

    sendMessage();

    meshStatusY = (tft.height() / 4) + 50;
}


void receivedCallback(uint32_t from, String &msg) {
    meshStatusHeader();

    meshStatusCheck(from, msg);
}


void meshStatusHeader() {
    tft.setTextColor(TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(1);

    tft.setCursor(0, (tft.height() / 4) + 20);
    tft.println("Mesh Status");
    tft.println("-----------");
}


void meshStatusCheck(uint32_t from, String &msg) {
    tft.setTextColor(TFT_BLACK);
    tft.setTextFont(2);
    tft.setTextSize(1);

    tft.setCursor(0, meshStatusY);
    tft.printf("%u: %s", from, statusMessage[msg.toInt()]);

    meshStatusY += 10;
}


// Init Splash
void initSplash() {
    tft.setRotation(0);
    tft.fillScreen(TFT_WHITE);
    tft.fillRect(0, 0, tft.width(), tft.height() / 4, TFT_LIGHTGREY);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold24pt7b);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Init", tft.width() / 2, tft.height() / 8);
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

    initSplash();
    toolTips();
}


void loop() {
    button_loop();
    mesh.update();
}