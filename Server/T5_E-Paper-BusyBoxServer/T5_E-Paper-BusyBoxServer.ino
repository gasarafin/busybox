/***************************************************************************************
  BusyBox Server for LilyGo T5 E-Paper

  Created:   22 Dec 2023

  Hardware
    - Microcontroller       LilyGo T5 E-Paper Dev Board (DEPG0213BN)

  Description: This is a painlessMesh server for the BusyBox network. This is written
  for an ESP32 device with an 2 color e-ink display. Using the built-in button, this
  device can change the status for all the BusyBox client viewers. The onboard screen
  will display all clients connected to the network along with their currently set
  status.

 **************************************************************************************/

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Button2.h>
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

#define BUTTON_1 39

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // DEPG0213BN 122x250, SSD1680, TTGO T5 V2.4.1, V2.3.1
Button2 btn1(BUTTON_1);

painlessMesh mesh;

bool isBusy = false;
bool meshStatusHeaderNotSet = true;
byte meshStatusY = 0;
// uint32_t connectedNodes[6] = {0,0,0,0,0,0};

void button_init() {
    btn1.setPressedHandler([](Button2 &b) {
        statusToggler();
    });
}

void sendMessage() {
    mesh.sendBroadcast(String(isBusy));
}

void statusToggler() {
    isBusy = !isBusy;
    meshStatusHeaderNotSet = true;

    sendMessage();

    if (isBusy) {
        busyHandler();
    } else {
        freeHandler();
    }
}

void busyHandler() {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.fillRect(0, 0, display.width(), display.height() / 4, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeMonoBold18pt7b);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Busy", 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = (((display.height() / 4) - tbh) / 2) - tby;

    display.setCursor(x, y);
    display.print("Busy");

    display.display(false);
}

void freeHandler() {
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.setRotation(0);
    display.drawRect(0, 0, display.width(), display.height() / 4, GxEPD_BLACK);
    display.setTextColor(GxEPD_BLACK);
    display.setFont(&FreeMonoBold18pt7b);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("Free", 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = ((display.width() - tbw) / 2) - tbx;
    uint16_t y = (((display.height() / 4) - tbh) / 2) - tby;

    display.setCursor(x, y);
    display.print("Free");

    display.display(false);
}

// void newConnectionCallback(uint32_t nodeId) {
//     // Write new connection nodeIDs to array (or vector) to know everything connected
//     // Use in mesh status check to keep better track of nodes that are connected but not updated
// }

// void droppedConnectionCallback(uint32_t nodeId) {
//     // Remove dropped connection nodeIDs from array (or vector) to keep connected list accurate
// }

void receivedCallback(uint32_t from, String &msg) {
    if (meshStatusHeaderNotSet) {
        meshStatusHeader();
        meshStatusHeaderNotSet = false;

        meshStatusY = (display.height() / 4) + 40;
    }

    meshStatusCheck(from, msg);
}

void meshStatusHeader() {
    display.setTextColor(GxEPD_BLACK);
    display.setFont();
    display.setPartialWindow(0, (display.height() / 4) + 20, display.width(), (display.height() / 4) + 40);
    display.firstPage();

    do {
        display.setCursor(0, (display.height() / 4) + 20);
        display.println("Mesh Status");
        display.println("-----------");
    } while (display.nextPage());
}

void meshStatusCheck(uint32_t from, String &msg) {
    display.setTextColor(GxEPD_BLACK);
    display.setFont();
    display.setPartialWindow(0, (display.height() / 4) + 40, display.width(), display.height());
    display.firstPage();

    do {
        display.setCursor(0, meshStatusY);
        display.printf("%u: %s\n", from, (msg.toInt() ? "Busy" : "Free"));
    } while (display.nextPage());
    meshStatusY += 10;
}

void setup() {
    display.init(115200, true, 2, false);
    statusToggler();
    button_init();

    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onReceive(&receivedCallback);
//    mesh.onNewConnection(&newConnectionCallback);
//    mesh.onDroppedConnection(&droppedConnectionCallback);

    display.hibernate();
}

void loop() {
    btn1.loop();
    mesh.update();
}