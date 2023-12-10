#version 430 core
#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 2
#endif

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
out vec4 frag_color;

struct Light
{
	vec3 diffuseColor;
	float diffuseStrength;
};

struct AmbientLight 
{
	Light base;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	Light base;
	vec3 position;
	vec3 direction;
	float edge;
};

struct Material
{
	float specularStrength;
	float shininess;
};

uniform sampler2D texture0;
uniform vec3 eyePosition;

uniform AmbientLight aLight;
uniform DirectionalLight dLight;
uniform PointLight pLights[NUM_POINT_LIGHTS];
uniform SpotLight sLight;
uniform Material mat;

vec4 calcAmbientLight(Light a)
{
	vec4 ambient = vec4(a.diffuseColor, 1.0f) * a.diffuseStrength;
	return ambient;
}

vec4 calcLightByDirection(Light l, vec3 dir)
{
	// Calculate diffuse lighting first.
	float diffuseFactor = max(dot(normalize(normal),normalize(-dir)), 0.0f); // Lambert's Cosine Law.
	vec4 diffuse = vec4(l.diffuseColor, 1.0f) * l.diffuseStrength * diffuseFactor;

	// Then calculate specular light.
	vec4 specular = vec4(0,0,0,0);
	if (diffuseFactor > 0.0f && l.diffuseStrength > 0.0f)
	{
		vec3 fragToEye = normalize(eyePosition - fragPos);
		vec3 reflectedVertex = normalize(reflect(dir, normalize(normal)));

		float specularFactor = dot(fragToEye, reflectedVertex);
		if (specularFactor > 0.0f)
		{
			specularFactor = pow(specularFactor, mat.shininess);
			specular = vec4(l.diffuseColor * mat.specularStrength * specularFactor, 1.0f);
		}
	}
	return (diffuse + specular);
}

vec4 calcDirectionalLight()
{
	return calcLightByDirection(dLight.base, dLight.direction);
}

vec4 calcPointLight(PointLight p)
{
	vec3 direction = fragPos - p.position;
	float distance = length(direction);
	direction = normalize(direction);
	// First calculate diffuse and specular.
	vec4 color = calcLightByDirection(p.base, direction);
	// Then calculate attenuation.
	float attenuation = 1.0 / (p.constant + p.linear * distance + p.quadratic * (distance * distance));
	return (color * attenuation);
}

vec4 calcSpotLight(SpotLight s)
{
	vec4 color = vec4(0,0,0,0);
	vec3 rayDirection = normalize(s.position - fragPos);
	float slFactor = dot(rayDirection, s.direction);
	if (slFactor > s.edge)
	{
		vec3 direction = normalize(fragPos - s.position);
		// First calculate diffuse and specular.
		color = calcLightByDirection(s.base, direction);
		// Then calculate the edge of the spotlight.
		color *= (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - s.edge)));
	}
	return color;
}

void main()
{
	// Calculate lighting.
	vec4 calcColor = vec4(0,0,0,0);
	calcColor += calcAmbientLight(aLight.base);
	calcColor += calcDirectionalLight();
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
		calcColor += calcPointLight(pLights[i]);
	calcColor += calcSpotLight(sLight);
	
	frag_color = texture(texture0, texCoord) * vec4(color, 1.0f) * calcColor;
}