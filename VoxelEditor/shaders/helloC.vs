#version 450

layout (location = 0) in vec3 position;

uniform mat4 view; //matriz de view
uniform mat4 projection; //matriz de projeção (perspectiva)
uniform mat4 model; //matriz de modelo (transformações do objeto)

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
}