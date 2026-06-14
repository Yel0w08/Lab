#version 330 core

in vec3 vNormal;
in vec3 vFragPos;

out vec4 FragColor;

uniform float uMass;
uniform float uColorSeed;
uniform vec3 uViewPos;

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

vec3 mainSequenceColor(float mass, float seed)
{
    float offset = (seed - 0.5) * 0.3;
    float t = clamp(mass + offset, 0.0, 1.0);
    return starColor(t);
}

void main()
{
    float mass = clamp(uMass, 0.0, 1.0);

    vec3 baseColor = mainSequenceColor(mass, uColorSeed);

    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-vFragPos);
    vec3 viewDir = normalize(uViewPos - vFragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

    float ambient = 0.05;
    float diffuse = mix(0.3, 1.0, diff);
    float specular = mix(0.0, 0.6, mass) * spec;
    float emissive = mix(0.15, 1.0, mass);

    vec3 color = baseColor * (ambient + diffuse) + vec3(specular) + baseColor * emissive;

    float brightness = mix(0.8, 3.0, mass);

    FragColor = vec4(color * brightness, 1.0);
}
