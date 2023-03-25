#pragma once

class Position {
public:
    Position() = default;

    void set_position(const glm::vec3& new_pos, const glm::vec3& new_angle);

    void move_forward(float dt);
    void move_back(float dt);
    void move_left(float dt);
    void move_right(float dt);
    void move_up(float dt);
    void move_down(float dt);

    void resize_screen(int w, int h);
    void set_mouse_point(int x, int y);

    void update();

public:
    int width = 0, height = 0;

    glm::vec3 position = glm::vec3();
    glm::vec3 forward = glm::vec3();
    glm::vec3 right = glm::vec3();
    glm::vec3 up = glm::vec3();
    glm::vec3 lookat = glm::vec3();
    glm::vec3 angle = glm::vec3();
};
