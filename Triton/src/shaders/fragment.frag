#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;
layout(location = 4) in vec3 fragTangent;
layout(location = 5) in vec3 fragBiTangent;
layout(location = 6) in vec4 LightPosAndPower;
layout(location = 7) in vec3 LightColor;

layout(location = 0) out vec4 outColor;

const float factor = 1;

const vec3 specColor = vec3(0.1, 0.1, 0.1);
const float shininess = 100;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

void main() {
    
    vec3 diffuseColor = texture(texSampler,fragTexCoord).rgb;
    vec3 ambientColor = diffuseColor * 0.0001f;

    vec3 normal = fragNormal;
    normal = normalize(normal);
    vec3 lightDir = LightPosAndPower.xyz - fragPos;
    float distance = dot(lightDir,lightDir);
    lightDir = normalize(lightDir);
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0) {

        vec3 viewDir = normalize(-fragPos);

        // this is blinn phong
        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);
        
    
    }
    vec3 colorLinear = 
        ambientColor + 
        diffuseColor * lambertian * LightColor * LightPosAndPower.w / distance +
        specColor * specular * LightColor * LightPosAndPower.w / distance ;
    // apply gamma correction (assume ambientColor, diffuseColor and specColor
    // have been linearized, i.e. have no gamma correction in them)
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
    // use the gamma corrected color in the fragment 
     
    outColor = vec4(colorGammaCorrected,1.0f);

}
