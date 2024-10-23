#include <ESP32Servo.h>

// Create Servo objects
Servo topEyelid, bottomEyelid, horizontal, vertical;

// Define min and max angles for each servo
int minTopEyelid = 90, maxTopEyelid = 20;
int minBottomEyelid = 90, maxBottomEyelid = 180;
int minHorizontal = 60, maxHorizontal = 130;
int minVertical = 60, maxVertical = 120;

// Pins for servos
int pinTopEyelid = 25;
int pinBottomEyelid = 33;
int pinHorizontal = 32;
int pinVertical = 26;

// Pins for joystick
int pinX = 14;
int pinY = 12;
int pinSW = 13;

void shuteye() {
    topEyelid.write(minTopEyelid);
    bottomEyelid.write(minBottomEyelid);
}

void openeye() {
    topEyelid.write(maxTopEyelid);
    bottomEyelid.write(maxBottomEyelid);
}

void setup() {
    Serial.begin(115200);
    pinMode(pinSW, INPUT_PULLUP);

    // Attach servos to their respective pins
    topEyelid.attach(pinTopEyelid);
    bottomEyelid.attach(pinBottomEyelid);
    horizontal.attach(pinHorizontal);
    vertical.attach(pinVertical);

    // Start all servos in the center position
    // topEyelid.write(90);
    // bottomEyelid.write(90);
    topEyelid.write(maxTopEyelid);
    bottomEyelid.write(maxBottomEyelid);
    horizontal.write(90);
    vertical.write(90);
}

int servoAngle(const int analogInput, const int minAngle, const int maxAngle) {
    // stop jittering when joystick is centered
    if (analogInput > 2500 && analogInput < 3400) {
        return (minAngle + maxAngle) / 2;
    }

    return map(analogInput, 0, 4095, minAngle, maxAngle);
}

void loop() {
    const int x = analogRead(pinX);
    const int y = analogRead(pinY);
    const int pressed = digitalRead(pinSW);
    Serial.printf("X: %d, Y: %d, SW: %d\n", x, y, pressed);
    delay(20);

    const int horizontalAngle = servoAngle(x, minHorizontal, maxHorizontal);
    const int verticalAngle = servoAngle(y, minVertical, maxVertical);
    horizontal.write(horizontalAngle);
    vertical.write(verticalAngle);

    if (!pressed) {
        shuteye();
    } else {
        // adjust eyelids according to vertical angle
        const int topEyelidAngle = map(verticalAngle, minVertical, maxVertical, maxTopEyelid, minTopEyelid);
        const int bottomEyelidAngle = map(verticalAngle, minVertical, maxVertical, minBottomEyelid, maxBottomEyelid);
        topEyelid.write(topEyelidAngle);
        bottomEyelid.write(bottomEyelidAngle);
    }
}
