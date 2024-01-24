/***************************************************************************************
  BusyBox Client for E-Ink Display (3 Color)

  Created:   22 Jan 2024

  Hardware
    - Microcontroller       ESP32 Dev Board
    - Screen Module         WeAct 2.9 E-Paper Module (3 color)

  Description: This is a painlessMesh client (viewer) for the BusyBox network.

 **************************************************************************************/

#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold18pt7b.h>
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

#define ENABLE_GxEPD2_GFX 0

GxEPD2_3C<GxEPD2_290_C90c, GxEPD2_290_C90c::HEIGHT> display(GxEPD2_290_C90c(/*CS=5*/ 5, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEM029C90 128x296, SSD1680

painlessMesh mesh;

enum status {FreeStatus, BusyStatus, KnockStatus, WaitingStatus}; 
int currentState = status::WaitingStatus;


// An attempt at making the status vertical - WIP
// void busyHandler() {
//     display.setFullWindow();
//     display.fillScreen(GxEPD_RED);
//     display.setRotation(0);
//     // display.fillRect(0, 0, display.width(), display.height() / 4, GxEPD_BLACK);
//     display.setTextColor(GxEPD_WHITE);
//     display.setFont(&FreeMonoBold24pt7b);
//
//     int16_t tbx, tby;
//     uint16_t tbw, tbh;
//     display.getTextBounds("B", 0, 0, &tbx, &tby, &tbw, &tbh);
//     uint16_t x = ((display.width() - tbw) / 2) - tbx;
//     uint16_t y = (((display.height() - tbh) / 4)) - tby;
//
//     display.setCursor(x, y);
//     display.println("B");
//     display.println("U");
//     display.println("S");
//     display.print("Y");
//
//     display.display(false);
// }


// Busy Handler
void busyHandler() {
    display.setFullWindow();
    display.fillScreen(GxEPD_RED);
    display.setRotation(1);
    display.drawRect(10, 10, display.width() - 20, display.height() - 20, GxEPD_WHITE);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeMonoBold18pt7b);
    display.setTextSize(2);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Busy", 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;

    display.setCursor(x, y);
    display.print("Busy");

    display.display(false);

    currentState = status::BusyStatus;
}


// Free Handler
void freeHandler() {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(1);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold18pt7b);
    display.setTextSize(2);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Free", 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;

    display.setCursor(x, y);
    display.print("Free");

    display.display(false);

    currentState = status::FreeStatus;
}


// Knock Handler
void knockHandler() {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(1);
    display.drawRect(10, 10, display.width() - 20, display.height() - 20, GxEPD_RED);
    display.drawRect(20, 20, display.width() - 40, display.height() - 40, GxEPD_BLACK);
    display.drawRect(30, 30, display.width() - 60, display.height() - 60, GxEPD_RED);
    display.setTextColor(GxEPD_RED);
    display.setFont(&FreeMonoBold18pt7b);
    display.setTextSize(2);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Knock", 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;

    display.setCursor(x, y);
    display.print("Knock");

    display.display(false);

    currentState = status::KnockStatus;
}


void sendStatusConfirmation(uint32_t from) {
    String msg = String(currentState);
    mesh.sendSingle(from, msg);
    // mesh.sendBroadcast(msg);
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


void waitSplash() {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(1);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold18pt7b);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Waiting...", 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = ((display.height() - tbh) / 2) - tby;

    display.setCursor(x, y);
    display.print("Waiting...");

    display.display(false);
}


void setup() {
    display.init(115200, true, 50, false);

    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);

    waitSplash();

    display.hibernate();
}


void loop() {
    mesh.update();
}