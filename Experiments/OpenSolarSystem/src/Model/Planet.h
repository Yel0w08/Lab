#pragma once
#include "raylib.h"

struct Planet {
    const char* name;
    float radiusKm;
    float distanceFromSunKm;
    float orbitalPeriodDays;
    float rotationPeriodHours;
    Color color;
};

extern Planet solarSystem[];
extern const int PLANET_COUNT;
extern const float SUN_RADIUS_KM;
extern const float SIZE_SCALE;
extern const float DISTANCE_SCALE;
extern float rotationAngles[];
extern float orbitAngles[];