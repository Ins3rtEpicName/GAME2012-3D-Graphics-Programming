//***************************************************************************
// Filename.cpp by Name - ID
//
// Assignment 1 submission.
//
// Description:
//	Click run to see the results.
//*****************************************************************************

////http://glew.sourceforge.net/
//The OpenGL Extension Wrangler Library (GLEW) is a cross-platform open-source C/C++ extension loading library. 
//GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target
//platform. OpenGL core and extension functionality is exposed in a single header file. GLEW has been tested on a 
//variety of operating systems, including Windows, Linux, Mac OS X, FreeBSD, Irix, and Solaris.
//
//http://freeglut.sourceforge.net/
//The OpenGL Utility Toolkit(GLUT) is a library of utilities for OpenGL programs, which primarily perform system - level I / O with the host operating system.
//Functions performed include window definition, window control, and monitoring of keyboardand mouse input.
//Routines for drawing a number of geometric primitives(both in solid and wireframe mode) are also provided, including cubes, spheresand the Utah teapot.
//GLUT also has some limited support for creating pop - up menus..

//OpenGL functions are in a single library named GL (or OpenGL in Windows). Function names begin with the letters glSomeFunction*();
//Shaders are written in the OpenGL Shading Language(GLSL)
//To interface with the window system and to get input from external devices into our programs, we need another library. For the XWindow System, this library is called GLX, for Windows, it is wgl,
//and for the Macintosh, it is agl. Rather than using a different library for each system,
//we use two readily available libraries, the OpenGL Extension Wrangler(GLEW) and the OpenGLUtilityToolkit(GLUT).
//GLEW removes operating system dependencies. GLUT provides the minimum functionality that should be expected in any modern window system.
//OpenGL makes heavy use of defined constants to increase code readability and avoid the use of magic numbers.Thus, strings such as GL_FILL and GL_POINTS are defined in header(#include <GL/glut.h>)

//https://glm.g-truc.net/0.9.9/index.html
////OpenGL Mathematics (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <string>

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 1
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint vao, points_vbo, colors_vbo, mvp_ID;
glm::mat4 MVP, View, Projection;

GLfloat points[] = {
	-0.5f,  0.5f,  0.0f,
	0.5f,  0.5f,  0.0f,
	0.5f, -0.5f,  0.0f,
	-0.5f, -0.5f,  0.0f
};

GLfloat colors[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f
};


float scale = 19.0f, inc = -1.0f, angle = 0.0f;

void timer(int); // Prototype.

void init(void)
{
	srand((unsigned)time(NULL));
	// Create shader program executable.
	vertexShaderId = setShader((char*)"vertex", (char*)"projection.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cube.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	mvp_ID = glGetUniformLocation(program, "MVP");

	// Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	// Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	Projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(0, 0, 1.0f), // Camera is at (0,0,1), in World Space
		glm::vec3(0, 0, 0),	 // and looks at the origin
		glm::vec3(0, 1, 0)    // Head is up (set to 0,-1,0 to look upside-down)
	);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), points, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
			glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), colors, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	timer(0);
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(float scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale));
	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
}

//---------------------------------------------------------------------
//
// recolorSquare
//
void recolorSquare(void)
{
	float r = (float)rand() / (float)RAND_MAX;
	float g = (float)rand() / (float)RAND_MAX;
	float b = (float)rand() / (float)RAND_MAX;

	float colors[] = {
		r, g, b,
		r, g, b,
		r, g, b,
		r, g, b
	};
	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//---------------------------------------------------------------------
//
// display
//

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(vao);

		transformObject(scale, Z_AXIS, angle, glm::vec3(0.0f, 0.0f, 0.0f));

		if ((scale += inc) <= 1.0f) inc = 1.0f;
		else if (scale >= 19.0f) inc = -1.0f;

		recolorSquare();

		//Ordering the GPU to start the pipeline
		glDrawArrays(GL_QUADS, 0, 4);

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glFlush(); // Instead of double buffering.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void timer(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timer, 0);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

//---------------------------------------------------------------------
//
// main
//

int main(int argc, char** argv)
{
	//Before we can open a window, theremust be interaction between the windowing systemand OpenGL.In GLUT, this interaction is initiated by the following function call :
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);

	//if you comment out this line, a window is created with a default size
	glutInitWindowSize(512, 512);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("03_Projection");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	//If there are events in the queue, our program responds to them through functions
	//called callbacks.A callback function is associated with a specific type of event.
	//A display callback is generated when the application programm or the
	//operating system determines that the graphics in a window need to be redrawn.
	glutDisplayFunc(display); // Output.
	//glutIdleFunc(idle);
	
	glutKeyboardFunc(keyInput); // Input.

	//begin an event-processing loop
	glutMainLoop();
}