//Nicholas Skinner
//CS450 Final
//Music Visualizer

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "glew.h"
#include <GL/glut.h>
#include <GL/freeglut.h>

#include <GL/gl.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <memory.h>
#include <cmath>
#include <fftw3.h>
#include "sndfile.h"
#include "portaudio.h"


using namespace std;





#define _USE_MATH_DEFINES
#include <math.h>
#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)

#endif


#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "Nicholas Skinner - CS450 Final" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

//functions for final



//***************************************************************
void transformed_display();
void timeDomain();
void reshape(int w, int h);
void init();
void init_audio();
void close_audio();
float lowpassFilter(float *sample, int size);
void windowFunction(float *sample, int size);
//***************************************************************



//declarations for audio
//***************************************************************
#define SAMPLE_RATE 441000
#define WIN 1024
#define buffer_size 1024
#define PI 3.14159265358979323846264338327950288
//***************************************************************



//more audio stuff
//***************************************************************
const int fft_size = 512;
double windowedSample[WIN];
float *buf, leftch[WIN], rightch[WIN];
int readSize=0, len;//to strore the size of current frame for the display func to pass to fft
static char CHANNEL[10];//MONO or STEREO
bool finished=false;//finished playing or not
int eff=1;//variable indicating the theme number
int channel=1;
double bartop[fft_size], radii[5];
char audpath[100] = "Horizon.wav" ;//demo file
PaStream *stream;//portaudio stream
//***************************************************************


float SceneRot; 

//even more audio stuff and declarations
//****************************************************************
GLsizei width = 800;
GLsizei height = 600;
GLsizei last_width = width;
GLsizei last_height = height;
// light 0 position
GLfloat light0_pos[4] = {2.0f, 1.2f, 4.0f, 1.0f};
// light 1 parameters
GLfloat light1_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat light1_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light1_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat light1_pos[4] = {-2.0f, 0.0f, -4.0f, 1.0f};
//***************************************************************


//struct layer
//***************************************************************
struct layer
{
	float radius;
	float zoom;
	struct layer *next;
	struct layer *prev;
};
//***************************************************************

typedef layer layers;
layers *head=NULL;

//***************************************************************

void	Axes( float );
void	HsvRgb( float[3], float [3] );

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

//idle func
//*************************************************
void idle()
{
	glutPostRedisplay();
}
//*************************************************

//init funct
//*************************************************
void init()
{
	glClearColor(0.0f, 0.05f, 0.05f, 0.3f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_LIGHTING);

	glLightModeli(GL_FRONT_AND_BACK, GL_TRUE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_COLOR_MATERIAL);


	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glEnable(GL_LIGHT1);

}
//***********************************************************

//reshape func
//***********************************************************
void reshape(int w, int h)
{
	width = w; height = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (GLfloat)(w) / (GLfloat)(h), 1.0, 300.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 3.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);


	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
}

//*************************************************




//time domain
//***************************************************
void timeDomain()
{
	GLfloat x = -2.0f, inc = 4.0f / readSize, y = 0.0f;

	glPushMatrix();
	float ang = float(360.0 / (float)len), angle = 0, u;
	glColor3f(0, float(rand()) / float(RAND_MAX), 0.7);
	glNormal3f(0.0f, 0.0f, 1.0f);// set vertex normals (for somewhat controlled lighting)
	glTranslatef(x, y, 0.0f);
	glScalef(inc, 1.0, 1.0);

	glBegin(GL_LINE_STRIP);//drawing the timedomain waveform
	for (int i = 0; i < readSize; i++) {
		glVertex3f(x++, buf[i], 0);
		glVertex3f(x++, buf[i], 1);
		//glVertex3f(x++, buf[i]-.15, 0);
		//glVertex3f(x++, buf[i]-.15, 1);
	}
	glEnd();
	glPopMatrix();

	glColor3f(1, 1, 1);
	glFlush();
	glutSwapBuffers();
}
//********************************************************



//low pass filter
//********************************************
float lowpassFilter(int size)
{
	static float lastInput = 0;
	float output = (*buf + lastInput) / 2;
	lastInput = *buf;
	return output;
}
//********************************************************

//window function
//********************************************************
void windowFunction(int size)//Hanning window function
{
	for (int i = 0; i < size; i++)
	{
		double multiplier = 0.5 * (1 - cos(2 * PI*i / (size - 1)));
		windowedSample[i] = buf[i] * multiplier;
	}

}
//*******************************************************

//ourdata
//*******************************************************


struct OurData//sound data
{
	SNDFILE *sndFile;//the file
	SF_INFO sfInfo;//information about the file
	int position;//index to the file
};

//performing our fft and then drawing the frequency domain wavrform
//***********************************************************
void transformed_display()
{
	if (channel == 2)
	{
		int k = 0;
		for (int i = 0; i<readSize; i++)
		{
			leftch[k] = buf[i];
			rightch[k] = buf[i + 1];
			k++;
		}
		len = k;
	}
	else
	{
		for (int i = 0; i<readSize; i++)
		{
			leftch[i] = buf[i];
			rightch[i] = leftch[i];
		}
		len = readSize;
	}

	int fftSize = fft_size;
	if (channel == 1) fftSize /= 2;

	lowpassFilter(fftSize);
	windowFunction(fftSize);

	fftw_complex *out;
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize);
	fftw_plan p;
	p = fftw_plan_dft_r2c_1d(fftSize, windowedSample, out, FFTW_ESTIMATE);

	fftw_execute(p);
	fftw_destroy_plan(p);

	float powerSpectrum[fft_size / 2], dbSpectrum[fft_size / 2], normSpect[fft_size / 2];

	for (int i = 0; i<fftSize / 2; i++) {
		powerSpectrum[i] = 0.0;
		dbSpectrum[i] = 0.0;
	}

	int minm = 0, maxm = 0;
	float mam = 0;
	for (int i = 0; i<fftSize / (channel * 2); i++) {
		powerSpectrum[i] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
		if (powerSpectrum[i]>powerSpectrum[maxm]) maxm = i;
		dbSpectrum[i] = 20.0 * log10(powerSpectrum[i]);
		normSpect[i] = dbSpectrum[i] / (fftSize / 2);
		if (normSpect[i]<normSpect[minm]) minm = i;
		if (normSpect[i]>mam) mam = normSpect[i];
	}

	float binSize = SAMPLE_RATE / fftSize;
	float peakFreq = maxm * binSize;
	//if(channel==1)
	//peakFreq/=2.0;

	if (eff == 2)//drawing frequency spectrum bars
	{
		GLfloat x = -2.0f, inc, y = 0.0f;
		x = -2.0f;
		inc = 4.0f / (fftSize / (channel * 2));
		y = 0.6*inc;

		glPushMatrix();
		glTranslatef(0, -1, 0);
		glScalef(1, 3, 1);

		glPointSize(3);

		GLfloat base = abs(normSpect[minm]);
		for (int j = 0; j<fftSize / (channel * 2); j++)
		{
			int foo = int(normSpect[j]);//used for controlling 
			float foo1 = float(normSpect[j] - (float)(foo));//the colors of the bars

			glBegin(GL_QUADS);
			glColor3f(1, 1, 0.5);
			glVertex3f(x, 0, 0);

			glColor3f(0.5 + foo1, 1, 0.7 + foo1);
			glVertex3f(x, base + normSpect[j], 0);

			glColor3f(0.5 + foo1, 1, 0.7 + foo1);
			glVertex3f(x + y, base + normSpect[j], 0);

			glColor3f(1, 1, 0.5);
			glVertex3f(x + y, 0, 0);
			glEnd();
			if (bartop[j]<base + normSpect[j])
				bartop[j] = base + normSpect[j];
			else
				bartop[j] = bartop[j] - 0.0008;

			glColor3f(1, 1, 0.3);
			glBegin(GL_QUADS);
			glVertex3f(x, bartop[j], 0);
			glVertex3f(x, bartop[j] + 0.005, 0);
			glVertex3f(x + y, bartop[j] + 0.005, 0);
			glVertex3f(x + y, bartop[j], 0);
			glEnd();

			x += inc;

		}
		glPopMatrix();

		glColor3f(1, 1, 1);
	}
	else if (eff == 3)//Third theme (peak frequencies and left-right channel waveforms)
	{
		//adding the newest peak frequency (i.e. the current one)
		layers *cur = new layers;
		cur->radius = peakFreq / 10000;
		cur->zoom = 1.0;
		cur->next = head;
		if (head != NULL)
			head->prev = cur;
		cur->prev = NULL;
		head = cur;

		glColor3f(.5, 0.5, 0.7);

		//The circle for "holding" the waveform
		layers *t = head;
		float v;
		v = t->radius;
		while (v>1.0)
			v -= 1.0;
		v += 1;
		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		for (float i = 0; i <= 360; i++)
		{
			glVertex3f(v*cos(i*PI / 180), v*sin(i*PI / 180), 0);
			glVertex3f(v*cos(i*PI / 180), v*sin(i*PI / 180), 1);
		}
		glEnd();
		glPopMatrix();


		//The peak freq layers
		float val;
		layers *temp = head;
		while (temp != NULL)
		{
			glPushMatrix();
			if (channel == 1) glScalef(0.25f, 0.25f, 0.25f);
			glBegin(GL_LINE_STRIP);
			for (float i = 0; i <= 360; i++)
			{
				val = temp->radius*temp->zoom;

				glVertex2f(val*cos(i*PI / 180), val*sin(i*PI / 180));
			}
			glEnd();
			glPopMatrix();
			temp->zoom += 0.010;
			if (temp->zoom >= 2.0)
			{
				temp->next = NULL;
				temp->prev->next = temp->next;
			}
			temp = temp->next;
		}

		//The waveform on the circle
		float ang = float(360.0 / (float)len), angle = 0, u;
		glColor3f(0, float(rand()) / float(RAND_MAX), 0.7);
		glPushMatrix();
		glBegin(GL_LINES);
		for (int i = 0; i<len; i++)
		{
			u = abs(leftch[i]);
			if (mam>0.1) u /= 2.0;
			u = abs(v - u);
			glVertex2f(v*cos(angle*PI / 180), v*sin(angle*PI / 180));
			glVertex2f((u)*cos(angle*PI / 180), (u)*sin(angle*PI / 180));
			angle += ang;
		}
		glEnd();
		glPopMatrix();


		//The horizontal waveform in the background
		GLfloat x = -2.0f, inc = 4.0f / len, y = 0.0f;
		glPushMatrix();
		glColor3f(1, 1, 0);
		if (mam>0.1)
			glScalef(1.0, 0.5f, 1.0);

		glBegin(GL_LINES);
		for (int i = 0; i<len; i++)
		{
			glVertex2f(x, 0);
			glVertex2f(x, abs(leftch[i]));
			glVertex2f(x, 0);
			glVertex2f(x, 0 - abs(rightch[i]));

			x += inc;
		}

		glEnd();
		glPopMatrix();

		glColor3f(1, 1, 1);
	}

	glFlush();
	glutSwapBuffers();
}
//***********************************************************


//callback function
//*******************************************************
int Callback(const void *input,
	void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* paTimeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	OurData *data = (OurData *)userData; /* we passed a data structure
										 into the callback so we have something to work with */
	float *cursor; /* current pointer into the output  */
	float *out = (float *)output;
	int thisSize = frameCount;
	int nFrames = frameCount;
	int thisRead;

	cursor = out; /* set the output cursor to the beginning */
	while (thisSize > 0)
	{
		/* seek to our current file position */
		sf_seek(data->sndFile, data->position, SEEK_SET);

		/* are we going to read past the end of the file?*/
		if (thisSize > (data->sfInfo.frames - data->position))
		{
			/*if we are, only read to the end of the file*/
			thisRead = data->sfInfo.frames - data->position;
			/* and then loop to the beginning of the file */
			data->position = 0;
		}
		else
		{
			/* otherwise, we'll just fill up the rest of the output buffer */
			thisRead = thisSize;
			/* and increment the file position */
			data->position += thisRead;
		}

		/* since our output format and channel interleaving is the same as
		sf_readf_int's requirements */
		/* we'll just read straight into the output buffer */
		nFrames = sf_readf_float(data->sndFile, cursor, thisRead);
		readSize = thisRead;
		buf = cursor;
		if (nFrames<frameCount)
		{
			finished = true;
			return paComplete;
		}


		/* increment the output cursor*/
		cursor += thisRead;
		/* decrement the number of samples left to process */
		thisSize -= thisRead;

	}

	return paContinue;
}
//*********************************************************************

//initalize our audio
//*********************************************************************
void init_audio()
{
	OurData *data = (OurData *)malloc(sizeof(OurData));

	PaError error;
	PaStreamParameters outputParameters;

	data->position = 0;
	data->sfInfo.format = 0;

	data->sndFile = sf_open(audpath, SFM_READ, &data->sfInfo);

	if (!data->sndFile)
	{
		cout << "Error opening file. Please check if the file exists at the specified location.\n";
		getchar();
		getchar();
		exit(1);
	}


	if (data->sfInfo.channels == 2)
		channel = 2;

	for (int i = 0; i<fft_size; i++)
		bartop[i] = -1.0;

	/* start portaudio */
	Pa_Initialize();

	/* set the output parameters */
	outputParameters.device = Pa_GetDefaultOutputDevice();
	outputParameters.channelCount = data->sfInfo.channels;
	if (data->sfInfo.channels == 1) strcpy(CHANNEL, "MONO"); else strcpy(CHANNEL, "STEREO");
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = 0.2;
	outputParameters.hostApiSpecificStreamInfo = 0;


	error = Pa_OpenStream(&stream, 0, &outputParameters, data->sfInfo.samplerate, WIN, paNoFlag, Callback, data);

	if (error)
	{
		printf("error opening output, error code = %i\n", error);
		Pa_Terminate();
		exit(1);
	}

	/* when we start the stream, the callback starts getting called */
	Pa_StartStream(stream);

}
//***********************************************************


//close our audio
void close_audio()
{
	Pa_StopStream(stream);
	Pa_CloseStream(stream);
	Pa_Terminate();
}





void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	float Time;
	#define MS_IN_THE_ANIMATION_CYCLE	10000
	//. . .
	int ms = glutGet(GLUT_ELAPSED_TIME);	// milliseconds
	ms %= MS_IN_THE_ANIMATION_CYCLE;
	Time = (float)ms / (float)MS_IN_THE_ANIMATION_CYCLE;
	SceneRot = Time * 360;

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void Display()
{


	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.08f, 0.08f, 0.3f);

	//printing now playing stuff...
	static char now[1024], str1[1024];
	int foo, len = strlen(audpath), foobar = 0;
	for (foo = 0; foo<len; foo++)
		if (audpath[foo] == '/')
			foobar = foo + 1;
	int ind = 0;
	for (int bar = foobar; bar<len - 4; bar++)
		str1[ind++] = audpath[bar];

	str1[ind] = '\0';


	//*************************************************************
	sprintf(now, "Song : %s || Channel: %s", str1, CHANNEL);
	char *ch = now;
	glPushMatrix();
	glTranslatef(-1.8, 1.2, 0.0);
	glScalef(0.001f, .0006f, .01f);
	glPointSize(0.5);
	for (ch = now; *ch != '\0'; ch++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *ch);
	//*************************************************************

	glPopMatrix();





	if (eff == 1)//first theme: time domain waveform
		timeDomain();
	else
		transformed_display();

	if (finished == true)
	{
		close_audio();
		exit(0);
	}


	// uniformly scale the scene:
	if (Scale < MINSCALE)
		Scale = MINSCALE;
	glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
	glPopMatrix();

	if (AxesOn)
		glCallList(AxesList);



}



void
DispEffMenu(int id)
{
	eff = id;

	glutSetWindow(MainWindow);
	glutPostRedisplay();
}

void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int DispMenu = glutCreateMenu(DispEffMenu);
	glutAddMenuEntry("Timedomain Waveform", 1);
	glutAddMenuEntry("Frequency Bars", 2);
	glutAddMenuEntry("Other", 3);

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(	  "Display",	   DispMenu);
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	init_audio();
	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc(reshape);
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc(idle);

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the object:

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );

		glBegin( GL_QUADS );

			glColor3f( 0., 0., 1. );
			glNormal3f( 0., 0.,  1. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f( -dx,  dy,  dz );

			glNormal3f( 0., 0., -1. );
				glTexCoord2f( 0., 0. );
				glVertex3f( -dx, -dy, -dz );
				glTexCoord2f( 0., 1. );
				glVertex3f( -dx,  dy, -dz );
				glTexCoord2f( 1., 1. );
				glVertex3f(  dx,  dy, -dz );
				glTexCoord2f( 1., 0. );
				glVertex3f(  dx, -dy, -dz );

			glColor3f( 1., 0., 0. );
			glNormal3f(  1., 0., 0. );
				glVertex3f(  dx, -dy,  dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f(  dx,  dy,  dz );

			glNormal3f( -1., 0., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f( -dx,  dy, -dz );
				glVertex3f( -dx, -dy, -dz );

			glColor3f( 0., 1., 0. );
			glNormal3f( 0.,  1., 0. );
				glVertex3f( -dx,  dy,  dz );
				glVertex3f(  dx,  dy,  dz );
				glVertex3f(  dx,  dy, -dz );
				glVertex3f( -dx,  dy, -dz );

			glNormal3f( 0., -1., 0. );
				glVertex3f( -dx, -dy,  dz );
				glVertex3f( -dx, -dy, -dz );
				glVertex3f(  dx, -dy, -dz );
				glVertex3f(  dx, -dy,  dz );

		glEnd( );

	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case '1':
			eff = 1;
			break;

		case '2':
			eff = 2;
			break;

		case '3':
			eff = 3;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}



// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
