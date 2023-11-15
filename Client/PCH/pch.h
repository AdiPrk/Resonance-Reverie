#define DO_NETWORKING 0
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

// ENet
#if DO_NETWORKING
#include <enet/enet.h>
#endif

// mine
#include <Source/Math/random.h>