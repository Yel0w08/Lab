#include "Planet.h"

Planet solarSystem[] = {
    { "Mercury", 2439.7f,   57900000.0f,   88.0f,    1407.6f, GRAY   },
    { "Venus",   6051.8f,  108200000.0f,  224.7f,   -5832.5f, ORANGE },
    { "Earth",   6371.0f,  149600000.0f,  365.25f,     23.9f, BLUE   },
    { "Mars",    3389.5f,  227900000.0f,  687.0f,      24.6f, RED    },
    { "Jupiter", 69911.0f, 778500000.0f, 4331.0f,       9.9f, BEIGE  },
    { "Saturn",  58232.0f,1433000000.0f,10747.0f,      10.7f, GOLD   },
    { "Uranus",  25362.0f,2872000000.0f,30589.0f,     -17.2f, SKYBLUE},
    { "Neptune", 24622.0f,4495000000.0f,59800.0f,      16.1f, DARKBLUE }
};

const int PLANET_COUNT = 8;
const float SUN_RADIUS_KM = 700000.0f;
const float SIZE_SCALE = 1.0f / 5000.0f;
const float DISTANCE_SCALE = 1.0f / 20000000.0f;

float rotationAngles[PLANET_COUNT] = { 0 };
float orbitAngles[PLANET_COUNT] = { 0 };