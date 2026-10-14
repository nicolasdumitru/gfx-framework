#include "lab_m1/lab1/lab1.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace m1;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

Lab1::Lab1() {
    // TODO(student): Never forget to initialize class variables!
}

Lab1::~Lab1() {}

void Lab1::Init() {
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh *mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
                       "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
                       "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh *mesh = new Mesh("teapot");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"),
                       "teapot.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
}

void Lab1::FrameStart() {}

void Lab1::Update(float deltaTimeSeconds) {
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    glClearColor(backgroundR, backgroundG, backgroundB, 1.0f);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // Draw the selected mesh at its current position.
    RenderMesh(meshes[meshNames[selectedMesh]], objectPosition, glm::vec3(0.5f));
}

void Lab1::FrameEnd() {
    DrawCoordinateSystem();
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Lab1::OnInputUpdate(float deltaTime, int mods) {
    // Reserve right-mouse + movement keys for the existing camera controls.
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        return;
    }

    const float speed = 2.0f; // World units per second along each axis.
    const float distance = speed * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) objectPosition.z -= distance;
    if (window->KeyHold(GLFW_KEY_S)) objectPosition.z += distance;
    if (window->KeyHold(GLFW_KEY_A)) objectPosition.x -= distance;
    if (window->KeyHold(GLFW_KEY_D)) objectPosition.x += distance;
    if (window->KeyHold(GLFW_KEY_Q)) objectPosition.y -= distance;
    if (window->KeyHold(GLFW_KEY_E)) objectPosition.y += distance;
}

void Lab1::OnKeyPress(int key, int mods) {
    // Add key press event
    if (key == GLFW_KEY_F) {
        // Change the background from black to red.
        backgroundR = 0.7f;
        backgroundG = 0.3f;
        backgroundB = 0.1f;
    }

    if (key == GLFW_KEY_G) {
        // Wrap back to the sphere after displaying the box.
        selectedMesh = (selectedMesh + 1) % 3;
    }
}

void Lab1::OnKeyRelease(int key, int mods) {
    // Add key release event
}

void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    // Add mouse move event
}

void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    // Add mouse button press event
}

void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
    // Add mouse button release event
}

void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
    // Treat mouse scroll event
}

void Lab1::OnWindowResize(int width, int height) {
    // Treat window resize event
}
