// per fragment shader
#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 2) in vec3 VertexNormal;



out vec3 Position;
out vec3 Normal;
out vec3 ReflectDir;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    Normal = normalize( NormalMatrix * VertexNormal);
    Position = vec3( ModelViewMatrix * vec4(VertexPosition,1.0) );
    gl_Position = MVP * vec4(VertexPosition,1.0);
    ReflectDir = reflect(Position, Normal);
}

