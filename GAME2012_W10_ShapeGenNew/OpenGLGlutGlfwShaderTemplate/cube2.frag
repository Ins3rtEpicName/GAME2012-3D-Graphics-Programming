#version 430 core

in vec3 color;
in vec2 texCoord;
out vec4 frag_color;

uniform sampler2D texture0;

void main()
{
	vec4 texColor = texture(texture0, texCoord) * vec4(color, 1.0f);
	if(texColor.a < 0.1)
        discard;
	frag_color = texColor;
}