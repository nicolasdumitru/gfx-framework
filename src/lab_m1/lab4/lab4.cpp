#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;

    // Sets the resolution of the small viewport
    glm::ivec2 resolution = window->GetResolution();
    miniViewportArea = ViewportArea(50, 50, resolution.x / 5.f, resolution.y / 5.f);
}

void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Lab4::RenderScene() {
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::RotateOX(angularStepOX);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(angularStepOZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
}

void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);

    RenderScene();
    DrawCoordinateSystem();

    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(miniViewportArea.x, miniViewportArea.y, miniViewportArea.width, miniViewportArea.height);

    // TODO(student): render the scene again, in the new viewport
    RenderScene();
    DrawCoordinateSystem();
}

void Lab4::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
    // TODO(student): Add transformation logic
    // WASD are also used by the camera while the right mouse button is held
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
        return;

    const float moveSpeed = 2.0f;
    const float scaleSpeed = 1.0f;
    const float rotateSpeed = 2.0f;

    // Translation of the first cube
    if (window->KeyHold(GLFW_KEY_W)) translateZ -= moveSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_S)) translateZ += moveSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_A)) translateX -= moveSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_D)) translateX += moveSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_R)) translateY += moveSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_F)) translateY -= moveSpeed * deltaTime;

    // Uniform scaling of the second cube
    if (window->KeyHold(GLFW_KEY_1))
    {
        scaleX += scaleSpeed * deltaTime;
        scaleY += scaleSpeed * deltaTime;
        scaleZ += scaleSpeed * deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_2))
    {
        scaleX = glm::max(0.0f, scaleX - scaleSpeed * deltaTime);
        scaleY = glm::max(0.0f, scaleY - scaleSpeed * deltaTime);
        scaleZ = glm::max(0.0f, scaleZ - scaleSpeed * deltaTime);
    }

    // Rotation of the third cube
    if (window->KeyHold(GLFW_KEY_3)) angularStepOX += rotateSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_4)) angularStepOX -= rotateSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_5)) angularStepOY += rotateSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_6)) angularStepOY -= rotateSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_7)) angularStepOZ += rotateSpeed * deltaTime;
    if (window->KeyHold(GLFW_KEY_8)) angularStepOZ -= rotateSpeed * deltaTime;
}


void Lab4::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
    
    // TODO(student): Add viewport movement and scaling logic
    const int moveStep = 10;
    if (key == GLFW_KEY_I) miniViewportArea.y += moveStep;
    if (key == GLFW_KEY_K) miniViewportArea.y -= moveStep;
    if (key == GLFW_KEY_J) miniViewportArea.x -= moveStep;
    if (key == GLFW_KEY_L) miniViewportArea.x += moveStep;

    if (key == GLFW_KEY_U || key == GLFW_KEY_O)
    {
        float factor = (key == GLFW_KEY_O) ? 1.1f : 1 / 1.1f;
        int newWidth = glm::max(1, (int)(miniViewportArea.width * factor));
        int newHeight = glm::max(1, (int)(miniViewportArea.height * factor));
        // Truncation would swallow the whole 10% step below 10 px, so always grow by at least 1 px
        if (key == GLFW_KEY_O)
        {
            newWidth = glm::max(newWidth, miniViewportArea.width + 1);
            newHeight = glm::max(newHeight, miniViewportArea.height + 1);
        }
        miniViewportArea.x -= (newWidth - miniViewportArea.width) / 2;
        miniViewportArea.y -= (newHeight - miniViewportArea.height) / 2;
        miniViewportArea.width = newWidth;
        miniViewportArea.height = newHeight;
    }
}


void Lab4::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
