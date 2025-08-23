#pragma once

#include <array>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <plog/Log.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#ifdef USE_OPENGL2_0
#include <imgui_impl_opengl2.h>
#else
#include <imgui_impl_opengl3.h>
#endif
