
#pragma once

#include "GL\glew\glew.h"
#include <string>

typedef enum GLSL_ERROR_CODES {
	
	GLSL_OK = 0,

	
	GLSL_VERTEX_SHADER_REQUIRED_ERROR,
	
	GLSL_SHADER_SOURCE_NOT_FOUND,
	GLSL_SHADER_OBJECT_CREATION_ERROR, 
	GLSL_SHADER_COMPILE_ERROR, 

	GLSL_VERTEX_SHADER_SOURCE_NOT_FOUND,
	GLSL_GEOMETRY_SHADER_SOURCE_NOT_FOUND,
	GLSL_FRAGMENT_SHADER_SOURCE_NOT_FOUND,
	
	GLSL_VERTEX_SHADER_OBJECT_CREATION_ERROR,
	GLSL_GEOMETRY_SHADER_OBJECT_CREATION_ERROR,
	GLSL_FRAGMENT_SHADER_OBJECT_CREATION_ERROR,
	
	GLSL_VERTEX_SHADER_COMPILE_ERROR,
	GLSL_GEOMETRY_SHADER_COMPILE_ERROR,
	GLSL_FRAGMENT_SHADER_COMPILE_ERROR,
	
	GLSL_PROGRAM_OBJECT_CREATION_ERROR,
	GLSL_PROGRAM_OBJECT_LINK_ERROR
	
} GLSL_ERROR;


GLuint setupShaders(const std::string& vsPath, const std::string& fsPath, GLSL_ERROR *error_result=NULL); 