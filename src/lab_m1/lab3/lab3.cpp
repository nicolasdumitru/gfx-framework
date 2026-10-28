#include "lab_m1/lab3/lab3.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "lab_m1/lab3/object2D.h"
#include "lab_m1/lab3/transform2D.h"

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Lab3::Lab3() {}

Lab3::~Lab3() {}

void Lab3::Init() {
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    // Local-space center, used as the pivot for centered rotation and scaling.
    cx = corner.x + squareSide * 0.5f;
    cy = corner.y + squareSide * 0.5f;

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;
    animationTime = 0;

    Mesh *square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh *square2 = object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0));
    AddMeshToList(square2);

    Mesh *square3 = object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1));
    AddMeshToList(square3);
}

void Lab3::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Lab3::Update(float deltaTimeSeconds) {
    animationTime += deltaTimeSeconds;

    // Sine varies smoothly between -1 and 1, reversing at each endpoint.
    translateX = 100.0f * std::sin(animationTime * 1.5f);
    translateY = 0.0f;
    scaleX = scaleY = 1.0f + 0.5f * std::sin(animationTime * 2.0f);
    angularStep = glm::radians(45.0f) * std::sin(animationTime);

    // Red: horizontal movement, 100 units either side of its base position.
    modelMatrix = transform2D::Translate(150, 250) *
                  transform2D::Translate(translateX, translateY);
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    // Green: grow and shrink around its center, from 0.5x to 1.5x.
    modelMatrix = transform2D::Translate(400, 250) *
                  transform2D::Translate(cx, cy) *
                  transform2D::Scale(scaleX, scaleY) *
                  transform2D::Translate(-cx, -cy);
    RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);

    // Blue: swing between -45 and +45 degrees around its center.
    modelMatrix = transform2D::Translate(650, 250) *
                  transform2D::Translate(cx, cy) *
                  transform2D::Rotate(angularStep) *
                  transform2D::Translate(-cx, -cy);
    RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);
}

void Lab3::FrameEnd() {}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Lab3::OnInputUpdate(float deltaTime, int mods) {}

void Lab3::OnKeyPress(int key, int mods) {
    // Add key press event
}

void Lab3::OnKeyRelease(int key, int mods) {
    // Add key release event
}

void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    // Add mouse move event
}

void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    // Add mouse button press event
}

void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    // Add mouse button release event
}

void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {}

void Lab3::OnWindowResize(int width, int height) {}
