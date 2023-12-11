//***************************************************************************
// GAME2012_Final_JarlettMark.cpp by Mark Jarlett 101427705 (C) 2020 All Rights Reserved.
//
// Final Assignment submission.
//
// Description:
// Final Assignment project, total size 40x30.
// 
//
//*****************************************************************************
using namespace std;

#include "stdlib.h"
#include "time.h"
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include "prepShader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include "Shape.h"
#include "Light.h"

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
#define SPEED 0.25f

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum keyMasks {
	KEY_FORWARD =				0b00000001,		// 0x01 or   1	or   01
	KEY_BACKWARD =				0b00000010,		// 0x02 or   2	or   02
	KEY_LEFT =					0b00000100,
	KEY_RIGHT =					0b00001000,
	KEY_UP =					0b00010000,
	KEY_DOWN =					0b00100000,
	KEY_MOUSECLICKED =			0b01000000,

	// Any other keys you want to add.
	// For Point Light movement in Assignment 4
	KEY_I =				0b0000000010000000,
	KEY_J =				0b0000000100000000,
	KEY_K =				0b0000001000000000,
	KEY_L =				0b0000010000000000
};

static unsigned int
program,
vertexShaderId,
fragmentShaderId;

GLuint modelID, viewID, projID;
glm::mat4 View, Projection;

// Our bitflag variable. 1 byte for up to 8 key states.
unsigned short keys = 0; // Initialized to 0 or 0b00000000.

// Texture variables.
GLuint officeFloorID, officeWallID, concreteFloorID, brickID;
GLint width, height, bitDepth;


// Light objects. Now OOP.
AmbientLight aLight(
	glm::vec3(1.0f, 1.0f, 1.0f),		// Diffuse color.
	0.1f);								// Diffuse strength.

DirectionalLight dLight(
	glm::vec3(-1.0f, 0.0f, -0.5f),		// Direction.
	glm::vec3(1.0f, 1.0f, 1.0f),		// Diffuse color.
	0.5f);								// Diffuse strength.

PointLight pLights[1] = {
	{ 
		glm::vec3(5.0f, 3.0f, -5.0f),	// Position.
		20.0f,							// Range.
		1.0f, 4.5f, 75.0f,				// Constant, Linear, Quadratic.   
		glm::vec3(1.0f, 1.0f, 1.0f),	// Diffuse color.
		1.0f							// Diffuse strength.
	}						
	//{ 
	//	glm::vec3(7.5f, 0.5f, -5.5f),	// Position.
	//	5.0f,							// Range.
	//	1.0f, 4.5f, 75.0f,				// Constant, Linear, Quadratic.   
	//	glm::vec3(1.0f, 0.2f, 0.2f),	// Diffuse color.
	//	1.0f							// Diffuse strength.
	//}
};	

//SpotLight sLight(
//	glm::vec3(5.0f, 3.0f, -5.0f),		// Position.
//	glm::vec3(0.1f, 1.0f, 0.1f),		// Diffuse color.
//	1.0f,								// Diffuse strength.
//	glm::vec3(0.0f, -1.0f, 0.25f),		// Direction. Normally opposite because it's used in dot product. See constructor.
//	20.0f);								// Edge.

Material mat = { 1.0f, 32 };

// Camera and transform variables.
float scale = 1.0f, angle = 0.0f;
glm::vec3 position, frontVec, worldUp, upVec, rightVec; // Set by function
GLfloat pitch, yaw;
int lastX, lastY;

// Object data.
	// Floors
Grid g_officeGrid(10, 30);
Grid g_warehouseGrid(30, 30);
	// Walls
Cube24 g_cube24_30x4(30.0f, 4.0f, 0.1f);
Cube24 g_cube24_40x4(40.0f, 4.0f, 0.1f);
Cube24 g_cube24_1x4(1.0f, 4.0f, 0.1f);
Cube24 g_cube24_2x4(2.0f, 4.0f, 0.1f);
Cube24 g_cube24_3x4(3.0f, 4.0f, 0.1f);
Cube24 g_cube24_4x4(4.0f, 4.0f, 0.1f);
Cube24 g_cube24_5x4(5.0f, 4.0f, 0.1f);
Cube24 g_cube24_8x4(8.0f, 4.0f, 0.1f);
Cube24 g_cube24_10x4(10.0f, 4.0f, 0.1f);
Cube24 g_cube24_14x4(14.0f, 4.0f, 0.1f);
//Prism g_prism(20);

void timer(int); // Prototype.

void resetView()
{
	position = glm::vec3(0.0f, 5.0f, 5.0f); // Super pulled back because of grid size.
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
	vertexShaderId = setShader((char*)"vertex", (char*)"../Assets/dat/cube.vert");
	fragmentShaderId = setShader((char*)"fragment", (char*)"../Assets/dat/cube.frag");
	program = glCreateProgram();
	glAttachShader(program, vertexShaderId);
	glAttachShader(program, fragmentShaderId);
	glLinkProgram(program);
	glUseProgram(program);

	modelID = glGetUniformLocation(program, "model");
	viewID = glGetUniformLocation(program, "view");
	projID = glGetUniformLocation(program, "projection");
	
	// Projection matrix : 60∞ Field of View, 1:1 ratio, display range : 0.1 unit <-> 200 units
	Projection = glm::perspective(glm::radians(60.0f), 1.0f / 1.0f, 0.1f, 200.0f);
	// Or, for an ortho camera :
	// Projection = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	resetView();

	// Image loading.
	stbi_set_flip_vertically_on_load(true);

	// Load first image.
	unsigned char* image = stbi_load("../Assets/img/concreteFloor2.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &concreteFloorID);
	glBindTexture(GL_TEXTURE_2D, concreteFloorID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End first image.

	// Load second image.
	image = stbi_load("../Assets/img/officeFloor.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &officeFloorID);
	glBindTexture(GL_TEXTURE_2D, officeFloorID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End second image.

	// Load third image.
	image = stbi_load("../Assets/img/brick.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &brickID);
	glBindTexture(GL_TEXTURE_2D, brickID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);
	// End third image.

	image = stbi_load("../Assets/img/officeWall3.jpg", &width, &height, &bitDepth, 0);
	if (!image) { cout << "Unable to load file!" << endl; }
	glGenTextures(1, &officeWallID);
	glBindTexture(GL_TEXTURE_2D, officeWallID);
	// Note: image types with native transparency will need to be GL_RGBA instead of GL_RGB.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(image);

	glUniform1i(glGetUniformLocation(program, "texture0"), 0);

	// Setting material values.
	glUniform1f(glGetUniformLocation(program, "mat.specularStrength"), mat.specularStrength);
	glUniform1f(glGetUniformLocation(program, "mat.shininess"), mat.shininess);

	// Setting ambient light.
	glUniform3f(glGetUniformLocation(program, "aLight.base.diffuseColor"), aLight.diffuseColor.x, aLight.diffuseColor.y, aLight.diffuseColor.z);
	glUniform1f(glGetUniformLocation(program, "aLight.base.diffuseStrength"), aLight.diffuseStrength);

	// Setting directional light.
	glUniform3f(glGetUniformLocation(program, "dLight.base.diffuseColor"), dLight.diffuseColor.x, dLight.diffuseColor.y, dLight.diffuseColor.z);
	glUniform1f(glGetUniformLocation(program, "dLight.base.diffuseStrength"), dLight.diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "dLight.direction"), dLight.direction.x, dLight.direction.y, dLight.direction.z);

	//// Setting spot light.
	//glUniform3f(glGetUniformLocation(program, "sLight.base.diffuseColor"), sLight.diffuseColor.x, sLight.diffuseColor.y, sLight.diffuseColor.z);
	//glUniform1f(glGetUniformLocation(program, "sLight.base.diffuseStrength"), sLight.diffuseStrength);
	//glUniform3f(glGetUniformLocation(program, "sLight.position"), sLight.position.x, sLight.position.y, sLight.position.z);
	//glUniform3f(glGetUniformLocation(program, "sLight.direction"), sLight.direction.x, sLight.direction.y, sLight.direction.z);
	//glUniform1f(glGetUniformLocation(program, "sLight.edge"), sLight.edgeRad);

	// All VAO/VBO data now in Shape.h! But we still need to invoke BufferShape() this AFTER OpenGL is initialized.
	g_officeGrid.BufferShape();
	g_warehouseGrid.BufferShape();
	g_cube24_30x4.BufferShape();
	g_cube24_40x4.BufferShape();
	g_cube24_1x4.BufferShape();
	g_cube24_2x4.BufferShape();
	g_cube24_3x4.BufferShape();
	g_cube24_4x4.BufferShape();
	g_cube24_5x4.BufferShape();
	g_cube24_8x4.BufferShape();
	g_cube24_10x4.BufferShape();
	g_cube24_14x4.BufferShape();
	//g_prism.BufferShape();

	// Enable depth testing and face culling. 
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	timer(0); // Setup my recursive 'fixed' timestep/framerate.
}

// moved point light init here so that the position (+ more) can be changed for Assignment 4
void SetUpLights() 
{
	// Setting point lights.
	glUniform3f(glGetUniformLocation(program, "pLights[0].base.diffuseColor"), pLights[0].diffuseColor.x, pLights[0].diffuseColor.y, pLights[0].diffuseColor.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[0].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[0].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[0].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[0].quadratic"), pLights[0].quadratic);

	glUniform3f(glGetUniformLocation(program, "pLights[1].base.diffuseColor"), pLights[0].diffuseColor.x, pLights[0].diffuseColor.y, pLights[0].diffuseColor.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].base.diffuseStrength"), pLights[0].diffuseStrength);
	glUniform3f(glGetUniformLocation(program, "pLights[1].position"), pLights[0].position.x, pLights[0].position.y, pLights[0].position.z);
	glUniform1f(glGetUniformLocation(program, "pLights[1].constant"), pLights[0].constant);
	glUniform1f(glGetUniformLocation(program, "pLights[1].linear"), pLights[0].linear);
	glUniform1f(glGetUniformLocation(program, "pLights[1].quadratic"), pLights[0].quadratic);
}

//---------------------------------------------------------------------
//
// calculateView
//
void calculateView()
{
	frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec.y = sin(glm::radians(pitch));
	frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	frontVec = glm::normalize(frontVec);
	rightVec = glm::normalize(glm::cross(frontVec, worldUp));
	upVec = glm::normalize(glm::cross(rightVec, frontVec));

	View = glm::lookAt(
		position, // Camera position
		position + frontVec, // Look target
		upVec); // Up vector
	glUniform3f(glGetUniformLocation(program, "eyePosition"), position.x, position.y, position.z);
}

//---------------------------------------------------------------------
//
// transformModel
//
void transformObject(glm::vec3 scale, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 translation) {
	glm::mat4 Model;
	Model = glm::mat4(1.0f);
	Model = glm::translate(Model, translation);
	Model = glm::rotate(Model, glm::radians(rotationAngle), rotationAxis);
	Model = glm::scale(Model, scale);

	calculateView();

	glUniformMatrix4fv(modelID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(viewID, 1, GL_FALSE, &View[0][0]);
	glUniformMatrix4fv(projID, 1, GL_FALSE, &Projection[0][0]);
}

//---------------------------------------------------------------------
//
// display
//
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(0.751f, 0.667f, 1.0f, 1.0f);
	glClearColor(0.09f, 0.094f, 0.114f, 1.0f);
	SetUpLights();

	// Office Grid/Floor
	glBindTexture(GL_TEXTURE_2D, officeFloorID);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	g_officeGrid.DrawShape(GL_TRIANGLES);

	// Warehouse Grid/Floor
	glBindTexture(GL_TEXTURE_2D, concreteFloorID);
	transformObject(glm::vec3(1.0f, 1.0f, 1.0f), X_AXIS, -90.0f, glm::vec3(0.0f, 0.0f, -10.0f));
	g_warehouseGrid.DrawShape(GL_TRIANGLES);

	// Office Area
		// 1
	glBindTexture(GL_TEXTURE_2D, officeWallID);
	transformObject(glm::vec3(30.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(0.0f, 0.0f, -0.1f));
	g_cube24_30x4.DrawShape(GL_TRIANGLES);
		// 2
	transformObject(glm::vec3(4.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(29.9f, 0.0f, 0.0f));
	g_cube24_4x4.DrawShape(GL_TRIANGLES);
		// 3
	transformObject(glm::vec3(4.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(29.9f, 0.0f, -6.0f));
	g_cube24_4x4.DrawShape(GL_TRIANGLES);
		// 4
	transformObject(glm::vec3(5.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(16.9f, 0.0f, -5.0f));
	g_cube24_5x4.DrawShape(GL_TRIANGLES);
		// 5
	transformObject(glm::vec3(5.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(16.9f, 0.0f, 0.0f));
	g_cube24_5x4.DrawShape(GL_TRIANGLES);
		// 6
	transformObject(glm::vec3(3.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(21.9f, 0.0f, -7.0f));
	g_cube24_3x4.DrawShape(GL_TRIANGLES);
		// 7
	transformObject(glm::vec3(3.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(15.9f, 0.0f, -7.0f));
	g_cube24_3x4.DrawShape(GL_TRIANGLES);
		// 8
	transformObject(glm::vec3(3.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(13.9f, 0.0f, -5.0f));
	g_cube24_3x4.DrawShape(GL_TRIANGLES);
		// 9
	transformObject(glm::vec3(4.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(14.9f, 0.0f, -7.0f));
	g_cube24_4x4.DrawShape(GL_TRIANGLES);
		// 10
	transformObject(glm::vec3(2.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(21.9f, 0.0f, -3.0f));
	g_cube24_2x4.DrawShape(GL_TRIANGLES);
		// 11
	transformObject(glm::vec3(2.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(19.9f, 0.0f, -10.0f));
	g_cube24_2x4.DrawShape(GL_TRIANGLES);
		// 12
	transformObject(glm::vec3(2.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(15.9f, 0.0f, -10.0f));
	g_cube24_2x4.DrawShape(GL_TRIANGLES);
		// 13
	transformObject(glm::vec3(1.1f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(20.9f, 0.0f, -7.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 14
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(21.9f, 0.0f, -0.1f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 15
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(11.9f, 0.0f, -7.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 16
	transformObject(glm::vec3(8.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(11.9f, 0.0f, -2.0f));
	g_cube24_8x4.DrawShape(GL_TRIANGLES);
		// 17	
	transformObject(glm::vec3(4.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(11.9f, 0.0f, -10.0f));
	g_cube24_4x4.DrawShape(GL_TRIANGLES);
		// 18
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(11.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 19
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(11.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 20
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(8.9f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 21
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(8.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 22
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(8.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 23
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(5.9f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 24
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(5.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 25
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(5.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 26
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(2.9f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 27
	transformObject(glm::vec3(1.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(2.0f, 0.0f, -2.0f));
	g_cube24_1x4.DrawShape(GL_TRIANGLES);
		// 28
	transformObject(glm::vec3(8.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(2.0f, 0.0f, -2.0f));
	g_cube24_8x4.DrawShape(GL_TRIANGLES);
		// 29
	transformObject(glm::vec3(10.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(2.0f, 0.0f, -10.0f));
	g_cube24_10x4.DrawShape(GL_TRIANGLES);
		// 30
	transformObject(glm::vec3(8.0f, 4.0f, 0.1f), Y_AXIS, 0.0f, glm::vec3(21.9f, 0.0f, -10.0f));
	g_cube24_8x4.DrawShape(GL_TRIANGLES);
		// 31
	transformObject(glm::vec3(14.0f, 4.0f, 0.1f), Y_AXIS, 90.0f, glm::vec3(0.0f, 0.0f, 0.0f));
	g_cube24_14x4.DrawShape(GL_TRIANGLES);


	
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
	if (keys & KEY_UP)
		position += upVec * MOVESPEED;
	if (keys & KEY_DOWN)
		position -= upVec * MOVESPEED;
	if (keys & KEY_I)
		pLights[0].position.z -= 1.0f * MOVESPEED;
	//* OR for fun...*/ pLights[0].position -= frontVec * MOVESPEED;
	if (keys & KEY_J)
		pLights[0].position.x -= 1.0f * MOVESPEED;
	//* OR for fun...*/ pLights[0].position -= rightVec * MOVESPEED;
	if (keys & KEY_K)
		pLights[0].position.z += 1.0f * MOVESPEED;
	//* OR for fun...*/ pLights[0].position += frontVec * MOVESPEED;
	if (keys & KEY_L)
		pLights[0].position.x += 1.0f * MOVESPEED;
	//* OR for fun...*/ pLights[0].position += rightVec * MOVESPEED;
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
		if (!(keys & KEY_UP))
			keys |= KEY_UP;
		break;
	case 'f':
		if (!(keys & KEY_DOWN))
			keys |= KEY_DOWN;
		break;
	case 'i':
		if (!(keys & KEY_I))
			keys |= KEY_I;
		break;
	case 'j':
		if (!(keys & KEY_J))
			keys |= KEY_J;
		break;
	case 'k':
		if (!(keys & KEY_K))
			keys |= KEY_K;
		break;
	case 'l':
		if (!(keys & KEY_L))
			keys |= KEY_L;
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
		keys &= ~KEY_UP;
		break;
	case 'f':
		keys &= ~KEY_DOWN;
		break;
	case 'i':
		keys &= ~KEY_I;
		break;
	case 'j':
		keys &= ~KEY_J;
		break;
	case 'k':
		keys &= ~KEY_K;
		break;
	case 'l':
		keys &= ~KEY_L;
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

void mouseMove(int x, int y)
{
	if (keys & KEY_MOUSECLICKED)
	{
		pitch += (GLfloat)((y - lastY) * TURNSPEED);
		yaw -= (GLfloat)((x - lastX) * TURNSPEED);
		lastY = y;
		lastX = x;
	}
}

void mouseClick(int btn, int state, int x, int y)
{
	if (state == 0)
	{
		lastX = x;
		lastY = y;
		keys |= KEY_MOUSECLICKED; // Flip flag to true
		glutSetCursor(GLUT_CURSOR_NONE);
		//cout << "Mouse clicked." << endl;
	}
	else
	{
		keys &= ~KEY_MOUSECLICKED; // Reset flag to false
		glutSetCursor(GLUT_CURSOR_INHERIT);
		//cout << "Mouse released." << endl;
	}
}

//---------------------------------------------------------------------
//
// clean
//
void clean()
{
	cout << "Cleaning up!" << endl;
	glDeleteTextures(1, &concreteFloorID);
}

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
	glutInitWindowSize(1000, 1000);

	//the top-left corner of the display
	glutInitWindowPosition(0, 0);

	glutCreateWindow("GAME2012_Final_JarlettMark");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init(); // Our own custom function.

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutSpecialFunc(keyDownSpec);
	glutKeyboardUpFunc(keyUp);
	glutSpecialUpFunc(keyUpSpec);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove); // Requires click to register.

	atexit(clean); // This useful GLUT function calls specified function before exiting program. 
	glutMainLoop();

	return 0;
}