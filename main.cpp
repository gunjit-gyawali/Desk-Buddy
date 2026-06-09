#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

const char* ssid = "LAB-4";
const char* password = "Lab4@2081";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

enum Animation {
    IDLE,
    BLINK,
    LOOK_LEFT,
    LOOK_RIGHT,
    LOOK_UP,
    LOOK_DOWN
};

Animation currentAnim = IDLE;

unsigned long nextChange = 0;
unsigned long animStart = 0;

void connectWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void setupTime() {
    configTime(20700, 0, "pool.ntp.org", "time.nist.gov");
}

String getTimeString() {
    struct tm t;

    if (!getLocalTime(&t)) {
        return "--:--";
    }

    char buf[6];
    strftime(buf, sizeof(buf), "%H:%M", &t);

    return String(buf);
}

void drawClock() {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(45, 0);
    display.print(getTimeString());
}

void drawEye(int cx, int cy, int ox, int oy) {
    display.fillRoundRect(cx - 18, cy - 12, 36, 24, 8, SSD1306_WHITE);
    display.fillCircle(cx + ox, cy + oy, 5, SSD1306_BLACK);
}

void renderIdle() {
    drawEye(40, 38, 0, 0);
    drawEye(88, 38, 0, 0);
}

void renderLook(int x, int y) {
    drawEye(40, 38, x, y);
    drawEye(88, 38, x, y);
}

void renderBlink() {
    display.fillRoundRect(22, 38, 36, 4, 2, SSD1306_WHITE);
    display.fillRoundRect(70, 38, 36, 4, 2, SSD1306_WHITE);
}

void pickNextAnimation() {
    int r = random(6);
    currentAnim = (Animation)r;
    animStart = millis();

    if (currentAnim == BLINK) {
        nextChange = millis() + 250;
    } else {
        nextChange = millis() + random(1000, 3000);
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        while (1) {
            delay(100);
        }
    }

    connectWiFi();
    setupTime();

    randomSeed(esp_random());

    nextChange = millis() + 1000;
}

void loop() {
    display.clearDisplay();

    drawClock();

    if (millis() >= nextChange) {
        pickNextAnimation();
    }

    switch (currentAnim) {
        case IDLE:
            renderIdle();
            break;
        case BLINK:
            renderBlink();
            break;
        case LOOK_LEFT:
            renderLook(-6, 0);
            break;
        case LOOK_RIGHT:
            renderLook(6, 0);
            break;
        case LOOK_UP:
            renderLook(0, -4);
            break;
        case LOOK_DOWN:
            renderLook(0, 4);
            break;
    }

    display.display();
    delay(16);
}