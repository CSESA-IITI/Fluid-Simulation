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
layout(location = 8) in float viewMode;

layout(location = 0) out vec4 outColor;

const float factor = 1;

const vec3 specColor = vec3(0.1, 0.1, 0.1);
const float shininess = 100;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

vec3 vectorToColor(vec2 v) {
    // Calculate phase (angle) for hue
    float hue = atan(v.y, v.x) / (2.0 * 3.14159265359); // Normalize to [0,1]
    hue = fract(hue); // Ensure hue wraps around
    
    // Calculate magnitude for value
    float value = length(v);
    // Optional: Normalize value if you expect large magnitudes
    // value = clamp(value / some_max_magnitude, 0.0, 1.0);
    
    // Saturation is max (1.0)
    float saturation = 1.0;
    
    // Convert HSV to RGB
    vec3 rgb;
    vec3 hsv = vec3(hue, saturation, value);
    
    // HSV to RGB conversion
    int i = int(hsv.x * 6.0);
    float f = hsv.x * 6.0 - float(i);
    float p = hsv.z * (1.0 - hsv.y);
    float q = hsv.z * (1.0 - hsv.y * f);
    float t = hsv.z * (1.0 - hsv.y * (1.0 - f));
    
    if (i == 0) rgb = vec3(hsv.z, t, p);
    else if (i == 1) rgb = vec3(q, hsv.z, p);
    else if (i == 2) rgb = vec3(p, hsv.z, t);
    else if (i == 3) rgb = vec3(p, q, hsv.z);
    else if (i == 4) rgb = vec3(t, p, hsv.z);
    else rgb = vec3(hsv.z, p, q);
    
    return rgb;
}



void main() {
    
    vec4 diffuseColor = texture(texSampler,fragTexCoord).rgba;
    vec4 ambientColor = diffuseColor * 0.0001f;    // use the gamma corrected color in the fragment 
     
    float mag = sqrt(diffuseColor.x * diffuseColor.x + diffuseColor.y * diffuseColor.y);

    //outColor = vec4(0.0f,-tanh(diffuseColor.w ),tanh(diffuseColor.w ),1.0f);
    if(viewMode == 1){
        outColor = vec4(0.0f,tanh(diffuseColor.w ),tanh(diffuseColor.w ),1.0f);
    }else if(viewMode == 2){
        outColor = vec4(0.0f,-tanh(diffuseColor.p * 10.0f),tanh(diffuseColor.p * 10.0f),1.0f);
    }else if(viewMode == 4){
        outColor = vec4(tanh(diffuseColor.w * vectorToColor(diffuseColor.xy)),1.0f);
    }
    else{
        outColor = vec4(tanh( vectorToColor(diffuseColor.xy)),1.0f);
    }
    //outColor = vec4(1.0f);
}
