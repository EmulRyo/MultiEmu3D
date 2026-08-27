#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform float metallic;
uniform float roughness;
uniform mat4 lightVP;
uniform sampler2D shadowMap;
uniform float shadowTexelSize;

out vec4 finalColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 normal, vec3 halfway, float surfaceRoughness)
{
    float a = surfaceRoughness*surfaceRoughness;
    float a2 = a*a;
    float nDotH = max(dot(normal, halfway), 0.0);
    float nDotH2 = nDotH*nDotH;
    float denominator = nDotH2*(a2 - 1.0) + 1.0;
    return a2/max(PI*denominator*denominator, 0.0001);
}

float GeometrySchlickGGX(float nDotV, float surfaceRoughness)
{
    float r = surfaceRoughness + 1.0;
    float k = (r*r)/8.0;
    return nDotV/max(nDotV*(1.0 - k) + k, 0.0001);
}

float GeometrySmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float surfaceRoughness)
{
    return GeometrySchlickGGX(max(dot(normal, viewDirection), 0.0), surfaceRoughness)*
           GeometrySchlickGGX(max(dot(normal, lightDirection), 0.0), surfaceRoughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 f0)
{
    return f0 + (1.0 - f0)*pow(1.0 - cosTheta, 5.0);
}

float ShadowFactor(vec3 normal, vec3 lightDirection)
{
    vec4 lightSpacePosition = lightVP*vec4(fragPosition, 1.0);
    vec3 projected = lightSpacePosition.xyz/lightSpacePosition.w;
    projected = projected*0.5 + 0.5;

    if (projected.z > 1.0 || projected.x <= 0.0 || projected.x >= 1.0 || projected.y <= 0.0 || projected.y >= 1.0)
        return 0.0;

    float bias = max(0.002*(1.0 - dot(normal, lightDirection)), 0.0002) + 0.0001;
    float shadow = 0.0;
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            vec2 sampleCoords = projected.xy + vec2(x, y)*shadowTexelSize;
            if (sampleCoords.x <= 0.0 || sampleCoords.x >= 1.0 || sampleCoords.y <= 0.0 || sampleCoords.y >= 1.0)
                continue;
            float closestDepth = texture(shadowMap, sampleCoords).r;
            shadow += projected.z - bias > closestDepth ? 1.0 : 0.0;
        }
    }
    return shadow/9.0;
}

void main()
{
    vec3 albedo = texture(texture0, fragTexCoord).rgb*colDiffuse.rgb;
    vec3 normal = normalize(fragNormal);
    vec3 viewDirection = normalize(viewPos - fragPosition);
    vec3 lightDirection = normalize(-lightDir);
    vec3 halfway = normalize(viewDirection + lightDirection);

    float nDotL = max(dot(normal, lightDirection), 0.0);
    vec3 f0 = mix(vec3(0.04), albedo, metallic);
    vec3 fresnel = FresnelSchlick(max(dot(halfway, viewDirection), 0.0), f0);
    float distribution = DistributionGGX(normal, halfway, roughness);
    float geometry = GeometrySmith(normal, viewDirection, lightDirection, roughness);
    vec3 specular = distribution*geometry*fresnel/max(4.0*max(dot(normal, viewDirection), 0.0)*nDotL, 0.0001);
    vec3 diffuse = (1.0 - fresnel)*(1.0 - metallic)*albedo/PI;
    vec3 direct = (diffuse + specular)*lightColor*nDotL*(1.0 - ShadowFactor(normal, lightDirection));
    vec3 color = ambientColor*albedo + direct;

    color = color/(color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));
    finalColor = vec4(color, 1.0);
}
