#version 330 core

out vec4 FragColor;

uniform float uMass;

vec3 starColor(float t)
{
    if (t < 0.25)
        return mix(
            vec3(1.0, 0.25, 0.15),
            vec3(1.0, 0.55, 0.15),
            t / 0.25);

    if (t < 0.5)
        return mix(
            vec3(1.0, 0.55, 0.15),
            vec3(1.0, 0.95, 0.4),
            (t - 0.25) / 0.25);

    if (t < 0.75)
        return mix(
            vec3(1.0, 0.95, 0.4),
            vec3(1.0, 1.0, 0.95),
            (t - 0.5) / 0.25);

    return mix(
        vec3(1.0, 1.0, 0.95),
        vec3(0.6, 0.8, 1.0),
        (t - 0.75) / 0.25);
}

void main()
{
    float mass = clamp(uMass, 0.0, 1.0);

    vec3 color = starColor(mass);

    float brightness = mix(0.8, 3.0, mass);

    FragColor = vec4(color * brightness, 1.0);
}