/***************************************************************************************
  BusyBox Standalone for LilyGo T-Display

  Created:   7 Dec 2023

  Hardware
    - Microcontroller       LilyGo T-Display

  Description: This is a standalone device within the BusyBox umbrella. This is written
  for an ESP32 device with a screen. This device will display either the free or busy
  message; the message is changed by pressing the associated button on the dev board.
  Look at the device screen after startup for more details.

 **************************************************************************************/

#include <TFT_eSPI.h>
#include <SPI.h>
#include "Button2.h"
#include "esp_adc_cal.h"
#include "BusyBoxSmooth.h"

#define ADC_EN 14
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0

TFT_eSPI tft = TFT_eSPI(135, 240);
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);

char buff[512];
int vref = 1100;

// Sleep Delay
void espDelay(int ms) {
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void button_init() {
    // Device Sleep Handler
    btn1.setLongClickHandler([](Button2 &b) {
    int r = digitalRead(TFT_BL);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);
    tft.drawString("Press again to wake up", tft.width() / 2, tft.height() / 2);
    espDelay(6000);
    digitalWrite(TFT_BL, !r);
    tft.writecommand(TFT_DISPOFF);
    tft.writecommand(TFT_SLPIN);

    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
    delay(200);
    esp_deep_sleep_start();
    });

    // Busy Handler
    btn1.setPressedHandler([](Button2 &b) {
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.drawString("BUSY", tft.width() / 2, tft.height() / 2);

    tft.setTextSize(2);
    tft.setTextDatum(BR_DATUM);
    tft.drawString("Free -->", tft.width(), tft.height());
    });

    // Free Handler
    btn2.setPressedHandler([](Button2 &b) {
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(8);
    tft.drawString("FREE", tft.width() / 2, tft.height() / 2);

    tft.setTextSize(2);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Busy -->", tft.width(), 0);
    });
}

void button_loop() {
    btn1.loop();
    btn2.loop();
}

void setup() {
    pinMode(ADC_EN, OUTPUT);
    digitalWrite(ADC_EN, HIGH);

    // Screen Init
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(1);
    tft.setSwapBytes(true);

    // BusyBox Splash Screen
    tft.pushImage(18, 2, 204, 132, BusyBoxSmooth);
    espDelay(4000);

    button_init();

    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        vref = adc_chars.vref;
    }

    tft.fillScreen(TFT_BLACK);

    // Set Busy Line
    tft.setTextSize(2);
    tft.setTextColor(TFT_RED);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Busy -->", tft.width(), 0);

    // Set Sleep Line
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Top Button Long Press:", tft.width() / 2, tft.height() / 2 - 4);
    tft.drawString("[Deep Sleep]", tft.width() / 2, tft.height() / 2 + 4);

    // Set Free Line
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN);
    tft.setTextDatum(BR_DATUM);
    tft.drawString("Free -->", tft.width(), tft.height());
}

void loop() {
    button_loop();
}