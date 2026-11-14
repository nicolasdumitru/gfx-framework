#pragma once

#include <string>

#include "core/gpu/mesh.h"

// All meshes are centered on the origin and are meant to be drawn with the "Color" shader.
namespace gwent {
// Side length 1.
Mesh *CreateSquare(const std::string &name);

// Regular polygon with circumradius 1 and one vertex pointing up.
Mesh *CreatePolygon(const std::string &name, int sides);

// Outer radius 1, one point up.
Mesh *CreateStar(const std::string &name, int points, float innerRadius);
} // namespace gwent
