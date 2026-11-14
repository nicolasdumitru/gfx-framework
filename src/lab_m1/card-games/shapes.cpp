#include "lab_m1/card-games/shapes.h"

#include <cmath>
#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

namespace {
Mesh *CreateFan(const std::string &name, const std::vector<glm::vec3> &outline) {
    std::vector<VertexFormat> vertices = {VertexFormat(glm::vec3(0))};
    std::vector<unsigned int> indices = {0};
    for (const glm::vec3 &point : outline) {
        indices.push_back((unsigned int)vertices.size());
        vertices.push_back(VertexFormat(point));
    }
    indices.push_back(1);

    Mesh *mesh = new Mesh(name);
    mesh->SetDrawMode(GL_TRIANGLE_FAN);
    mesh->InitFromData(vertices, indices);
    return mesh;
}

glm::vec3 OnCircle(float radius, float turns) {
    float angle = (0.25f + turns) * 2 * (float)M_PI;
    return glm::vec3(radius * std::cos(angle), radius * std::sin(angle), 0);
}
} // namespace

Mesh *gwent::CreateSquare(const std::string &name) {
    return CreateFan(name, {glm::vec3(-0.5f, -0.5f, 0), glm::vec3(0.5f, -0.5f, 0),
                            glm::vec3(0.5f, 0.5f, 0), glm::vec3(-0.5f, 0.5f, 0)});
}

Mesh *gwent::CreatePolygon(const std::string &name, int sides) {
    std::vector<glm::vec3> outline;
    for (int i = 0; i < sides; i += 1) {
        outline.push_back(OnCircle(1, (float)i / sides));
    }
    return CreateFan(name, outline);
}

Mesh *gwent::CreateStar(const std::string &name, int points, float innerRadius) {
    std::vector<glm::vec3> outline;
    for (int i = 0; i < 2 * points; i += 1) {
        outline.push_back(OnCircle(i % 2 == 0 ? 1 : innerRadius, (float)i / (2 * points)));
    }
    return CreateFan(name, outline);
}
