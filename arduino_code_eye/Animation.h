#include <Servo.h>  // Cambiar de ESP32Servo.h a Servo.h para Arduino
#include "KeyFrame.h"

#ifndef ANIMATION_H
#define ANIMATION_H

class Animation {
private:
    KeyFrame* keyframes;       // Puntero a un arreglo de keyframes
    int numKeyFrames;          // Número de keyframes
    int currentKeyFrame;       // Índice del keyframe actual
    unsigned long startTime;   // Tiempo en que inició el keyframe actual

public:
    Animation(KeyFrame* frames, int numFrames)
        : keyframes(frames), numKeyFrames(numFrames), currentKeyFrame(0), startTime(0) {}

    void start() {
        currentKeyFrame = 0;
        startTime = millis();
    }

    bool update(Servo& topEyelid, Servo& bottomEyelid, Servo& horizontal, Servo& vertical) {
        if (currentKeyFrame >= numKeyFrames) {
            return false; // Animación terminada
        }

        KeyFrame currentFrame = keyframes[currentKeyFrame];
        unsigned long currentTime = millis();
        unsigned long elapsedTime = currentTime - startTime;

        if (elapsedTime >= currentFrame.duration) {
            // Pasar al siguiente keyframe
            currentKeyFrame++;
            if (currentKeyFrame < numKeyFrames) {
                startTime = currentTime;
            }
        } else {
            // Mover servos hacia los ángulos objetivo
            topEyelid.write(currentFrame.topEyelidAngle);
            bottomEyelid.write(currentFrame.bottomEyelidAngle);
            horizontal.write(currentFrame.horizontalAngle);
            vertical.write(currentFrame.verticalAngle);
        }

        return true; // La animación sigue en ejecución
    }
};

#endif //ANIMATION_H
