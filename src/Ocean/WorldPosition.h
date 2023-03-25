#pragma once

class Position {
public:
    Position() = default;

    void SetPosition(const glm::vec3& new_pos, const glm::vec3& new_angle);

    void MoveForward(float dt);
    void MoveBack(float dt);
    void MoveLeft(float dt);
    void MoveRight(float dt);
    void MoveUp(float dt);
    void MoveDown(float dt);

    void MouseDown(double x, double y);
    void MouseMove(double x, double y);
    void MouseUp();

    void Update();

public:
    int width = 0, height = 0;

    bool mouseDrag = false;
    double mouseDragX = 0, mouseDragY = 0;

    glm::vec3 position = glm::vec3();
    glm::vec3 forward = glm::vec3();
    glm::vec3 right = glm::vec3();
    glm::vec3 up = glm::vec3();
    glm::vec3 lookat = glm::vec3();
    glm::vec3 angle = glm::vec3();
};
