
#pragma once

#define __CG_USE_WINDOWS_IMAGING_COMPONENT___		1

#include <glew\glew.h>
#include <Windows.h>
#include <string>

HRESULT initCOM(void);
void shutdownCOM(void);

#ifdef __CG_USE_WINDOWS_IMAGING_COMPONENT___
GLuint wicLoadTexture(const std::wstring& filename);
#endif

GLuint fiLoadTexture(const char *filename);
