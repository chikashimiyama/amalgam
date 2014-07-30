// vertex shader
#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec3 VertexNormal;

out vec3 LightIntensity;

uniform vec4 LightPosition; // Light position in eye coords.
uniform vec3 Ld;            // Diffuse light intensity
uniform vec3 Kd;            // Diffuse reflectivity

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    vec3 tnorm = normalize( NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition,1.0);
    vec3 s = normalize(vec3(LightPosition - eyeCoords));
    vec3 v = normalize(-eyeCoords.xyz);
    vec3 r = reflect(-s, tnorm);
    float sDotN = max(dot(s, tnorm), 0.0);
    vec3 diffuse  = Ld * Kd * sDotN;
    vec3 spec = vec3(0.0);
    if(sDotN > 0.0){
        vec3 mSpec = vec3(1.0, 1.0, 1.0);
        spec = mSpec * pow( max( dot(r,v), 0.0 ), 10.0);
    }
    LightIntensity = diffuse + spec;
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
