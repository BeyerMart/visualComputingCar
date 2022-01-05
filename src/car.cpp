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


    for(const auto& obj : models)
    {
        if(obj.name == "SHOCK_ABSORBER_mat_shock") car.parts[Car::SHOCK_ABSORBER] = obj;
        else if(obj.name == "BODY_mat_body") car.parts[Car::BODY] = obj;
        else if(obj.name == "WINDOWS_mat_windows") car.parts[Car::WINDOWS] = obj;
        else if(obj.name == "INSIDE_mat_inside") car.parts[Car::INSIDE] = obj;
        else if(obj.name == "LIGHTS_mat_light") car.parts[Car::LIGHTS] = obj;
        else if(obj.name == "DETAILS_mat_details") car.parts[Car::DETAILS] = obj;
        else if(obj.name == "AXIS_BOTTOM_mat_axis") car.parts[Car::AXIS_BOTTOM] = obj;
        else if(obj.name == "ROOF_mat_roof") car.parts[Car::ROOF] = obj;
        else if(obj.name == "GRILLE_mat_grille") car.parts[Car::GRILLE] = obj;
        else if(obj.name == "WHEEL_SPARE_mat_spare") car.parts[Car::WHEEL_SPARE] = obj;
        else if(obj.name == "WHEEL_BACK_LEFT_mat_wheel_bl") car.parts[Car::WHEEL_BACK_LEFT] = obj;
        else if(obj.name == "WHEEL_FRONT_LEFT_mat_wheel_fl") car.parts[Car::WHEEL_FRONT_LEFT] = obj;
        else if(obj.name == "WHEEL_BACK_RIGHT_mat_wheel_br") car.parts[Car::WHEEL_BACK_RIGHT] = obj;
        else if(obj.name == "WHEEL_FRONT_RIGHT_mat_wheel_fr") car.parts[Car::WHEEL_FRONT_RIGHT] = obj;
        else throw std::runtime_error("[CAR] unkown part name " + obj.name);
    }

    /* compute steering angle */
    /* wheel base hardcoded from obj file */
    float carWheelBase = 1.2166f + 0.94187f;
    float carWidth = 0.77091 + 0.77623;

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
    car.wheelRotation = Matrix4D::rotationX(dir * deltaMove / RADIUS) * car.wheelRotation;
    car.steeringWheelRotation = Matrix4D::rotationY(car.turningAngle * turn);

    car.rotation = Matrix4D::rotationY(deltaTurn * dir * turn) * car.rotation;
    car.transformation = Matrix4D::translation(car.translation) * car.rotation;

    /* wheels are given with translation in obj file (move wheels to origin for the transformation) */
    #define TRANS_INV(vec, matrix) Matrix4D::translation(vec) * (matrix) * Matrix4D::translation(-vec);
    car.partTranformation[Car::WHEEL_BACK_LEFT] = TRANS_INV(Vector3D(0.77091, 0.35002, -1.2166), car.wheelRotation );
    car.partTranformation[Car::WHEEL_BACK_RIGHT] = TRANS_INV(Vector3D(-0.77623, 0.35002, -1.2166), car.wheelRotation );
    car.partTranformation[Car::WHEEL_FRONT_LEFT] = TRANS_INV(Vector3D(0.7705, 0.35002, 0.94187), car.steeringWheelRotation * car.wheelRotation );
    car.partTranformation[Car::WHEEL_FRONT_RIGHT] = TRANS_INV(Vector3D(-0.77623, 0.35002, 0.94187), car.steeringWheelRotation * car.wheelRotation );
}
