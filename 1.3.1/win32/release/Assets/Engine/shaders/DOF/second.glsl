[vert]
void main(void)
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
}

[frag]
uniform sampler2D Tex0;

void main (void)
{
	gl_FragData[0] = texture2D(Tex0, gl_TexCoord[0].st);
}
