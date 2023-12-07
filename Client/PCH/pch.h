#define NOMINMAX

// GLEW and GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// standard libs
#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <execution>
#include <chrono>
#include <stdexcept>
#include <filesystem>
#include <bitset>

#define _USE_MATH_DEFINES
#include <math.h>

// fmod
#include <fmod.hpp>
#include <fmod_common.h>
#include <fmod_errors.h>

// modern json for cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// box2d
#include <box2d/box2d.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// threadpool
#include <BS_thread_pool.hpp>

// mine
#include <Engine/Math/random.h>
#include <Engine/Math/utils.h>