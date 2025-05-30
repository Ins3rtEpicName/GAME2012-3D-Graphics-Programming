#version 430 core

in vec4 vColor;
out vec4 finalColor;

uniform vec4 lightDiffuse;
uniform vec4 materialDiffuse;
uniform vec4 lightAmbient;
uniform vec4 materialAmbient;
uniform vec4 lightSpecular;
uniform vec4 materialSpecular;
uniform float shininess;

in vec3 L;
in vec3 N;
in vec3 E;
vec3 nL, nN, nE;

void main(void) {

// Per Pixel (per fragment)
nE = normalize(E);
nN = normalize(N);
nL = normalize(L);

//Lambert's cosine law
float lambertTerm = dot(nN,nL);

float Kd = max (dot (nN, nL) , 0.0);
//Ambient Term
vec4 Ia = lightAmbient * materialAmbient;

//Diffuse Term
vec4 Id = vec4(0.0,0.0,0.0,1.0);

//Specular Term
vec4 Is = vec4(0.0,0.0,0.0,1.0);

if(lambertTerm > 0.0) //only if lambertTerm is positive
{
Id = lightDiffuse* materialDiffuse * lambertTerm; //add diffuse term

vec3 R = reflect(nL, nN);
float specular = pow(max(dot(R, nE), 0.0), max(shininess,1.0) );

Is = lightSpecular * materialSpecular * specular; //add specular term
}

finalColor = Ia + Id + Is;
finalColor.a = 1.0;
}