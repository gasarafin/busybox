/***************************************************************************************
  BusyBox Server for ESP32

  Created:   7 Dec 2023

  Hardware
    - Microcontroller       ESP32-WROOM Dev Board
  
  Description: This is a painlessMesh server for the BusyBox network. This is written
  for a screenless ESP32, therefore all feedback is given within console logs. The
  built-in 'BOOT' button on the dev board is used to switch between free and busy.

 **************************************************************************************/

#include "painlessMesh.h"
#include <Arduino.h>

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

painlessMesh mesh;

bool isBusy = false;

void sendMessage() {
    mesh.sendBroadcast(String(isBusy));
    isBusy = !isBusy;
}

// Due to the lack of a built-in screen connection and callbacks are via console.
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New Connection With Node: %u\n", nodeId);
}

void receivedCallback(uint32_t from, String &msg) {
    Serial.printf("%u: %s\n", from, (msg.toInt() ? "Busy" : "Free"));
}

void setup() {
    Serial.begin(115200);

    mesh.init(MESH_SSID, MESH_PASSWORD, MESH_PORT);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onReceive(&receivedCallback);

    pinMode(0, INPUT_PULLUP);
}

void loop() {
    if (digitalRead(0) == LOW) {
        Serial.println(isBusy ? "Sending busy signal." : "Sending free signal.");
        sendMessage();
        delay(500);
    }

    mesh.update();
}
