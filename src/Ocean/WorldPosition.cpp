#include "stdafx.h"
#include "WorldPosition.h"

constexpr float MotionVel = 100.0f;
constexpr float RotationVel = 0.005f;

void Position::SetPosition(const glm::vec3& new_pos, const glm::vec3& new_angle) {
    position = new_pos;
    angle = new_angle;
    Update();
}

void Position::MoveForward(float dt) {
    position += forward * MotionVel * dt;
}

void Position::MoveBack(float dt) {
    position -= forward * MotionVel * dt;
}

void Position::MoveLeft(float dt) {
    position -= right * MotionVel * dt;
}

void Position::MoveRight(float dt) {
    position += right * MotionVel * dt;
}

void Position::MoveUp(float dt) {
    position.y += MotionVel * dt;
}

void Position::MoveDown(float dt) {
    position.y -= MotionVel * dt;
}

void Position::MouseDown(double x, double y) {
    mouseDragX = x;
    mouseDragY = y;
    mouseDrag = true;
}

void Position::MouseMove(double x, double y) {
    if (!mouseDrag) {
        return;
    }

    angle.x -= static_cast<float>(x - mouseDragX) * RotationVel;
    angle.y -= static_cast<float>(y - mouseDragY) * RotationVel;

    // Boundary Conditions
    if (angle.x<-M_PI) {
        angle.x += M_PI * 2;
    }

    if (angle.x>M_PI) {
        angle.x -= M_PI * 2;
    }

    if (angle.y<-M_PI/2) {
        angle.y = -M_PI / 2;
    }

    if (angle.y>M_PI/2) {
        angle.y = M_PI / 2;
    }

    mouseDragX = x;
    mouseDragY = y;

    Update();
}

void Position::MouseUp() {
    mouseDrag = false;
}

void Position::Update() {
    forward.x = sin(angle.x);
    forward.y = 0.0;
    forward.z = cos(angle.x);

    right.x = -cos(angle.x);
    right.y = 0.0;
    right.z = sin(angle.x);

    lookat.x = sin(angle.x) * cos(angle.y);
    lookat.y = sin(angle.y);
    lookat.z = cos(angle.x) * cos(angle.y);

    up = glm::cross(right, lookat);
}
