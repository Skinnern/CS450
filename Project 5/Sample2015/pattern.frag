#version 330 compatibility

uniform float uKa, uKd, uKs; // coefficients of each type of lighting
uniform vec3 uColor; // object color
uniform vec3 uSpecularColor; // light color
uniform float uShininess; // specular exponent
uniform float uTime;
const float PI = 3.14159;

uniform bool fragPattern;

in vec2 vST; // texture cords
in vec3 vN; // normal vector
in vec3 vL; // vector from point to light
in vec3 vE; // vector from point to eye

void
main( )
{
    vec3 Normal = normalize(vN);
    vec3 Light = normalize(vL);
    vec3 Eye = normalize(vE);
    vec3 myColor = uColor;

	if(fragPattern)
	{
		if( vST.s >= 0. && vST.s <= (1. * (sin(uTime * PI))) ) 
		{
			myColor = vec3( sin(5. * uTime), sin(5. * uTime + PI), cos(5. * uTime) );
		}
	}
    vec3 ambient = uKa * myColor;
    float d = max( dot(Normal,Light), 0. ); // only do diffuse if the light can see the point
    myColor = uColor;
    myColor = vec3( .7, .7, .7 );
    
    vec3 diffuse = uKd * d * myColor;
    gl_FragColor = vec4( ambient + diffuse, 1. );
}