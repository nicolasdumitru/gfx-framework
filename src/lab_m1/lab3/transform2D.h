#pragma once

#include "utils/glm_utils.h"
#include <cmath>

namespace transform2D {
// Translate matrix
inline glm::mat3 Translate(float translateX, float translateY) {
    // TODO(student): Implement the translation matrix
    return glm::mat3(
        // clang-format off
            1, 0, 0, 
            0, 1, 0,
            translateX, translateY, 1
        // clang-format on
    );
}

// Scale matrix
inline glm::mat3 Scale(float scaleX, float scaleY) {
    // TODO(student): Implement the scaling matrix
    return glm::mat3(
        // clang-format off
        scaleX, 0, 0,
        0, scaleY, 0,
        0, 0, 1
        // clang-format on
    );
}

// Rotate matrix
inline glm::mat3 Rotate(float radians) {
    // TODO(student): Implement the rotation matrix
    float c = cos(radians);
    float s = sin(radians);
    return glm::mat3(
        // clang-format off
        c, s, 0,
        -s, c, 0,
        0, 0, 1
        // clang-format on
    );
}
} // namespace transform2D
