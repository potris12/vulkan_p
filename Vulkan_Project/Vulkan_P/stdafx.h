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
GLM�� ���� ���� �� ���� ���� ����� �⺻������ OpenGL ���� ������ -1.0 ~ 1.0�� ����մϴ�.
GLM_FORCE_DEPTH_ZERO_TO_ONE ���Ǹ� ����Ͽ� Vulkan ������ 0.0���� 1.0���� ����ϵ��� �����ؾ��մϴ�.
*/
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>
#include <chrono>

#include <functional>