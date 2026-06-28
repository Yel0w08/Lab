#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in float vMass;
in float vColorSeed;

out vec4 FragColor;

uniform vec3 uViewPos;

vec3 mainSequenceColor(float mass, float seed)
{
    float t = clamp(mass + (seed - 0.5) * 0.06, 0.0, 1.0);
    return mix(vec3(1.0, 0.55, 0.15), vec3(0.3, 0.5, 1.0), t);
}

void main()
{
    float mass = clamp(vMass, 0.0, 1.0);

    vec3 baseColor = mainSequenceColor(mass, vColorSeed);

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
