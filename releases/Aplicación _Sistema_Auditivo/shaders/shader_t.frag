#version 330
in vec4 vColor;
in vec2 TexCoord0;
out vec4 color;
uniform sampler2D theTexture;
void main()
{
	color=texture(theTexture,TexCoord0);
}