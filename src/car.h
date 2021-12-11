#pragma once

#include <mygl/model.h>

struct Car
{
    enum ePart
    {
        LIGHT_BACK = 0,
        AXLE_BACK,
        AXLE_FRONT,
        WHEEL_RESERVE,
        WHEEL_BACK_RIGHT,
        WHEEL_FRONT_RIGHT,
        WHEEL_BACK_LEFT,
        WHEEL_FRONT_LEFT,
        FENDERS,
        GRILLE,
        BODY,
        PART_COUNT
    };

    enum eControl
    {
        LEFT = 0,
        RIGHT,
        FORWARD,
        BACKWARD,
        CONTROL_COUNT
    };

    std::vector<Model> parts;
    std::vector<Matrix4D> partTranformation;

    Matrix4D transformation = Matrix4D::identity();
    Matrix4D rotation = Matrix4D::identity();
    Vector3D translation = Vector3D{0, 0, 0};

    Matrix4D wheelRotation = Matrix4D::identity();
    Matrix4D steeringWheelRotation = Matrix4D::identity();

    float speedMS = 30.0f / 3.6f;
    float turningAngle = to_radians(20.0f);
    float turningDegPerMeter = 0.0f;
};

Car carLoad(const std::string& filepath);
void carDelete(Car& car);
void carMove(Car& car, bool control[Car::CONTROL_COUNT], float elapsedTime);
