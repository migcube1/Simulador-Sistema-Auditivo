#version 330
layout (location =0) in vec3 pos;
layout (location =2) in vec2 tex;
out vec4 vColor;
out vec2 TexCoord0;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
void main()
{
	gl_Position=projection * view* model * vec4(pos,1.0f);
	vColor=vec4(1.0f,0.0f,0.0,1.0f);
	TexCoord0=tex;
}