#version 330
layout (location =0) in vec3 pos;
//layout (location =1) in vec3 color;
out vec4 vColor;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 color;
void main()
{
	gl_Position=view*projection*model*vec4(pos,1.0f);
	vColor=vec4(color,1.0f);
	//vColor=vec4(clamp(pos,0.0f,1.0f),1.0f);
}