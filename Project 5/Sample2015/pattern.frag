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
		//this will cause our cycle, where we have a point of rest, and a transition
		if( vST.s >= .1 && vST.s <= (2 * (sin(uTime * PI))) ) 
		{
			//change color depending on time 
			myColor = vec3( sin(5. * uTime), sin(5. * uTime), 0 );
		}
	}
    vec3 ambient = uKa * myColor;
    float d = max( dot(Normal,Light), 0. ); //we will only do the diffuse if the light can see the point
    myColor = uColor;
    myColor = vec3( .4, .4, .4 );
    //apply our frag
    vec3 diffuse = uKd * d * myColor;
    gl_FragColor = vec4( ambient + diffuse, .25 );
}