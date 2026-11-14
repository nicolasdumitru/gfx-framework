#pragma once

#include <cmath>

#include "utils/glm_utils.h"

// Not in the transform2D namespace: lab3 defines inline functions with the same names there, and
// the linker would be free to pick either definition.
namespace gwent {
inline glm::mat3 Translate(float translateX, float translateY) {
    return glm::mat3(1, 0, 0, 0, 1, 0, translateX, translateY, 1);
}

inline glm::mat3 Scale(float scaleX, float scaleY) {
    return glm::mat3(scaleX, 0, 0, 0, scaleY, 0, 0, 0, 1);
}

inline glm::mat3 Rotate(float radians) {
    float c = std::cos(radians);
    float s = std::sin(radians);
    return glm::mat3(c, s, 0, -s, c, 0, 0, 0, 1);
}
} // namespace gwent
