#version 410
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 tint;

void main()
{
	//color = vec4(tint, 0.0) * texture(image, TexCoords);
	vec4 c = texture(image, TexCoords);
	if(c.a < 0.1f)
		discard;
	color = c;
	//color = vec4(1.0, 1.0, 1.0, 0.1);
}