// fragment shader
#version 410

in vec3 Position;
in vec3 Normal;
in vec3 ReflectDir;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;

uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ka;            // Diffuse reflectivity
uniform vec3 Ks;            // Diffuse reflectivity
uniform float Shininess;    // Specular shininess factor

uniform samplerCube cubemap;

layout( location = 0 ) out vec4 FragColor;

vec3 ads( )
{
    vec3 s = normalize( vec3(LightPosition) - Position );
    vec3 v = normalize(vec3(-Position));
    vec3 r = reflect( -s, Normal );
    
    return
    ( Ka +
                      Kd * max( dot(s, Normal), 0.0 ) +
                      Ks * pow( max( dot(r,v), 0.0 ), Shininess ) );
}

void main() {
    FragColor = vec4(ads(), 1.0);
    
    //FragColor = clamp(vec4(texture(cubemap, ReflectDir)), 0.0, 1.0);
    
}
