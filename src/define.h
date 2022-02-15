#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

#include <thread>
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
#define WIDTH 1280
#define HEIGHT 640

#define BASIC_VS "D:\\C++\\Ortho Test\\src\\render\\shaders\\basic.vs"
#define BASIC_FS "D:\\C++\\Ortho Test\\src\\render\\shaders\\basic.fs"
#define TEXTURE_VS "D:\\C++\\Ortho Test\\src\\render\\shaders\\texture.vs"
#define TEXTURE_FS "D:\\C++\\Ortho Test\\src\\render\\shaders\\texture.fs"
