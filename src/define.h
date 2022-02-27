#pragma once

#include <queue>
#include <deque>


#include <thread>
#include <glm/gtx/hash.hpp>
#include <string>
#include <bitset>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <utility>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include <array>
#include <fstream>
#include <chrono>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <stb_image.h>

#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_cross_product.hpp>

#define TITLE "STUFF"
#define WIDTH 1280.0f
#define HEIGHT 640.0f

#define BASIC_VS "D:\\C++\\Ortho Test\\src\\render\\shaders\\basic.vs"
#define BASIC_FS "D:\\C++\\Ortho Test\\src\\render\\shaders\\basic.fs"
#define UI_BASIC_VS "D:\\C++\\Ortho Test\\src\\render\\shaders\\ui_basic.vs"
#define UI_BASIC_FS "D:\\C++\\Ortho Test\\src\\render\\shaders\\ui_basic.fs"
#define TEXTURE_VS "D:\\C++\\Ortho Test\\src\\render\\shaders\\texture.vs"
#define TEXTURE_FS "D:\\C++\\Ortho Test\\src\\render\\shaders\\texture.fs"

#define CHUNK_SIZE 16