
#pragma once

#include "util/vec4.hpp"

using sa::vec4;

namespace Color {
	static const vec4<float> WHITE = vec4<float>(1.0f, 1.0f, 1.0f, 1.0f);
	static const vec4<float> BLACK = vec4<float>(0,0,0,1);
    static const vec4<float> BLUE = vec4<float>(0,0,1,1);
    static const vec4<float> GREEN = vec4<float>(0,1,0,1);
    static const vec4<float> CYAN = vec4<float>(0,1,1,1);
    static const vec4<float> RED = vec4<float>(1,0,0,1);
    static const vec4<float> DARK_RED = vec4<float>(0.5f,0,0,1);
    static const vec4<float> GREY = vec4<float>(0.5f, 0.5f, 0.5f, 1.0f);
    static const vec4<float> MAGENTA = vec4<float>(1,0,1,1);
    static const vec4<float> YELLOW = vec4<float>(1,1,0,1);
    static const vec4<float> ORANGE = vec4<float>(1, 0.5f, 0.25f, 1);
    static const vec4<float> LIGHT_BLUE = vec4<float>(0.5f, 1.0f, 1.0f, 1);
    static const vec4<float> LIGHT_RED = vec4<float>(1.0f, 0.5f, 0.3f, 1);
    static const vec4<float> BROWN = vec4<float>(0.5f, 0.2f, 0.1f, 1);
    static const vec4<float> GOLDEN = vec4<float>(249/256.0f, 229/256.0f, 0, 1);
    static const vec4<float> DARK_GREY = vec4<float>(0.3f, 0.3f, 0.3f, 1);
    static const vec4<float> DARK_GREEN = vec4<float>(0, 0.4f, 0, 1);
}