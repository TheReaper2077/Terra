cmake_minimum_required(VERSION 3.0.0)
project(new_project VERSION 0.1.0)
include(CTest)
enable_testing()

file(GLOB TARGET_SRC "./src/main.cpp" "./src/Window.cpp" "./dependencies/glad/src/*.c" "./dependencies/imgui/*.cpp")

add_executable(main ${TARGET_SRC})

#---------------------------------------------------------------------------#	SDL2

# include_directories("./dependencies/SDL2/include")

# add_library(SDL2 STATIC IMPORTED)
# set_target_properties(SDL2 PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/SDL2/lib/libSDL2.dll.a")
# add_library(SDL2main STATIC IMPORTED)
# set_target_properties(SDL2main PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/SDL2/lib/libSDL2main.a")

# add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/dependencies/SDL2/bin/ ${CMAKE_CURRENT_BINARY_DIR})
# target_link_libraries(main PUBLIC mingw32 SDL2main SDL2)

#---------------------------------------------------------------------------#	GLFW

include_directories("./dependencies/GLFW/include")

add_library(glfw3dll STATIC IMPORTED)
set_target_properties(glfw3dll PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/dependencies/GLFW/lib-mingw-w64/libglfw3dll.a")

add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/dependencies/GLFW/lib-mingw-w64/glfw3.dll ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(main PUBLIC gdi32 user32 kernel32 opengl32 glfw3dll)

#---------------------------------------------------------------------------#	GLAD

include_directories("./dependencies/")

#---------------------------------------------------------------------------#	stb_image

include_directories("./dependencies/")

#---------------------------------------------------------------------------#	glm

include_directories("./dependencies/")

#---------------------------------------------------------------------------#

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
