#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS 
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <functional>
#include <iostream>
#include <vector>
#include <cstring>
#include <map>
#include <set>
#include <fstream>

#ifdef _WIN32
#include<Windows.h>
#endif

#define PRINT(X, ...) { printf(__VA_ARGS__);}

#ifdef _WIN32
/**
* Show Message in a MessageBox, Just for windows OS.
*@param __VA__ARGS__ = Varadic argument for function. Just works if is LPCWSTR.
*/
#define SHOWMESSAGE(...)  \
(\
	MessageBoxW(NULL, __VA_ARGS__) \
)

#define ERRORMESSAGE(...) \
(\
	MessageBoxW(NULL, __VA_ARGS__, MB_ICONERROR| MB_OK) \
)
 
#endif

