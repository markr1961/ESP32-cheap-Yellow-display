// from https://majenko.co.uk/blog/our-blog-1/flexible-led-fading-14
// modified to use ESP32 ledc library

#include <Arduino.h>

// Pins for the LEDs
const uint8_t RED = 4;
const uint8_t GREEN = 16;
const uint8_t BLUE = 17;

const uint32_t fadeSpeed = 20;

int freq = 2000;    // frequency
int resolution = 8;   // Resolution

// Array of colour sequences. This is a 3D array - that is, an array of arrays.
// Each array slice consists of an array of three values, R, G and B.
uint8_t colors[][3] = {
    {255, 0, 0}, // Red
    {0, 0, 255}, // Blue
    {0, 255, 0}, // Green 
};

// How many color entries are there?
#define NUM_COLORS (sizeof(colors) / sizeof(colors[0]))

// The current color pointer
uint8_t color = 0;

// Main setup routine to configure the pins.
void setup() {
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    digitalWrite(RED, 0);
    digitalWrite(GREEN, 0);
    digitalWrite(BLUE, 0);
    ledcAttach(RED, freq, resolution);
    ledcAttach(GREEN, freq, resolution);
    ledcAttach(BLUE, freq, resolution);

    Serial.begin(115200);
}

// Main loop
void loop() {
    if (fadeTo(RED, GREEN, BLUE, colors[color][0], colors[color][1], colors[color][2], fadeSpeed)) {
        color++;
        if (color == NUM_COLORS) {
            color = 0;
        }
        // dwell on the new color.
        delay(fadeSpeed * 50);
    }
}

// This is where the magic happens
//
// This function remembers internally (using static variables) the current state of each of the
// colors, and adjusts the brightness of each color one step towards the requested target color.
// When all three colors match the target color it returns 'true' so you can change to a different
// target color.
bool fadeTo(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t redVal, uint8_t greenVal, uint8_t blueVal, uint32_t del) {
    // These store the current brightness values
    static uint8_t redCurrent = 0;
    static uint8_t greenCurrent = 0;
    static uint8_t blueCurrent = 0;

    // Keep track of if any color has faded or not
    bool finished = true;

    // Adjust the red ...
    if (redCurrent > redVal) {
        redCurrent--;
        finished = false;
    } else if (redCurrent < redVal) {
        redCurrent++;
        finished = false;
    }

    // ... green ...
    if (greenCurrent > greenVal) {
        greenCurrent--;
        finished = false;
    } else if (greenCurrent < greenVal) {
        greenCurrent++;
        finished = false;
    }

    // ... and blue values.
    if (blueCurrent > blueVal) {
        blueCurrent--;
        finished = false;
    } else if (blueCurrent < blueVal) {
        blueCurrent++;
        finished = false;
    }

    // Update the outputs with the new values
//    analogWrite(redPin, redCurrent);
//    analogWrite(greenPin, greenCurrent);
//    analogWrite(bluePin, blueCurrent);
    ledcWrite(redPin, redCurrent);
    ledcWrite(greenPin, greenCurrent);
    ledcWrite(bluePin, blueCurrent);

    Serial.printf("red: %3d, green %3d, blue: %3d\n", redCurrent, greenCurrent, blueCurrent);

    // Pause as long as is requested
    delay(del);

    // and finally return if we met the target colors or not
    return finished;
}
