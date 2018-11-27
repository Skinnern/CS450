#version 330 compatibility


//out vec texture
//out vec normal




//reset

//object manipulation







out vec2 vST; // texture coords
out vec3 vN; // normal vector
out vec3 vL; // vector from point to light
out vec3 vE; // vector from point to eye
uniform float uTime;
const float PI = 3.14159;

uniform bool vertPattern;

vec3 LightPosition = vec3( 5., 5., 0. );

void
main( )
{
    vST = gl_MultiTexCoord0.st;
    vec4 pos = gl_ModelViewMatrix * gl_Vertex;

    vN = normalize( gl_NormalMatrix * gl_Normal ); // normal vector
    vL = LightPosition - pos.xyz; // vector from the point to the light position
    vE = vec3( 0., 0., 0. ) - pos.xyz; // vector from the point to the eye position
	//Set up our vector manipulation
	vec3 vert = gl_Vertex.xyz;
	if(vertPattern)
	{
		//Modify Y axis
		vert.y = vert.y +  cos( uTime *8);
		//Modify X axis
		vert.x = vert.x +  cos( uTime *8);
		//Modify Z axis
		vert.z = vert.z +  cos( uTime *8);
	}
	//reset back to origin
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vert, 1.);

}