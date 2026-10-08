#pragma once

#include <string>

#include "components/simple_scene.h"

namespace m1 {
class Lab1 : public gfxc::SimpleScene {
public:
  Lab1();
  ~Lab1();

  void Init() override;

private:
  void FrameStart() override;
  void Update(float deltaTimeSeconds) override;
  void FrameEnd() override;

  void OnInputUpdate(float deltaTime, int mods) override;
  void OnKeyPress(int key, int mods) override;
  void OnKeyRelease(int key, int mods) override;
  void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
  void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
  void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
  void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
  void OnWindowResize(int width, int height) override;

  float backgroundR = 0.0f;
  float backgroundG = 0.0f;
  float backgroundB = 0.0f;

  const std::string meshNames[3] = {"sphere", "teapot", "box"};
  int selectedMesh = 0;
  glm::vec3 objectPosition = glm::vec3(0, 1.5f, 0);
};
} // namespace m1
