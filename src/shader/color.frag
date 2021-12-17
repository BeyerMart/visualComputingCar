#version 330 core

//SOURCE: https://github.com/Pilzschaf/OpenGLTutorial

out vec4 fColor;

//in vec3 vNormal;
//in vec3 vPosition;
in vec3 tNormal;
in vec3 tFragPos;

struct Material
{
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

struct PointLight
{
   vec3 position;

   vec3 diffuse;
   vec3 ambient;
   vec3 specular;
   
   float constant;
   float linear;
   float quadratic;
};

uniform Material uMaterial;
uniform DirectionalLight uDirectionalLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 viewPos;


//point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * uMaterial.diffuse;
    vec3 diffuse  = light.diffuse  * diff * uMaterial.diffuse;
    vec3 specular = light.specular * spec * uMaterial.specular;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcDirLight() {
    vec3 view = normalize(-tFragPos);
    vec3 normal = normalize(tNormal);

    vec3 light = normalize(-uDirectionalLight.direction);
    vec3 reflection = reflect(-uDirectionalLight.direction, normal);

    vec3 ambient = uDirectionalLight.ambient * uMaterial.diffuse;
    vec3 diffuse = uDirectionalLight.diffuse * max(dot(normal, light), 0.0) * uMaterial.diffuse;
    vec3 specular = uDirectionalLight.specular * pow(max(dot(reflection, view), 0.0), uMaterial.shininess) * uMaterial.specular;
    
    return (ambient + diffuse + specular);
}


void main(void){   
     
     vec3 viewDir = normalize(viewPos - tFragPos);
    //PointLight
    //light = normalize(uPointLightPosition - vPosition);
    //reflection = reflect(-light, normal);
    //ambient += uPointLight.ambient * uMaterial.diffuse;
    //diffuse += uPointLight.diffuse * max(dot(normal, light), 0.0) * uMaterial.diffuse;
    //specular += uPointLight.specular * pow(max(dot(reflection, view), 0.0), uMaterial.shininess) * uMaterial.specular;
    //fColor = vec4(ambient + diffuse + specular, 1.0f);
    
    float k_a = 0.1; //ambient coefficient
    vec3 ambient = k_a * uMaterial.ambient * uDirectionalLight.diffuse;

    float diffuseCoefficient = 1.0;
    vec3 diffuse = diffuseCoefficient * uMaterial.diffuse * uDirectionalLight.diffuse * dot(tNormal, uDirectionalLight.diffuse);

    //vec3 result = CalcDirLight();
    // phase 2: Point lights
    //for(int i = 0; i < NR_POINT_LIGHTS; i++){
        //pointLights[i].constant = 1.0;
        //result += CalcPointLight(pointLights[i], vNormal, vPosition, viewDir);
    //}
    
    //fColor = vec4(result, 1.0);
    fColor = vec4(ambient + diffuse, 1.0);
}
