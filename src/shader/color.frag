#version 330 core

struct Material
{
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
    float shininess;
};

out vec4 fragColor;
vec3 ambient;
vec3 diffuse;
vec3 specular;

float k_a = 0.1; //ambient coefficient
vec3 globalAmbientLightColorRGB = vec3(0.9,0.9,0.9);

//https://learnopengl.com/Lighting/Basic-Lighting was a great help
uniform vec3 lightPos = vec3(0.0, 10.0, 0.0);
in vec3 tNormal;
in vec3 tFragPos;
in vec2 tUV;
float diffuseCoefficient = 1.0;
uniform vec3 globalDirectioalLightColorRGB; //= vec3(0.9,0.0,0.0);
uniform vec3 globalDirectionalLightDirection;// = vec3(1.0, 1.0, 0.0);

float specularCoefficient = 0.1;
uniform vec3 viewPosition;

uniform Material uMaterial;

void main(void)
{   
    ambient = k_a * uMaterial.ambient * globalAmbientLightColorRGB;

    diffuse = diffuseCoefficient * uMaterial.diffuse * globalDirectioalLightColorRGB * dot(tNormal, globalDirectioalLightColorRGB);

    //TODO per light source
    //do not use globalDirectionalLightDirection, but dir (and color) of light source
    //not sure if dot(tNormal, globalDirectionalLightDirection) is the correct angle.
    vec3 viewDir = normalize(viewPosition - tFragPos);
    vec3 reflectDir = reflect(-globalDirectionalLightDirection, tNormal);


    //specular = specularCoefficient * uMaterial.specular * globalDirectioalLightColorRGB * pow(max (dot(tNormal, globalDirectionalLightDirection), 0.0), uMaterial.shininess); 
    specular = specularCoefficient * uMaterial.specular * globalDirectioalLightColorRGB * pow(max (dot(viewDir, reflectDir), 0.0), uMaterial.shininess * 2); 

    //fragColor = vec4(uMaterial.diffuse + ambient, 1.0);
    //fragColor = vec4(ambient + diffuse, 1.0);
    fragColor = vec4(ambient + (diffuse + specular), 1.0);


}
