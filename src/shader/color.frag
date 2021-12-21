#version 330 core

struct Material
{
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    float shininess;
};

// PointLight was transformed to SpotLight
// struct PointLight {
//     vec3 position;
    
//     float constant;
//     float linear;
//     float quadratic;

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };  
// #define NR_POINT_LIGHTS 0
//uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutoffAngle;

  vec3 color;

  float constant;
  float linear;
  float quadratic;
};

//uniform float spotLight;
#define NR_SPOT_LIGHTS 4
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

out vec4 fragColor;
vec3 ambient;
vec3 diffuse;
vec3 specular;

float k_a = 0.1; //ambient coefficient
vec3 globalAmbientLightColorRGB = vec3(0.9,0.9,0.9);

//https://learnopengl.com/Lighting/Basic-Lighting was a great help
in vec3 tNormal;
in vec3 tFragPos;
in vec2 tUV;
float diffuseCoefficient = 1.0;

uniform vec3 globalDirectionalLightColorRGB;
uniform vec3 globalDirectionalLightDirection;//

float specularCoefficient = 0.1;
uniform vec3 viewPosition;

uniform Material uMaterial;


//PointLight -> transformed to SpotLight
// vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
// {
//     vec3 lightDir = normalize(light.position - fragPos);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//     vec3 reflectDir = reflect(-lightDir, normal);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
//     // attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
//     // combine results
//     vec3 ambient  = light.ambient * k_a * uMaterial.ambient * globalAmbientLightColorRGB;
//     vec3 diffuse  = light.diffuse * diff * diffuseCoefficient * uMaterial.diffuse * globalDirectionalLightColorRGB * dot(tNormal, globalDirectionalLightColorRGB);
//     vec3 specular = light.specular * specularCoefficient * uMaterial.specular * spec * globalDirectionalLightColorRGB * pow(max (dot(viewDir, reflectDir), 0.0), uMaterial.shininess * 2);

    
//     ambient  *= attenuation;
//     diffuse  *= attenuation;
//     specular *= attenuation;

//     return (ambient + diffuse + specular);
// }

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    
    if (theta > light.cutoffAngle) {
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
         // /*
        // combine results
        vec3 ambient  = light.color * k_a * uMaterial.ambient;
        //vec3 ambient  = light.color * k_a * uMaterial.ambient * globalAmbientLightColorRGB;
        vec3 diffuse  = light.color * diff * diffuseCoefficient * uMaterial.diffuse * dot(tNormal, globalDirectionalLightColorRGB);
        //vec3 diffuse  = light.color * diff * diffuseCoefficient * uMaterial.diffuse * globalDirectionalLightColorRGB * dot(tNormal, globalDirectionalLightColorRGB);
        vec3 specular = light.color * specularCoefficient * uMaterial.specular * spec * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
        //vec3 specular = light.color * specularCoefficient * uMaterial.specular * spec * globalDirectionalLightColorRGB * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
        //*/
        //return (light.color * attenuation);

    } else {
        //return light.color * uMaterial.ambient * globalAmbientLightColorRGB;
        return vec3 (0.0, 0.0, 0.0);
    }

}

void main(void)
{   
    vec3 viewDir = normalize(viewPosition - tFragPos);
    vec3 reflectDir = reflect(-globalDirectionalLightDirection, tNormal);
    

    // directional light
    ambient = k_a * uMaterial.ambient * globalAmbientLightColorRGB;
    diffuse = diffuseCoefficient * uMaterial.diffuse * globalDirectionalLightColorRGB * dot(tNormal, globalDirectionalLightColorRGB);
    specular = specularCoefficient * uMaterial.specular * globalDirectionalLightColorRGB * pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);

    vec3 result = (ambient + diffuse + specular);
    
    for(int i = 0; i < NR_SPOT_LIGHTS; i++){
        result += CalcSpotLight(spotLights[i], tNormal, tFragPos, viewDir);
    }

    fragColor = vec4(result, 1.0);
}
