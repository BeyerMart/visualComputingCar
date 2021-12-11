#include "car.h"

#include <stdexcept>

Car carLoad(const std::string &filepath)
{
    std::vector<Model> models = modelLoad(filepath);

    if(models.size() != Car::PART_COUNT)
    {
        throw std::runtime_error("[CAR] number of parts do not match!");
    }

    Car car;
    car.parts.resize(models.size());
    car.partTranformation.resize(models.size(), Matrix4D::identity());

    /* re-assign models to match enums (just to be safe ;) (order should actually match the one in the obj file)) */
    for(const auto& obj : models)
    {
        if(obj.name == "LIGHT_BACK") car.parts[Car::LIGHT_BACK] = obj;
        else if(obj.name == "AXLE_BACK") car.parts[Car::AXLE_BACK] = obj;
        else if(obj.name == "AXLE_FRONT") car.parts[Car::AXLE_FRONT] = obj;
        else if(obj.name == "WHEEL_RESERVE") car.parts[Car::WHEEL_RESERVE] = obj;
        else if(obj.name == "WHEEL_BACK_RIGHT") car.parts[Car::WHEEL_BACK_RIGHT] = obj;
        else if(obj.name == "WHEEL_FRONT_RIGHT") car.parts[Car::WHEEL_FRONT_RIGHT] = obj;
        else if(obj.name == "WHEEL_BACK_LEFT") car.parts[Car::WHEEL_BACK_LEFT] = obj;
        else if(obj.name == "WHEEL_FRONT_LEFT") car.parts[Car::WHEEL_FRONT_LEFT] = obj;
        else if(obj.name == "FENDERS") car.parts[Car::FENDERS] = obj;
        else if(obj.name == "GRILLE") car.parts[Car::GRILLE] = obj;
        else if(obj.name == "BODY") car.parts[Car::BODY] = obj;
        else throw std::runtime_error("[CAR] unkown part name");
    }

    /* compute steering angle */
    /* wheel base hardcoded from obj file */
    float carWheelBase = 1.0554f + 1.5296f;
    float carWidth = 0.7737f * 2.0f;

    /* according to https://calculator.academy/turning-radius-calculator/ and https://gamedev.stackexchange.com/questions/50022/typical-maximum-steering-angle-of-a-real-car/50029 */
    float carTurningRadius = carWheelBase / tan(car.turningAngle);
    float carInnerTurningRadius = carTurningRadius - carWidth;
    car.turningDegPerMeter = 360.0f / (2.0f * carInnerTurningRadius * M_PI);

    return car;
}

void carDelete(Car &car)
{
    modelDelete(car.parts);
}

void carMove(Car &car, bool control[], float elapsedTime)
{
    #define RADIUS 0.3f

    float dir = 0.0f;
    dir += control[Car::FORWARD]  ? +1.0f : 0.0f;
    dir += control[Car::BACKWARD] ? -1.0f : 0.0f;

    float turn = 0.0f;
    turn += control[Car::LEFT]  ? +1.0f : 0.0f;
    turn += control[Car::RIGHT] ? -1.0f : 0.0f;

    float deltaMove = car.speedMS * elapsedTime;
    float deltaTurn = to_radians(deltaMove * car.turningDegPerMeter);

    Vector4D trans = car.rotation * Vector4D{0.0, 0.0, dir * deltaMove, 0.0};
    car.translation += Vector3D{trans.x, trans.y, trans.z};
    car.wheelRotation = Matrix4D::rotationX(-dir * deltaMove / RADIUS) * car.wheelRotation;
    car.steeringWheelRotation = Matrix4D::rotationY(car.turningAngle * turn);

    car.rotation = Matrix4D::rotationY(deltaTurn * dir * turn) * car.rotation;
    car.transformation = Matrix4D::translation(car.translation) * car.rotation;

    /* wheels are given with translation in obj file (move wheels to origin for the transformation) */
    #define TRANS_INV(vec, matrix) Matrix4D::translation(vec) * (matrix) * Matrix4D::translation(-vec);
    car.partTranformation[Car::WHEEL_BACK_LEFT] = TRANS_INV(Vector3D(0.7737, 0.44787, -1.0554), car.wheelRotation );
    car.partTranformation[Car::WHEEL_BACK_RIGHT] = TRANS_INV(Vector3D(-0.7737, 0.44787, -1.0554), car.wheelRotation );
    car.partTranformation[Car::WHEEL_FRONT_LEFT] = TRANS_INV(Vector3D(0.7737, 0.44787, 1.5296), car.steeringWheelRotation * car.wheelRotation );
    car.partTranformation[Car::WHEEL_FRONT_RIGHT] = TRANS_INV(Vector3D(-0.7737, 0.44787, 1.5296), car.steeringWheelRotation * car.wheelRotation );
}
