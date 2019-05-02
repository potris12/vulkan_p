#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <set>
#include <map>
#include <cassert>
#include <algorithm>


typedef __int64 GAD_KEY;

/*
GLM에 의해 생성 된 투시 투영 행렬은 기본적으로 OpenGL 깊이 범위가 -1.0 ~ 1.0을 사용합니다.
GLM_FORCE_DEPTH_ZERO_TO_ONE 정의를 사용하여 Vulkan 범위를 0.0에서 1.0으로 사용하도록 구성해야합니다.
*/
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <chrono>

#include <functional>