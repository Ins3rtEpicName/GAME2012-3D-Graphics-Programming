#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#define PI 3.14159265358979324
using namespace std;

struct Shape
{
protected:
	vector<GLshort> shape_indices;
	vector<GLfloat> shape_vertices;
	vector<GLfloat> shape_colors;
	vector<GLfloat> shape_uvs;
	vector<GLfloat> shape_normals;
	GLuint vao, ibo, points_vbo, colors_vbo, uv_vbo, normals_vbo;

public:
	~Shape()
	{
		shape_indices.clear();
		shape_indices.shrink_to_fit();
		shape_vertices.clear();
		shape_vertices.shrink_to_fit();
		shape_colors.clear();
		shape_colors.shrink_to_fit();
		shape_uvs.clear();
		shape_uvs.shrink_to_fit();
		shape_normals.clear();
		shape_normals.shrink_to_fit();
	}
	GLsizei NumIndices() 
	{  
		return shape_indices.size();
	}
	void BufferShape() // Gets invoked in Init of program.
	{
		vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
			
			ibo = 0;
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shape_indices[0]) * shape_indices.size(), &shape_indices.front(), GL_STATIC_DRAW);
			
			points_vbo = 0;
			glGenBuffers(1, &points_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(shape_vertices[0]) * shape_vertices.size(), &shape_vertices.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(shape_vertices[0]) * 3, 0);
			glEnableVertexAttribArray(0);

			colors_vbo = 0;
			glGenBuffers(1, &colors_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(shape_colors[0]) * shape_colors.size(), &shape_colors.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			uv_vbo = 0;
			glGenBuffers(1, &uv_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, uv_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(shape_uvs[0]) * shape_uvs.size(), &shape_uvs.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			normals_vbo = 0;
			glGenBuffers(1, &normals_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(shape_normals[0]) * shape_normals.size(), &shape_normals.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(3);

			glBindBuffer(GL_ARRAY_BUFFER, 0); // Close the active buffer.

		glBindVertexArray(0); // Close the active vertex array.
	}
	void RecolorShape(GLfloat r, GLfloat g, GLfloat b)
	{
		ColorShape(r, g, b);
		glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shape_colors[0]) * shape_colors.size(), &shape_colors.front(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional close of the buffer.
	}
	void DrawShape(GLchar c)
	{
		glBindVertexArray(vao);
			glDrawElements(c, this->NumIndices(), GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}
	void CalcAverageNormals(vector<GLshort>& indices, unsigned indiceCount, vector<GLfloat>& vertices, unsigned verticeCount)
	{
		// Set the size of shape_normals, so we can use [].
		shape_normals.resize(verticeCount);
		// Calculate the normals of each triangle first.
		for (unsigned i = 0; i < indiceCount; i += 3)
		{
			unsigned in0 = indices[i] * 3;
			unsigned in1 = indices[i + 1] * 3;
			unsigned in2 = indices[i + 2] * 3;
			glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
			glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
			glm::vec3 normal = glm::normalize(glm::cross(v1, v2)); // Create a normal by using cross product on the vectors then normalizing it.
			// Add the normal to each of the vertices in the triangle, potentially creating a new normal direction if vertex is accessed again.
			shape_normals[in0] += normal.x; shape_normals[in0 + 1] += normal.y; shape_normals[in0 + 2] += normal.z; // First vertex.
			shape_normals[in1] += normal.x; shape_normals[in1 + 1] += normal.y; shape_normals[in1 + 2] += normal.z; // Second vertex.
			shape_normals[in2] += normal.x; shape_normals[in2 + 1] += normal.y; shape_normals[in2 + 2] += normal.z; // Third vertex.
		}
		// Normalize each of the new normal (and possibly averaged) vectors.
		for (unsigned i = 0; i < shape_normals.size(); i += 3)
		{
			glm::vec3 vec(shape_normals[i], shape_normals[i + 1], shape_normals[i + 2]);
			vec = glm::normalize(vec);
			shape_normals[i] = vec.x; shape_normals[i + 1] = vec.y; shape_normals[i + 2] = vec.z;
		}
	}

protected: 
	void ColorShape(GLfloat r, GLfloat g, GLfloat b)
	{
		shape_colors.clear();
		for (int i = 0; i < shape_vertices.size(); i += 3)
		{
			shape_colors.push_back(r);
			shape_colors.push_back(g);
			shape_colors.push_back(b);
		}
		shape_colors.shrink_to_fit();
	}
};

struct Plane : public Shape // Vertical plane of 1x1 units across.
{
	Plane()
	{
		shape_indices = {
			0, 1, 2,
			2, 3, 0
		};
		shape_vertices = {
			0.0f, 0.0f, 0.0f,	// 0.
			1.0f, 0.0f, 0.0f,	// 1.
			1.0f, 1.0f, 0.0f,	// 2.
			0.0f, 1.0f, 0.0f,	// 3.
		};
		for (int i = 0; i < shape_vertices.size(); i += 3)
		{
			shape_uvs.push_back(shape_vertices[i]);
			shape_uvs.push_back(shape_vertices[i + 1]);
		}
		ColorShape(1.0f, 1.0f, 1.0f);
		CalcAverageNormals(shape_indices, shape_indices.size(), shape_vertices, shape_vertices.size());
	}
};

struct Grid : public Shape // Flat grid on ground. Starts at 0,0,0 and increases into positive.
{
	Grid(int rows, int cols)
	{
		for (int row = 0; row <= rows; row++)
		{
			for (int col = 0; col <= cols; col++)
			{
				shape_vertices.push_back(col);
				shape_vertices.push_back(row);
				shape_vertices.push_back(0.0f);
			}
		}
		shape_vertices.shrink_to_fit();
		int i = 0;
		for (int row = 0; row < (rows * (cols + 1)); row = i * (cols + 1))
		{
			if (i % 2 == 0) // Even row. Draw left to right.
			{
				for (int col = 0; col < cols; col++)
				{
					// Triangle one.
					shape_indices.push_back(row + col);
					shape_indices.push_back(row + (col + 1));
					shape_indices.push_back(row + (cols + 1) + (col + 1));
					// Triangle two.
					shape_indices.push_back(row + (cols + 1) + (col + 1));
					shape_indices.push_back(row + (cols + 1) + col);
					shape_indices.push_back(row + col);
				}
			}
			else // Odd row. Draw right to left.
			{
				for (int col = cols - 1; col >= 0; col--)
				{
					// Triangle one.
					shape_indices.push_back(row + col);
					shape_indices.push_back(row + (col + 1));
					shape_indices.push_back(row + (cols + 1) + (col + 1));
					// Triangle two.
					shape_indices.push_back(row + (cols + 1) + (col + 1));
					shape_indices.push_back(row + (cols + 1) + col);
					shape_indices.push_back(row + col);
				}
			}
			i++;
		}
		for (int i = 0; i < shape_vertices.size(); i += 3)
		{																// for texture applied per grid square use commented code instead
			shape_uvs.push_back(shape_vertices[i] / rows);  		// shape_uvs.push_back(shape_vertices[i]);
			shape_uvs.push_back(shape_vertices[i + 1] / rows); 	  	// shape_uvs.push_back(shape_vertices[i + 1]);
		}
		ColorShape(1.0f, 1.0f, 1.0f);
		CalcAverageNormals(shape_indices, shape_indices.size(), shape_vertices, shape_vertices.size());
	}	
};

struct Cube8 : public Shape
{
	Cube8(GLfloat uScale = 1.0f, GLfloat vScale = 1.0f)
	{
		shape_indices = {
			// Front.
			0, 1, 2,
			2, 3, 0,
			// Right.
			1, 5, 6,
			6, 2, 1,
			// Back.
			5, 4, 7,
			7, 6, 5,
			// Left.
			4, 0, 3,
			3, 7, 4,
			// Top.
			7, 3, 2,
			2, 6, 7,
			// Bottom.
			4, 5, 1,
			1, 0, 4
		};
		shape_vertices = {
			0.0f, 0.0f, 1.0f,		// 0.
			1.0f, 0.0f, 1.0f,		// 1.
			1.0f, 1.0f, 1.0f,		// 2.
			0.0f, 1.0f, 1.0f,		// 3.
			0.0f, 0.0f, 0.0f,		// 4.
			1.0f, 0.0f, 0.0f,		// 5.
			1.0f, 1.0f, 0.0f,		// 6.
			0.0f, 1.0f, 0.0f,		// 7.
		};
		shape_uvs = {
			0.0f, 0.0f,				// 0.
			uScale, 0.0f,			// 1.
			uScale, vScale,			// 2.
			0.0f, vScale,			// 3.
			uScale, 0.0f,			// 4.
			0.0f, 0.0f,				// 5.
			0.0f, vScale,			// 6.
			uScale, vScale,			// 7.
		};
		ColorShape(1.0f, 1.0f, 1.0f);
		CalcAverageNormals(shape_indices, shape_indices.size(), shape_vertices, shape_vertices.size());
	}
};

struct Cube24 : public Shape
{
	Cube24(GLfloat xScale = 1.0f, GLfloat yScale = 1.0f, GLfloat zScale = 1.0f)
	{
		shape_indices = {
			// Front.
			0, 1, 2,
			2, 3, 0,
			// Right.
			4, 5, 6,
			6, 7, 4,
			// Back.
			8, 9, 10,
			10, 11, 8,
			// Left.
			12, 13, 14,
			14, 15, 12,
			// Top.
			16, 17, 18,
			18, 19, 16,
			// Bottom.
			20, 21, 22,
			22, 23, 20
		};
		shape_vertices = {
			// Front.
			0.0f, 0.0f, 1.0f,		// 0.
			1.0f, 0.0f, 1.0f,		// 1.
			1.0f, 1.0f, 1.0f,		// 2.
			0.0f, 1.0f, 1.0f,		// 3.
			// Right.
			1.0f, 0.0f, 1.0f,		// 4. Clone of 1.
			1.0f, 0.0f, 0.0f,		// 5.
			1.0f, 1.0f, 0.0f,		// 6.
			1.0f, 1.0f, 1.0f,		// 7. Clone of 2.
			// Back.
			1.0f, 0.0f, 0.0f,		// 8. Clone of 5.
			0.0f, 0.0f, 0.0f,		// 9. Original 4.
			0.0f, 1.0f, 0.0f,		// 10. Original 7.
			1.0f, 1.0f, 0.0f,		// 11. Clone of 6.
			// Left.
			0.0f, 0.0f, 0.0f,		// 12. Original 4.
			0.0f, 0.0f, 1.0f,		// 13. Clone of 0.
			0.0f, 1.0f, 1.0f,		// 14. Clone of 3.
			0.0f, 1.0f, 0.0f,		// 15. Original 7.
			// Top.
			0.0f, 1.0f, 0.0f,		// 16. Original 7.
			0.0f, 1.0f, 1.0f,		// 17. Clone of 3.
			1.0f, 1.0f, 1.0f,		// 18. Clone of 2.
			1.0f, 1.0f, 0.0f,		// 19. Clone of 6.
			// Bottom.
			0.0f, 0.0f, 0.0f,		// 20. Original 4.
			1.0f, 0.0f, 0.0f,		// 21. Clone of 5.
			1.0f, 0.0f, 1.0f,		// 22. Clone of 1.
			0.0f, 0.0f, 1.0f,		// 23. Clone of 0.
		};
		shape_uvs = {
			// Front.
			0.0f, 0.0f,				// 0.
			xScale, 0.0f,			// 1.
			xScale, yScale,			// 2.
			0.0f, yScale,			// 3.
			// Right.		
			0.0f, 0.0f,				// 4. Clone of 1.
			zScale, 0.0f,			// 5.
			zScale, yScale,			// 6.
			0.0f, yScale,			// 7. Clone of 2.
			// Back.		
			0.0f, 0.0f,				// 8. Clone of 5.
			xScale, 0.0f,			// 9. Original 4.
			xScale, yScale,			// 10. Original 7.
			0.0f, yScale,			// 11. Clone of 6.
			// Left.		
			0.0f, 0.0f,				// 12. Original 4.
			zScale, 0.0f,			// 13. Clone of 0.
			zScale, yScale,			// 14. Clone of 3.
			0.0f, yScale,			// 15. Original 7.
			// Top.			
			0.0f, zScale,			// 16. Original 7.
			0.0f, 0.0f,				// 17. Clone of 3.
			xScale, 0.0f,			// 18. Clone of 2.
			xScale, zScale,			// 19. Clone of 6.
			// Bottom.		
			0.0f, 0.0f,				// 20. Original 4.
			xScale, 0.0f,			// 21. Clone of 5.
			xScale, zScale,			// 22. Clone of 1.
			0.0f, zScale,			// 23. Clone of 0.
		};
		ColorShape(1.0f, 1.0f, 1.0f);
		CalcAverageNormals(shape_indices, shape_indices.size(), shape_vertices, shape_vertices.size());
	}
};

struct Prism : public Shape
{
	Prism(int sides, float x = 1.0f, float y = 1.0f)
	{
		float theta = 0.0f;
		// Top face.
		shape_vertices.push_back(0.5f);
		shape_vertices.push_back(1.0f);
		shape_vertices.push_back(0.5f);
		for (int i = 0; i < sides; ++i)
		{
			shape_vertices.push_back(0.5f + 0.5f * cos(theta));
			shape_vertices.push_back(1.0f);
			shape_vertices.push_back(0.5f + 0.5f * sin(theta));
			theta += 2 * PI / sides;
		}
		// Clone of second vertex on top.
		shape_vertices.push_back(0.5f + 0.5f * cos(0.0f));
		shape_vertices.push_back(1.0f);
		shape_vertices.push_back(0.5f + 0.5f * sin(0.0f));
		// Bottom face.
		shape_vertices.push_back(0.5f);
		shape_vertices.push_back(0.0f);
		shape_vertices.push_back(0.5f);
		theta = 0.0f; // Not really necessary.
		for (int i = 0; i < sides; ++i)
		{
			shape_vertices.push_back(0.5f + 0.5f * cos(theta));
			shape_vertices.push_back(0.0f);
			shape_vertices.push_back(0.5f + 0.5f * sin(theta));
			theta += 2 * PI / sides;
		}
		// Clone of second vertex on bottom.
		shape_vertices.push_back(0.5f + 0.5f * cos(0.0f));
		shape_vertices.push_back(0.0f);
		shape_vertices.push_back(0.5f + 0.5f * sin(0.0f));
		// Indices now.
		// Bottom face.
		for (int i = sides + 2; i <= sides * 2 + 1; i++)
		{
			shape_indices.push_back(sides + 2);
			shape_indices.push_back(i + 1);
			shape_indices.push_back(i + 2);
		}
		// Middle faces.
		for (int i = 1; i <= sides; i++)
		{
			// Triangle one.
			shape_indices.push_back(i);
			shape_indices.push_back(i + 1);
			shape_indices.push_back(sides + i + 3);
			// Triangle two.
			shape_indices.push_back(sides + i + 3);
			shape_indices.push_back(sides + i + 2);
			shape_indices.push_back(i);
		}
		// Top face.
		for (int i = 1; i < sides + 1; i++)
		{
			shape_indices.push_back(0);
			shape_indices.push_back(i + 1);
			shape_indices.push_back(i);
		}

		// Update UVs. Top middle.
		shape_uvs.push_back(0.0f);
		shape_uvs.push_back(0.0f);
		for (int i = 0; i <= sides; ++i)
		{
			shape_uvs.push_back((1.0f - (float)i / (float)sides) * x);	// u
			shape_uvs.push_back(y);								// v
		}
		// Bottom middle.
		shape_uvs.push_back(0.5f);
		shape_uvs.push_back(0.5f);
		for (int i = 0; i <= sides; ++i)
		{
			shape_uvs.push_back((1.0f - (float)i / (float)sides) * x);
			shape_uvs.push_back(0.0f);
		}
		ColorShape(1.0f, 1.0f, 1.0f);
		CalcAverageNormals(shape_indices, shape_indices.size(), shape_vertices, shape_vertices.size());
	}
};

#endif