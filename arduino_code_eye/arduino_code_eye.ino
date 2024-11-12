//#define USE_JOYSTICK_INPUT  // Descomentar para activar el control mediante joystick
#include <Servo.h> // Librería estándar de servos para Arduino

#ifndef USE_JOYSTICK_INPUT
#include "KeyFrame.h"
#include "Animation.h"
#endif

// Crear objetos Servo
Servo topEyelid, bottomEyelid, horizontal, vertical;

// Definir ángulos mínimos y máximos para cada servo
int minTopEyelid = 90, maxTopEyelid = 20;
int minBottomEyelid = 90, maxBottomEyelid = 180;
int minHorizontal = 60, maxHorizontal = 130;
int minVertical = 60, maxVertical = 120;

// Pines para los servos
int pinTopEyelid = 9;
int pinBottomEyelid = 10;
int pinHorizontal = 11;
int pinVertical = 12;

// Pines para el joystick
int pinX = A1;
int pinY = A2;
int pinSW = 8;


#ifdef USE_JOYSTICK_INPUT
int servoAngle(const int analogInput, const int minAngle, const int maxAngle) {
  if (analogInput > 500 && analogInput < 523) { // Rango centrado para joystick
    return (minAngle + maxAngle) / 2;
  }
  return map(analogInput, 0, 1023, minAngle, maxAngle); // Escala de 0 a 1023 en Arduino
}
#else

KeyFrame smartKeyFrame(const int horizontalAngle, const int verticalAngle, const unsigned long duration) {
  const int maxTopEyelid2 = maxTopEyelid - (0.10 * maxTopEyelid);
  const int minTopEyelid2 = minTopEyelid + (0.05 * minTopEyelid);
  const int maxBottomEyelid2 = maxBottomEyelid + (0.10 * maxBottomEyelid);
  const int minBottomEyelid2 = minBottomEyelid - (0.05 * minBottomEyelid);
  const int topEyelidAngle = map(verticalAngle, minVertical, maxVertical, maxTopEyelid2, minTopEyelid2);
  const int bottomEyelidAngle = map(verticalAngle, minVertical, maxVertical, minBottomEyelid2, maxBottomEyelid2);
  return KeyFrame(topEyelidAngle, bottomEyelidAngle, horizontalAngle, verticalAngle, duration);
}

// Animaciones
KeyFrame blinkAnimationFrames[] = {
  KeyFrame(20, 180, 90, 90, 500),  // Abrir párpados en 500 ms
  KeyFrame(90, 90, 90, 90, 150)    // Cerrar párpados en 500 ms
};

KeyFrame sideToSideBlinkAnimationFrames[] = {
  KeyFrame(40, 160, minHorizontal, 90, 700),
  KeyFrame(90, 90, 100, 90, 150),
  KeyFrame(40, 160, maxHorizontal, 90, 700),
  KeyFrame(90, 90, 100, 90, 150),
};

KeyFrame diagonalSideToSideBlinkAnimationFrames[] = {
  smartKeyFrame(minHorizontal, minVertical, 500),
  smartKeyFrame(maxHorizontal, minVertical, 500),
  KeyFrame(90, 90, 100, 90, 150),
  smartKeyFrame(minHorizontal, maxVertical, 500),
  smartKeyFrame(maxHorizontal, maxVertical, 500),
  KeyFrame(90, 90, 100, 90, 150),
};

KeyFrame rollingEyesFrames[] = {
  smartKeyFrame(90, 60, 75),
  smartKeyFrame(130, 60, 75),
  smartKeyFrame(130, 90, 75),
  smartKeyFrame(130, 120, 75),
  smartKeyFrame(90, 120, 75),
  smartKeyFrame(60, 120, 75),
  smartKeyFrame(60, 90, 75),
  smartKeyFrame(60, 60, 75),
  smartKeyFrame(90, 60, 75),
};

KeyFrame centeredEyeAnimationFrames[] = {
  smartKeyFrame((minHorizontal + maxHorizontal) / 2, (minVertical + maxVertical) / 2, 1000),
};

// Crear objetos de Animación a partir de los keyframes
Animation blinkAnimation(blinkAnimationFrames, 2);
Animation sideToSideAnimation(sideToSideBlinkAnimationFrames, 4);
Animation diagonalSideToSideAnimation(diagonalSideToSideBlinkAnimationFrames, 6);
Animation centeredEyeAnimation(centeredEyeAnimationFrames, 1);
Animation rollingEyesAnimation(rollingEyesFrames, sizeof(rollingEyesFrames) / sizeof(KeyFrame));

// Arreglo de animaciones para facilitar el acceso
Animation *animations[] = {
  &blinkAnimation,
  &sideToSideAnimation,
  &diagonalSideToSideAnimation,
  &centeredEyeAnimation,
  &rollingEyesAnimation,
};
int currentAnimationIndex = 0;
Animation *activeAnimation = animations[currentAnimationIndex];

// Función para cambiar a una animación aleatoria
void switchToRandomAnimation() {
  int randomIndex = random(5); // Genera un índice aleatorio entre 0 y 4
  activeAnimation = animations[randomIndex];
  activeAnimation->start(); // Inicia la nueva animación
}

#endif

void shuteye() {
  topEyelid.write(minTopEyelid);
  bottomEyelid.write(minBottomEyelid);
}

void openeye() {
  topEyelid.write(maxTopEyelid);
  bottomEyelid.write(maxBottomEyelid);
}

void setup() {
  Serial.begin(9600);
  pinMode(pinSW, INPUT_PULLUP);

  // Adjuntar servos a sus respectivos pines
  topEyelid.attach(pinTopEyelid);
  bottomEyelid.attach(pinBottomEyelid);
  horizontal.attach(pinHorizontal);
  vertical.attach(pinVertical);

  // Inicializar todos los servos en la posición central
  topEyelid.write(maxTopEyelid);
  bottomEyelid.write(maxBottomEyelid);
  horizontal.write(90);
  vertical.write(90);

  // Inicializar generador de números aleatorios
  randomSeed(analogRead(A0));
}

void loop() {
#ifdef USE_JOYSTICK_INPUT
  const int x = analogRead(pinX);
  const int y = analogRead(pinY);
  const int pressed = digitalRead(pinSW);
  Serial.print("X: "); Serial.print(x); Serial.print(", Y: "); Serial.print(y); Serial.print(", SW: "); Serial.println(pressed);
  delay(20);

  const int horizontalAngle = servoAngle(x, minHorizontal, maxHorizontal);
  const int verticalAngle = servoAngle(y, minVertical, maxVertical);
  horizontal.write(horizontalAngle);
  vertical.write(verticalAngle);

  if (!pressed) {
    shuteye();
  } else {
    const int topEyelidAngle = map(verticalAngle, minVertical, maxVertical, maxTopEyelid, minTopEyelid);
    const int bottomEyelidAngle = map(verticalAngle, minVertical, maxVertical, minBottomEyelid, maxBottomEyelid);
    topEyelid.write(topEyelidAngle);
    bottomEyelid.write(bottomEyelidAngle);
  }

#else
  if (!activeAnimation->update(topEyelid, bottomEyelid, horizontal, vertical)) {
    switchToRandomAnimation(); // Cambiar a otra animación cuando finalice la actual
  }
#endif
}
