/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef UTIL_H
#define UTIL_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <streambuf>
#include <sstream>

// Vertex Attribute Locations
const GLuint vertexLoc=0, indexLoc = 4, normalLoc=1, texCoordLoc=2, colorLoc=3;

// Uniform Bindings Points
const GLuint matricesUniLoc = 1, materialUniLoc = 2;

// The sampler uniform for textured models
// we are assuming a single texture so this will
//always be texture unit 0
const GLuint texUnit = 0;

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile) {
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch(eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader) {
	
	GLuint program = glCreateProgram();
	
	glAttachShader(program, vertexShader);

	glBindAttribLocation(program,vertexLoc,"in_Position");
	glBindAttribLocation(program,colorLoc,"in_Color");
	glBindAttribLocation(program,texCoordLoc,"in_TexCoord");
	glBindAttribLocation(program,normalLoc,"in_Normal");

	glAttachShader(program, fragmentShader);

	glBindAttribLocation(program,vertexLoc,"in_Position");
	glBindAttribLocation(program,colorLoc,"in_Color");
	glBindAttribLocation(program,texCoordLoc,"in_TexCoord");
	glBindAttribLocation(program,normalLoc,"in_Normal");

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}
	return program;

}

inline std::string readTextFile(const char* filename) {
	std::fstream shaderFile(filename,std::ios::in);
	std::string shader;

	std::stringstream buffer;
	buffer << shaderFile.rdbuf();
	shader = buffer.str();

	return buffer.str();
}
/*
namespace pho {

	class WiiButtonState {
	public:
		WiiButtonState();
		bool a,b,power,plus,minus,home,one,two,down,up,left,right;
		void reset();
	};
}
*/

#endif 
