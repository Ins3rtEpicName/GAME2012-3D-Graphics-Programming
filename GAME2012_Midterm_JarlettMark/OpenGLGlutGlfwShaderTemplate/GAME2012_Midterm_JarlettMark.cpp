//***************************************************************************
// GAME2012_Midterm_JarlettMark.cpp by Mark Jarlett - ID: 101427705
//
// 3D Graphics Programming Midterm
//
// Description:
//	Small 12x12 maze made for GAME2012 - 3D Graphics Programming Midterm. Uses cubes to draw items that make up the objects in the world.
//	Tried to make a function that would accept a colour (GLfloat myColour[]) parameter but apparently sizeof doesnt see the actual size, it said
//	it was size 4 or 8 instead of 24 or 96 (if i remember correctly). Other than that I hope the way I made squares with cubes is ok! Thank you for the
//	zoom help video/stream, hints and extension on the midterm and A2 dates. Appreciate it and hope you have/had a good intersession week!
//*****************************************************************************

#include <vector>
using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include <string>
#include <iostream>

#define BUFFER_OFFSET(x)  ((const void*) (x))
#define FPS 60
#define MOVESPEED 0.1f
#define TURNSPEED 0.05f
#define X_AXIS glm::vec3(1,0,0)
#define Y_AXIS glm::vec3(0,1,0)
#define Z_AXIS glm::vec3(0,0,1)
#define XY_AXIS glm::vec3(1,0.9,0)
#define YZ_AXIS glm::vec3(0,1,1)
#define XZ_AXIS glm::vec3(1,0,1)
#define XYZ_AXIS glm::vec3(1.0,1.0,1.0) 
#define SPEED 0.25f

enum keyMasks {
	KEY_FORWARD =		0b00000001,		// 0x01 or   1	or   01
	KEY_BACKWARD =		0b00000010,		// 0x02 or   2	or   02
	//KEY_MOUSECLICKED =	0b00000100,		// 0x04 or	 4  or   04
	// Any other keys you want to add. 
	KEY_LEFT =          0b00001000,		// 0x08 or   8  or	 08
	KEY_RIGHT =         0b00010000,		// 0x16 or   16 
	KEY_R =				0b00100000,		// 0x32 or	 32 
	KEY_F =				0b01000000,		// 0x64 or   64
	KEY_Q =				0b10000000,		// 128
	KEY_E =				0b00000100		// 4
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint vao, ibo, points_vbo, colors_vbo, mvp_ID;
glm::mat4 MVP, View, Projection;
// Our bitflag variable. 1 byte for up to 8 key states.
unsigned char keys = 0; // Initialized to 0 or 0b00000000.

//GLfloat customColour[24];

GLshort cube_indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	7, 4, 0, 3,
	// Bottom.
	0, 4, 5, 1,
	// Right.
	2, 1, 5, 6,
	// Back.
	5, 4, 7, 6,
	// Top.
	2, 6, 7, 3
};

GLfloat cube_vertices[] = {
	 0.0f, 0.0f, 0.0f,		// 0.
	 1.0f, 0.0f, 0.0f,		// 1.
	 1.0f, 1.0f, 0.0f,		// 2.
	 0.0f, 1.0f, 0.0f,		// 3.
	 0.0f, 0.0f, -1.0f,		// 4.
	 1.0f, 0.0f, -1.0f,		// 5.
	 1.0f, 1.0f, -1.0f,		// 6.
	 0.0f, 1.0f, -1.0f,		// 7.
};

GLfloat cube_colors[] = {
	0.0f, 0.0f, 1.0f,		// 0.
	0.0f, 0.0f, 1.0f,		// 1.
	0.0f, 0.0f, 1.0f,		// 2.
	0.0f, 0.0f, 1.0f,		// 3.
	0.0f, 0.0f, 1.0f,		// 4.
	0.0f, 0.0f, 1.0f,		// 5.
	0.0f, 0.0f, 1.0f,		// 6.
	0.0f, 0.0f, 1.0f,		// 7.
};

GLfloat door_colors[] = {
	1.0f, 0.52f, 0.72f,		// 0.
	1.0f, 0.52f, 0.72f,		// 1.
	1.0f, 0.52f, 0.72f,		// 2.
	1.0f, 0.52f, 0.72f,		// 3.
	1.0f, 0.52f, 0.72f,		// 4.
	1.0f, 0.52f, 0.72f,		// 5.
	1.0f, 0.52f, 0.72f,		// 6.
	1.0f, 0.52f, 0.72f,		// 7.
};

GLshort cubeInRoom_indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	4, 5, 6, 7,			// 7, 4, 0, 3,
	// Bottom.
	8, 9, 10, 11,		// 0, 4, 5, 1,
	// Right.
	12, 13, 14, 15,		// 2, 1, 5, 6,
	// Back.
	16, 17, 18, 19,		// 5, 4, 7, 6,
	// Top.
	20, 21, 22, 23		// 2, 6, 7, 3
};

GLfloat cubeInRoom_vertices[] = {
	0.0f, 0.0f, 1.0f,		// 0.
	1.0f, 0.0f, 1.0f,		// 1.
	1.0f, 1.0f, 1.0f,		// 2.
	0.0f, 1.0f, 1.0f,		// 3.

	0.0f, 1.0f, 0.0f,		// 4. (copy of old 7)
	0.0f, 0.0f, 0.0f,		// 5. (copy of old 4)
	0.0f, 0.0f, 1.0f,		// 6. (copy of old 0)
	0.0f, 1.0f, 1.0f,		// 7. (copy of old 3)

	0.0f, 0.0f, 1.0f,		// 8. (copy of old 0)
	0.0f, 0.0f, 0.0f,		 // 9. (copy of old 4)
	1.0f, 0.0f, 0.0f,		// 10. (copy of old 5)
	1.0f, 0.0f, 1.0f,		// 11. (copy of old 1)

	1.0f, 1.0f, 1.0f,		// 12. (copy of old 2)
	1.0f, 0.0f, 1.0f,		// 13. (copy of old 1)
	1.0f, 0.0f, 0.0f,		// 14. (copy of old 5)
	1.0f, 1.0f, 0.0f,		// 15. (copy of old 6)

	1.0f, 0.0f, 0.0f,		// 16. (copy of old 5)
	0.0f, 0.0f, 0.0f,       // 17. (copy of old 4)
	0.0f, 1.0f, 0.0f,		// 18. (copy of old 7)
	1.0f, 1.0f, 0.0f,		// 19. (copy of old 6)

	1.0f, 1.0f, 1.0f,		// 20. (copy of old 2)
	1.0f, 1.0f, 0.0f,		// 21. (copy of old 6)
	0.0f, 1.0f, 0.0f,		// 22. (copy of old 7)
	0.0f, 1.0f, 1.0f		// 23. (copy of old 3)
};

// Colours for the 3 cubes in the 4x4 room
GLfloat redCube_inRoom_colors[] = {
	0.0f, 0.0f, 0.0f,		// 0.
	0.0f, 0.0f, 0.0f,		// 1.
	0.0f, 0.0f, 0.0f,		// 2.
	0.0f, 0.0f, 0.0f,		// 3.
		  		
	1.0f, 0.0f, 0.0f,		// 4.
	1.0f, 0.0f, 0.0f,		// 5.
	1.0f, 0.0f, 0.0f,		// 6.
	1.0f, 0.0f, 0.0f,		// 7.
		  		
	1.0f, 0.0f, 0.0f,		// 8.
	1.0f, 0.0f, 0.0f,		// 9.
	1.0f, 0.0f, 0.0f,		// 10.
	1.0f, 0.0f, 0.0f,		// 11.
		  		
	1.0f, 0.0f, 0.0f,		// 12.
	1.0f, 0.0f, 0.0f,		// 13.
	1.0f, 0.0f, 0.0f,		// 14.
	1.0f, 0.0f, 0.0f,		// 15.
		  		
	1.0f, 1.0f, 1.0f,		// 16.
	1.0f, 1.0f, 1.0f,		// 17.
	1.0f, 1.0f, 1.0f,		// 18.
	1.0f, 1.0f, 1.0f,		// 19.
		  		
	1.0f, 0.0f, 0.0f,		// 20.
	1.0f, 0.0f, 0.0f,		// 21.
	1.0f, 0.0f, 0.0f,		// 22.
	1.0f, 0.0f, 0.0f		// 23.
};

GLfloat greenCube_inRoom_colors[] = {
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		

	1.0f, 1.0f, 1.0f,		
	1.0f, 1.0f, 1.0f,		
	1.0f, 1.0f, 1.0f,		
	1.0f, 1.0f, 1.0f,		

	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		

	0.0f, 0.0f, 0.0f,		
	0.0f, 0.0f, 0.0f,		
	0.0f, 0.0f, 0.0f,		
	0.0f, 0.0f, 0.0f,		

	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		

	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
	0.0f, 1.0f, 0.0f,		
};

GLfloat blueCube_inRoom_colors[] = {
	0.0f, 0.0f, 1.0f,		// 0.
	0.0f, 0.0f, 1.0f,		// 1.
	0.0f, 0.0f, 1.0f,		// 2.
	0.0f, 0.0f, 1.0f,		// 3.

	0.0f, 0.0f, 1.0f,		// 4.
	0.0f, 0.0f, 1.0f,		// 5.
	0.0f, 0.0f, 1.0f,		// 6.
	0.0f, 0.0f, 1.0f,		// 7.

	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

// Wall colour for maze
GLfloat wall_colour[] = {
	0.18f, 0.083f, 0.29f,		// 0.
	0.18f, 0.083f, 0.29f,		// 1.
	0.18f, 0.083f, 0.29f,		// 2.
	0.18f, 0.083f, 0.29f,		// 3.
	0.18f, 0.083f, 0.29f,		// 4.
	0.18f, 0.083f, 0.29f,		// 5.
	0.18f, 0.083f, 0.29f,		// 6.
	0.18f, 0.083f, 0.29f,		// 7.
};

// grid / floor colours
GLfloat grid_colour1[] = {
	0.60f, 1.0f, 0.95f,		// 0.
	0.60f, 1.0f, 0.95f,		// 1.
	0.60f, 1.0f, 0.95f,		// 2.
	0.60f, 1.0f, 0.95f,		// 3.
	0.60f, 1.0f, 0.95f,		// 4.
	0.60f, 1.0f, 0.95f,		// 5.
	0.60f, 1.0f, 0.95f,		// 6.
	0.60f, 1.0f, 0.95f,		// 7.
};

GLfloat grid_colour2[] = {
	0.60f, 0.55f, 0.9f,		// 0.
	0.60f, 0.55f, 0.9f,		// 1.
	0.60f, 0.55f, 0.9f,		// 2.
	0.60f, 0.55f, 0.9f,		// 3.
	0.60f, 0.55f, 0.9f,		// 4.
	0.60f, 0.55f, 0.9f,		// 5.
	0.60f, 0.55f, 0.9f,		// 6.
	0.60f, 0.55f, 0.9f,		// 7.
};

// Wireframe data.
GLshort wire_indices[] = {
	// Front.
	0, 1, 2, 3,
	// Left.
	7, 4, 0, 3,
	// Bottom.
	0, 4, 5, 1,
	// Right.
	2, 1, 5, 6,
	// Back.
	5, 4, 7, 6,
	// Top.
	2, 6, 7, 3
};

GLfloat wire_vertices[] = {
	-0.9f, -0.9f, 0.9f,		// 0.
	0.9f, -0.9f, 0.9f,		// 1.
	0.9f, 0.9f, 0.9f,		// 2.
	-0.9f, 0.9f, 0.9f,		// 3.
	-0.9f, -0.9f, -0.9f,	// 4.
	0.9f, -0.9f, -0.9f,		// 5.
	0.9f, 0.9f, -0.9f,		// 6.
	-0.9f, 0.9f, -0.9f,		// 7.
};

GLfloat wire_colors[] = {
	1.0f, 1.0f, 1.0f,		// 0.
	1.0f, 1.0f, 1.0f,		// 1.
	1.0f, 1.0f, 1.0f,		// 2.
	1.0f, 1.0f, 1.0f,		// 3.
	1.0f, 1.0f, 1.0f,		// 4.
	1.0f, 1.0f, 1.0f,		// 5.
	1.0f, 1.0f, 1.0f,		// 6.
	1.0f, 1.0f, 1.0f		// 7.
};

float scale = 1.0f, angle = 0.0f;
float angle2, angle3, angle4 = 0.0f; // for cubes in room, my jank/lazy code woooo (sorry)
float scale_x, scale_y, scale_z;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;

// used for controlling yaw with q and e
float xOffset;
float lastX, xpos;

// door stuff
float doorZ = -7.5f;
float doorSpeed = -0.01f;

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(5.0f, 2.0f, 5.0f);
	frontVec = glm::vec3(0.0f, 0.0f, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	yaw = -90.0f;
	// View will now get set only in transformObject
}

void init(void)
{
	srand((unsigned)time(NULL));
	// Create shader program executable.
	vertexShaderId = setShader((char*)"vertex", (char*)"cube.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"cube.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	mvp_ID = glGetUniformLocation(program, "MVP");

	// Projection matrix : 60∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(glm::radians(60.0f), 1.0f / 1.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		ibo = 0;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

		points_vbo = 0;
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
			// glBufferData moved to draw function.
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(0);

		colors_vbo = 0;
		glGenBuffers(1, &colors_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
			// glBufferData moved to draw function.
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Can optionally unbind the buffer to avoid modification.

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.

	glLineWidth(5.0f);

	// Enable depth testing and face culling. 
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE); no culling, only for the 3 cubes in the final room
	glFrontFace(GL_CCW);
	// glCullFace(GL_BACK);

	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}


//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	xOffset = xpos - lastX;
	lastX = xpos;
	xOffset *= TURNSPEED;
	yaw += xOffset;

	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch)); 
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // looking to the forwardVec of camera
		upVec); // Up vector
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
	
	// We must now update the View.
	calculateView();

	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
}

void transformObject(float scale_x, float scale_y, float scale_z, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, glm::vec3(scale_x,scale_y,scale_z));

	// We must now update the View.
	calculateView();

	MVP = Projection * View * Model;
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &MVP[0][0]);
}

// TODO: work on different colour setter function for cubes
//void drawCubeSetColour(std::vector<GLfloat> customColour[])
//{
//	GLsizei size = customColour->size() * sizeof(customColour[0]);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
//	glBufferData(GL_ARRAY_BUFFER, 96, &customColour, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.
//
//	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
//}

//---------------------------------------------------------------------
//
// drawCube
//
void drawCube()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawDoor()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(door_colors), door_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawCubeFloor1()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_colour1), grid_colour1, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawCubeFloor2()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_colour2), grid_colour2, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawCubeWall()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wall_colour), wall_colour, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawRedCubeRoom()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInRoom_indices), cubeInRoom_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeInRoom_vertices), cubeInRoom_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redCube_inRoom_colors), redCube_inRoom_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawGreenCubeRoom()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInRoom_indices), cubeInRoom_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeInRoom_vertices), cubeInRoom_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(greenCube_inRoom_colors), greenCube_inRoom_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

void drawBlueCubeRoom()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeInRoom_indices), cubeInRoom_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeInRoom_vertices), cubeInRoom_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(blueCube_inRoom_colors), blueCube_inRoom_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_SHORT, 0);
}

//---------------------------------------------------------------------
//
// drawWire
//
void drawWire()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wire_indices), wire_indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW); // copying cube vertices

	glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wire_colors), wire_colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the currently open buffer.

	glDrawElements(GL_LINE_LOOP, 24, GL_UNSIGNED_SHORT, 0);
}

void drawGrid() // 12x12 floor
{
	int itr = 0;
	for (int rows = 0; rows < 12; rows++)
	{
		for (int cols = 0; cols < 12; cols++)
		{
			transformObject(scale_x = 1.0f, scale_y = 0.0f, scale_z = 1.0f, XYZ_AXIS, angle, glm::vec3((float)cols, 0.0f, 0.0f - rows));
			if (itr++ % 2 == 0)
			{
				drawCubeFloor1();
				//drawWire();
			}
			else
			{
				drawCubeFloor2();
				//drawWire();
			}
			drawWire();
		}
		itr++;
	}
}

void drawCubeWall(float x, float y ,float z, glm::vec3 axis, float angle2, float posx, float posy, float posz)
{
	float wallx = x;
	float wally = y;
	float wallz = z;
	glm::vec3 wallaxis = axis;
	float a = angle2;
	float pos_x = posx;
	float pos_y = posy;
	float pos_z = posz;
	transformObject(wallx, wally, wallz, wallaxis, a, glm::vec3(pos_x, pos_y, pos_z));
	drawCubeWall();
	drawWire();
}

void drawCubeRed(float x, float y, float z, glm::vec3 axis, float angle2, float posx, float posy, float posz)
{
	float wallx = x;
	float wally = y;
	float wallz = z;
	glm::vec3 wallaxis = axis;
	float a = angle2;
	float pos_x = posx;
	float pos_y = posy;
	float pos_z = posz;
	transformObject(wallx, wally, wallz, wallaxis, a, glm::vec3(pos_x, pos_y, pos_z));
	drawRedCubeRoom();
}

void drawCubeGreen(float x, float y, float z, glm::vec3 axis, float angle2, float posx, float posy, float posz)
{
	float wallx = x;
	float wally = y;
	float wallz = z;
	glm::vec3 wallaxis = axis;
	float a = angle2;
	float pos_x = posx;
	float pos_y = posy;
	float pos_z = posz;
	transformObject(wallx, wally, wallz, wallaxis, a, glm::vec3(pos_x, pos_y, pos_z));
	drawGreenCubeRoom();
}

void drawCubeBlue(float x, float y, float z, glm::vec3 axis, float angle2, float posx, float posy, float posz)
{
	float wallx = x;
	float wally = y;
	float wallz = z;
	glm::vec3 wallaxis = axis;
	float a = angle2;
	float pos_x = posx;
	float pos_y = posy;
	float pos_z = posz;
	transformObject(wallx, wally, wallz, wallaxis, a, glm::vec3(pos_x, pos_y, pos_z));
	drawBlueCubeRoom();
}

//void drawCubeCustom(std::vector<GLfloat> setcolour[], float x, float y, float z, glm::vec3 axis, float angle3, float posx, float posy, float posz)
//{
//	float wallx = x;
//	float wally = y;
//	float wallz = z;
//	glm::vec3 wallaxis = axis;
//	float a = angle3;
//	float pos_x = posx;
//	float pos_y = posy;
//	float pos_z = posz;
//	drawCubeSetColour(setcolour);
//	transformObject(wallx, wally, wallz, wallaxis, a, glm::vec3(pos_x, pos_y, pos_z));
//	drawWire();
//}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	
	glClearColor(0.0, 0.0, 0.0, 1.0); // this is for the background colour
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindVertexArray(vao);

	// draw grid, 
	// TODO: add alternate colours for grid
	drawGrid();

	// draw walls
		// initial hallway
	drawCubeWall(0.0f, 4.0f, 3.0f, Y_AXIS, 0.0f, 4.0f, 0.0f, 0.0f); // Left
	drawCubeWall(0.0f, 4.0f, 5.0f, Y_AXIS, 0.0f, 6.0f, 0.0f, 0.0f); // Right

		// first left turn
	drawCubeWall(0.0f, 4.0f, 3.0f, Y_AXIS, 90.0f, 4.0f, 0.0f, -3.0f); // L
	drawCubeWall(0.0f, 4.0f, 3.0f, Y_AXIS, 90.0f, 6.0f, 0.0f, -5.0f); // R

		// first right turn
	drawCubeWall(0.0f, 4.0f, 6.0f, Y_AXIS, 0.0f, 1.0f, 0.0f, -3.0f); // L
	drawCubeWall(0.0f, 4.0f, 2.0f, Y_AXIS, 0.0f, 3.0f, 0.0f, -5.0f); // R

		// second right turn
	drawCubeWall(0.0f, 4.0f, 6.0f, Y_AXIS, -90.0f, 1.0f, 0.0f, -9.0f); // L
	drawCubeWall(0.0f, 4.0f, 4.0f, Y_AXIS, -90.0f, 3.0f, 0.0f, -7.0f); // R

		// cube room walls
	drawCubeWall(0.0f, 4.0f, 1.5f, Y_AXIS, 0.0f, 7.0f, 0.0f, -6.0f); // L Bottom or South(?)
	drawCubeWall(0.0f, 4.0f, 1.5f, Y_AXIS, 0.0f, 7.0f, 0.0f, -8.5f); // L Top or North(?)
	drawCubeWall(0.0f, 4.0f, 4.0f, Y_AXIS, -90.0f, 7.0f, 0.0f, -10.0f); // Top or north(?)
	drawCubeWall(0.0f, 4.0f, 4.0f, Y_AXIS, -90.0f, 7.0f, 0.0f, -6.0f); // Bottom or south(?)
	drawCubeWall(0.0f, 4.0f, 4.0f, Y_AXIS, 0.0f, 11.0f, 0.0f, -6.0f); // R
	drawCubeWall(0.0f, 3.0f, 1.0f, Y_AXIS, 0.0f, 7.0f, 1.0f, -7.5f); // wall above door

		// grid border walls
	drawCubeWall(0.0f, 4.0f, 12.0f, Y_AXIS, 0.0f, 0.0f, 0.0f, 0.0f); // L
	drawCubeWall(0.0f, 4.0f, 12.0f, Y_AXIS, 0.0f, 12.0f, 0.0f, 0.0f); // R
	drawCubeWall(0.0f, 4.0f, 12.0f, Y_AXIS, -90.0f, 0.0f, 0.0f, -12.0f); // T / N
	drawCubeWall(0.0f, 4.0f, 4.0f, Y_AXIS, -90.0f, 0.0f, 0.0f, 0.0f); // B / L
	drawCubeWall(0.0f, 4.0f, 6.0f, Y_AXIS, -90.0f, 6.0f, 0.0f, 0.0f); // B / R

	// door stuff
	transformObject(0.0f, 1.0f, 1.0f, Y_AXIS, 0.0f, glm::vec3(7.01f, 0.0f, doorZ += doorSpeed));
	drawDoor();
	if (doorZ <= -8.5f)
	{
		doorSpeed = 0.01f;
		transformObject(0.0f, 1.0f, 1.0f, Y_AXIS, 0.0f, { 7.01f, 0.0f, doorZ += doorSpeed});
		drawDoor();
	}
	else if (doorZ >= -7.5f)
	{
		doorSpeed = -0.01f;
		transformObject(0.0f, 1.0f, 1.0f, Y_AXIS, 0.0f, { 7.01f, 0.0f, doorZ += doorSpeed});
		drawDoor();
	}

	// cubes in the 4x4 room, re-enable culling
		// brain boomed
		// drawCubeSetColour(&redCube_inRoom_colors);
		// drawCubeCustom(&redCube_inRoom_colors, 0.5f, 0.5f, 0.5f, X_AXIS, 45.0f, 9.0f, 2.0f, -8.0f);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
		//red cube
	drawCubeRed(0.15f, 0.15f, 0.15f, X_AXIS, angle2 -= 10.0f, 9.0f, 2.0f, -9.0f);
		//green cube
	drawCubeGreen(0.5, 0.5, 0.5, XY_AXIS, angle3 += 2.0f, 9.0f, 2.0f, -8.0f);
		//blue cube
	drawCubeBlue(0.25, 0.25, 0.25, XYZ_AXIS, angle4 -= 5.0f, 9.0f, 2.0f, -7.0f);
	

	glBindVertexArray(0); // Can optionally unbind the vertex array to avoid modification.
	glutSwapBuffers(); // Now for a potentially smoother render.
}

void idle() // Not even called.
{
	glutPostRedisplay();
}

void parseKeys() 
{
	if (keys & KEY_FORWARD)
		position += frontVec * MOVESPEED;
	if (keys & KEY_BACKWARD)
		position -= frontVec * MOVESPEED;
	if (keys & KEY_LEFT)
		position -= rightVec * MOVESPEED;
	if (keys & KEY_RIGHT)
		position += rightVec * MOVESPEED;
	if (keys & KEY_R)
		position += upVec * MOVESPEED;
	if (keys & KEY_F)
		position -= upVec * MOVESPEED;
	if (keys & KEY_Q)
		xpos -= 40.0f;
	if (keys & KEY_E)
		xpos += 40.0f;
}

void timer(int) { // Tick of the frame.
	// Get first timestamp
	int start = glutGet(GLUT_ELAPSED_TIME);
	// Update call
	parseKeys();
	// Display call
	glutPostRedisplay();
	// Calling next tick
	int end = glutGet(GLUT_ELAPSED_TIME);
	glutTimerFunc((1000 / FPS) - (end-start), timer, 0);
}

// Keyboard input processing routine.
void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case 's':
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	case 'a':
		if (!(keys & KEY_LEFT))
			keys |= KEY_LEFT;
		break;
	case 'd':
		if (!(keys & KEY_RIGHT))
			keys |= KEY_RIGHT;
		break;
	case 'r':
		if (!(keys & KEY_R))
			keys |= KEY_R;
		break;
	case 'f':
		if (!(keys & KEY_F))
			keys |= KEY_F;
		break;
	case 'q':
		if (!(keys & KEY_Q))
			keys |= KEY_Q;
		break;
	case 'e':
		if (!(keys & KEY_E))
			keys |= KEY_E;
		break;
	default:
		break;
	}
}

void keyDownSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP: // Up arrow.
		if (!(keys & KEY_FORWARD))
			keys |= KEY_FORWARD; // keys = keys | KEY_FORWARD
		break;
	case GLUT_KEY_DOWN: // Down arrow.
		if (!(keys & KEY_BACKWARD))
			keys |= KEY_BACKWARD;
		break;
	default:
		break;
	}
}

void keyUp(unsigned char key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case 'w':
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD. ~ is bitwise NOT.
		break;
	case 's':
		keys &= ~KEY_BACKWARD;
		break;
	case 'a':
		keys &= ~KEY_LEFT;
		break;
	case 'd':
		keys &= ~KEY_RIGHT;
		break;
	case 'r':
		keys &= ~KEY_R;
		break;
	case 'f':
		keys &= ~KEY_F;
		break;
	case 'q':
		keys &= ~KEY_Q;
		break;
	case 'e':
		keys &= ~KEY_E;
		break;
	case ' ':
		resetView();
		break;
	default:
		break;
	}
}

void keyUpSpec(int key, int x, int y) // x and y is mouse location upon key press.
{
	switch (key)
	{
	case GLUT_KEY_UP:
		keys &= ~KEY_FORWARD; // keys = keys & ~KEY_FORWARD
		break;
	case GLUT_KEY_DOWN:
		keys &= ~KEY_BACKWARD;
		break;
	default:
		break;
	}
}

// Mouse function disabled to use q and e for yaw instead
//void mouseMove(int x, int y)
//{
//	if (keys & KEY_MOUSECLICKED)
//	{
//		pitch += (GLfloat)((y - lastY) * TURNSPEED);
//		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
//		lastY = y;
//		lastX = x;
//	}
//}
//
//void mouseClick(int btn, int state, int x, int y)
//{
//	if (state == 0)
//	{
//		lastX = x;
//		lastY = y;
//		keys |= KEY_MOUSECLICKED; // Flip flag to true
//		glutSetCursor(GLUT_CURSOR_NONE);
//		//cout << "Mouse clicked." << endl;
//	}
//	else
//	{
//		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
//		glutSetCursor(GLUT_CURSOR_INHERIT);
//		//cout << "Mouse released." << endl;
//	}
//}

//---------------------------------------------------------------------
//
// main
//

int main(int argc, char** argv)
{
	//Before we can open a window, theremust be interaction between the windowing systemand OpenGL.In GLUT, this interaction is initiated by the following function call :
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
	glutSetOption(GLUT_MULTISAMPLE, 8);

	//if you comment out this line, a window is created with a default size
	glutInitWindowSize(800, 800);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("GAME2012_Midterm_JarlettMark - 101427705");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	//glutMouseFunc(mouseClick);
	//glutMotionFunc(mouseMove); // Requires click to register.

	glutMainLoop();

	return 0;
}