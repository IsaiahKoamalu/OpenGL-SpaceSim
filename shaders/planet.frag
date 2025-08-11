#version 330 core
in VS_OUT
{
    vec3 worldPos;
    vec3 worldN;
    vec3 worldT;
    vec3 worldB;
    vec2 uv;
} fs;

out vec4 FragColor;

uniform sampler2D uAlbedoTex;
uniform sampler2D uNormalTex;
uniform sampler2D uNightTex;
uniform bool uHasAlbedo = false;
uniform bool uHasNormal = false;
uniform bool uHasNight = false;

// Light + material
uniform vec3 uSunDir = normalize(vec3(1.0, -0.4, -0.2));
uniform vec3 uAlbedoColor = vec3(0.30, 0.60, 0.90);
uniform float uGloss = 0.0; // 0=matte, 1=shiny
uniform float uSpecPower = 64.0;

// Atmosphere
uniform vec3 uRimColor = vec3(0.35, 0.55, 1.0);
uniform float uRimPower = 2.0; //Note: higher=thinner
uniform float uRimStrength = 0.4;

// Cam Pos
uniform vec3 uCameraWorld;

vec3 srgb_to_linear(vec3 c)
{
    return pow(c, vec3(2.2));
}
vec3 linear_to_srgb(vec3 c)
{
    return pow(max(c, 0.0), vec3(1.0/2.2));
}

void main()
{
    mat3 TBN = mat3(normalize(fs.worldT), normalize(fs.worldB), normalize(fs.worldN));

    // Norm (world)
    vec3 N = normalize(fs.worldN);
    if(uHasNormal)
    {
        vec3 tnorm = texture(uNormalTex, fs.uv).rgb * 2.0 - 1.0;
        N = normalize(TBN * tnorm);
    }

    // View & Light dirs (world)
    vec3 V = normalize(uCameraWorld - fs.worldPos);
    vec3 L = normalize(uSunDir);

    // Albedo (lin)
    vec3 albedo = uAlbedoColor;
    if(uHasAlbedo)
    {
        vec3 srgb = texture(uAlbedoTex, fs.uv).rgb;
        albedo = srgb_to_linear(srgb);
    }

    //Diffuse
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * NdotL;

    // Specular
    vec3 H = normalize(L + V);
    float spec = (uGloss > 0.0 && NdotL > 0.0) ? pow(max(dot(N, H), 0.0), mix(8.0, uSpecPower, uGloss)) : 0.0;
    vec3 specCol = mix(vec3(0.0), vec3(1.0), uGloss) * spec;

    //Ambient
    vec3 ambient = albedo * 0.03;

    // Night
    vec3 night = vec3(0.0);
    if(uHasNight)
    {
        float nightMask = smoothstep(0.05, 0.0, NdotL);
        night = texture(uNightTex, fs.uv).rgb * nightMask * 0.7;
        night = srgb_to_linear(night);
    }

    //Atmosphere
    float rim = pow(1.0 - max(dot(N, V), 0.0), uRimPower) * uRimStrength;
    vec3 rimCol = uRimColor * rim;

    // Combine lin
    vec3 colorLinear = ambient + diffuse + specCol + rimCol + night;

    // Output
    FragColor = vec4(linear_to_srgb(colorLinear), 1.0);
}