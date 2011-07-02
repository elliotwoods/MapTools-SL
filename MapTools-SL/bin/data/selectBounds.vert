#version 120

varying vec3 PosW;

void main(void)
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	PosW = gl_Vertex.xyz / gl_Vertex.w;
}