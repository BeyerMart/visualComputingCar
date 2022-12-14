#include <cstdlib>
#include <iostream>

#include "mygl/shader.h"
#include "mygl/mesh.h"
#include "mygl/geometry.h"
#include "mygl/camera.h"

using namespace std;

/* translation, scale and color for the ground plane */
namespace groundPlane
{
    const Vector4D color = { 0.9f, 0.9f, 0.9f, 1.0f };
    const Matrix4D scale = Matrix4D::scale(50.0f, 0.0f, 50.0f);
    const Matrix4D trans = Matrix4D::identity();
}

/* translation, scale and color for the scaled cube */
namespace scaledCube
{
    const Matrix4D scale = Matrix4D::scale(2.0f, 2.0f, 2.0f);
    const Matrix4D trans = Matrix4D::translation({ 0.0f, 4.0f, 0.0f });
}

/* translation, scale and color for the car */
namespace scaledCar
{
    const Vector4D brownColor = { 0.396f, 0.263f, 0.129f, 1.0f }; // BASE of the car
    const Vector4D orangeColor = { 1.0f, 0.647f, 0.0f, 1.0f }; // TOP of the car
    const Vector4D blackColor = { 0.05f, 0.05f, 0.05f, 1.0f }; // WHEELS and AXIS of the car

    /* sizes may differ from the one's in the instruction */    
    const Matrix4D scale = Matrix4D::scale(1.0f, 1.0f, 1.0f);
    const Matrix4D trans = Matrix4D::translation({ 0.0f, 1.7f, 0.0f });
    const Matrix4D baseScale = Matrix4D::scale(4.6f, 0.8f, 1.8f);
    const Matrix4D baseTransl = Matrix4D::translation({ 0.0f, 0.0f, 0.0f }); 
    const Matrix4D topScale = Matrix4D::scale(3.6f, 0.8f, 1.8f);
    const Matrix4D topTransl = Matrix4D::translation({ -1.0f, 1.6f, 0.0f }); 
    const Matrix4D spareScale = Matrix4D::scale(0.225f, 0.6f, 0.6f);
    const Matrix4D spareTransl = Matrix4D::translation({ -4.8f, 0.7f, 0.0f });
    const Matrix4D frontAxisScale = Matrix4D::scale(0.2f, 0.2f, 1.9f);
    const Matrix4D frontAxisTransl = Matrix4D::translation({ 3.2f, -0.9f, 0.0f }); 
    const Matrix4D backAxisScale = Matrix4D::scale(0.2f, 0.2f, 1.9f); 
    const Matrix4D backAxisTransl = Matrix4D::translation({ -2.8f, -0.9f, 0.0f }); 
    const Matrix4D wheelScale = Matrix4D::scale(0.6f, 0.6f, 0.225f); 
    const Matrix4D frontLeftWheelTransl = Matrix4D::translation({ 3.2f, -0.9f, -2.0f });
    const Matrix4D frontRightWheelTransl = Matrix4D::translation({ 3.2f, -0.9f, 2.0f });
    const Matrix4D backLeftWheelTransl = Matrix4D::translation({ -2.8f, -0.9f, -2.0f });
    const Matrix4D backRightWheelTransl = Matrix4D::translation({ -2.8f, -0.9f, 2.0f });
    const float carTurningAngle = 0.02;
    const float carDrivingSpeed = 8.33;
    const float carWheelSteeringAngle = M_PI / 7.0f;
}

struct car
{
    Mesh baseMesh;
    Matrix4D baseScalingMatrix;
    Matrix4D baseTranslationMatrix;
    Mesh topMesh;
    Matrix4D topScalingMatrix;
    Matrix4D topTranslationMatrix;
    Mesh spareMesh;
    Matrix4D spareScalingMatrix;
    Matrix4D spareTranslationMatrix;
    Mesh frontAxisMesh;
    Matrix4D frontAxisScalingMatrix;
    Matrix4D frontAxisTranslationMatrix;
    Mesh backAxisMesh;
    Matrix4D backAxisScalingMatrix;
    Matrix4D backAxisTranslationMatrix;

    Matrix4D wheelScalingMatrix;
    Mesh frontLeftWheelMesh;
    Matrix4D frontLeftWheelTranslationMatrix;
    Mesh frontRightWheelMesh;
    Matrix4D frontRightWheelTranslationMatrix;
    Mesh backLeftWheelMesh;
    Matrix4D backLeftWheelTranslationMatrix;
    Mesh backRightWheelMesh;
    Matrix4D backRightWheelTranslationMatrix;
    Matrix4D wheelTransformationMatrix;

    Matrix4D frontAxisTransformationMatrix;

}sCar;

/* struct holding all necessary state variables for scene */
struct
{
    /* camera */
    Camera camera;
    float zoomSpeedMultiplier;
    int cameraMode;

    /* plane mesh and transformation */
    Mesh planeMesh;
    Matrix4D planeModelMatrix;

    /* cube mesh and transformations */
    Mesh cubeMesh;
    Matrix4D cubeScalingMatrix;
    Matrix4D cubeTranslationMatrix;
    Matrix4D cubeTransformationMatrix;
    float cubeSpinRadPerSecond;

    /* car mesh and transformation */
    car carMesh;
    Matrix4D carScalingMatrix;
    Matrix4D carTranslationMatrix;
    Matrix4D carTransformationMatrix;
    float carDrivePerSecond;

    /* shader */
    ShaderProgram shaderColor;
} sScene;

/* struct holding all state variables for input */
struct
{
    bool mouseLeftButtonPressed = false;
    Vector2D mousePressStart;
    bool buttonPressed[7] = { false, false, false, false, false, false, false};
} sInput;

/* calculate how much the car approximately turns per meter travelled for a given turning angle */
float carCalculateTurningAnglePerMeter(float carTurningAngle, float carWheelBase, float carWidth) {
    /* according to https://calculator.academy/turning-radius-calculator/ and https://gamedev.stackexchange.com/questions/50022/typical-maximum-steering-angle-of-a-real-car/50029 */
    float carTurningRadius = carWheelBase / tan(carTurningAngle);
    float carInnerTurningRadius = carTurningRadius - carWidth;
    return 360.0f / (2.0f * carInnerTurningRadius * M_PI);
}

/* GLFW callback function for keyboard events */
void callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /* called on keyboard event */

    /* close window on escape */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    /* make screenshot and save in work directory */
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        screenshotToPNG("screenshot.png");
    }

    /* input for cube control */
    if (key == GLFW_KEY_W)
    {
        sInput.buttonPressed[0] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    if (key == GLFW_KEY_S)
    {
        sInput.buttonPressed[1] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }

    if (key == GLFW_KEY_A)
    {
        sInput.buttonPressed[2] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    if (key == GLFW_KEY_D)
    {
        sInput.buttonPressed[3] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    /* shift for momentary increase of speed */
    if (key == GLFW_KEY_LEFT_SHIFT)
    {
        sInput.buttonPressed[4] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    /* input for camera control */
    if (key == GLFW_KEY_1) {
        sInput.buttonPressed[5] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    if (key == GLFW_KEY_2) {
        sInput.buttonPressed[6] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
}

/* GLFW callback function for mouse position events */
void callbackMousePos(GLFWwindow* window, double x, double y)
{
    /* called on cursor position change */
    if (sInput.mouseLeftButtonPressed && sScene.cameraMode == 1) {
        Vector2D diff = sInput.mousePressStart - Vector2D(x, y);
        cameraUpdateOrbit(sScene.camera, diff, 0.0f);
        sInput.mousePressStart = Vector2D(x, y);
    }
}

/* GLFW callback function for mouse button events */
void callbackMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        sInput.mouseLeftButtonPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

        double x, y;
        glfwGetCursorPos(window, &x, &y);
        sInput.mousePressStart = Vector2D(x, y);
    }
}

/* GLFW callback function for mouse scroll events */
void callbackMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraUpdateOrbit(sScene.camera, { 0, 0 }, sScene.zoomSpeedMultiplier * yoffset);
}

/* GLFW callback function for window resize event */
void callbackWindowResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    sScene.camera.width = width;
    sScene.camera.height = height;
}

/* function to setup and initialize the whole scene */
void sceneInit(float width, float height)
{
    /* initialize camera */
    sScene.camera = cameraCreate(width, height, 75.0f, 0.01f, 500.0f, { 20.0f, 10.0f, 20.0f }, { 0.0f, 0.0f, 0.0f });
    sScene.zoomSpeedMultiplier = 0.05f;
    sScene.cameraMode = 1;

    /* create opengl buffers for mesh */
    sScene.planeMesh = meshCreate(quad::vertexPos, quad::indices, groundPlane::color);
    sScene.cubeMesh = meshCreate(cube::vertices, cube::indices);
    sScene.carMesh.baseMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::brownColor);
    sScene.carMesh.topMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::orangeColor);
    sScene.carMesh.spareMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);
    sScene.carMesh.frontAxisMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);
    sScene.carMesh.backAxisMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);
    sScene.carMesh.frontLeftWheelMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);
    sScene.carMesh.frontRightWheelMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);
    sScene.carMesh.backLeftWheelMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);
    sScene.carMesh.backRightWheelMesh = meshCreate(unitCube::vertexPos, unitCube::indices, scaledCar::blackColor);

    /* setup transformation matrices for objects */
    sScene.planeModelMatrix = groundPlane::trans * groundPlane::scale;

    sScene.cubeScalingMatrix = scaledCube::scale;
    sScene.cubeTranslationMatrix = scaledCube::trans;

    sScene.cubeTransformationMatrix = Matrix4D::identity();

    sScene.cubeSpinRadPerSecond = M_PI / 2.0f;

    sScene.carDrivePerSecond = scaledCar::carDrivingSpeed;

    sScene.carScalingMatrix = scaledCar::scale;
    sScene.carTranslationMatrix = scaledCar::trans;
    sScene.carTransformationMatrix = Matrix4D::identity();

    sScene.carMesh.baseScalingMatrix = scaledCar::baseScale;
    sScene.carMesh.baseTranslationMatrix = scaledCar::baseTransl;
    sScene.carMesh.topScalingMatrix = scaledCar::topScale;
    sScene.carMesh.topTranslationMatrix = scaledCar::topTransl;
    sScene.carMesh.spareScalingMatrix = scaledCar::spareScale;
    sScene.carMesh.spareTranslationMatrix = scaledCar::spareTransl;
    sScene.carMesh.frontAxisScalingMatrix = scaledCar::frontAxisScale;
    sScene.carMesh.frontAxisTranslationMatrix = scaledCar::frontAxisTransl;
    sScene.carMesh.backAxisScalingMatrix = scaledCar::backAxisScale;
    sScene.carMesh.backAxisTranslationMatrix = scaledCar::backAxisTransl;

    sScene.carMesh.wheelScalingMatrix = scaledCar::wheelScale;
    sScene.carMesh.frontLeftWheelTranslationMatrix = scaledCar::frontLeftWheelTransl;
    sScene.carMesh.frontRightWheelTranslationMatrix = scaledCar::frontRightWheelTransl;
    sScene.carMesh.backLeftWheelTranslationMatrix = scaledCar::backLeftWheelTransl;
    sScene.carMesh.backRightWheelTranslationMatrix = scaledCar::backRightWheelTransl;

    sScene.carMesh.wheelTransformationMatrix = Matrix4D::identity();
    sScene.carMesh.frontAxisTransformationMatrix = Matrix4D::identity();

    /* load shader from file */
    sScene.shaderColor = shaderLoad("shader/default.vert", "shader/default.frag");
}

/* function to move and update objects in scene (e.g., rotate cube according to user input) */
void sceneUpdate(float elapsedTime)
{
    /* if 'w' or 's' pressed, cube should rotate around x axis */
    int rotationDirX = 0;
    if (sInput.buttonPressed[1]) { //S
        rotationDirX = -1;
    }
    else if (sInput.buttonPressed[0]) { //W
        rotationDirX = 1;
    }

    /* if 'a' or 'd' pressed, cube should rotate around y axis */
    int rotationDirY = 0;
    if (sInput.buttonPressed[3]) {
        rotationDirY = -1;
    }
    else if (sInput.buttonPressed[2]) {
        rotationDirY = 1;
    }
    /* if shift, then double the speed */
    if (sInput.buttonPressed[4]) {
        sScene.carDrivePerSecond = 2 * scaledCar::carDrivingSpeed;
    }
    else {
        sScene.carDrivePerSecond = scaledCar::carDrivingSpeed;
    }

    /* static cam */
    if (sInput.buttonPressed[5]) {
        sScene.cameraMode = 1;
    }
    /* following camera */
    if (sInput.buttonPressed[6]) {
        sScene.cameraMode = 2;
    }
   
    /* Rotation angle : alpha = deltaX / r // wheel spin */
    float alpha = ((sScene.carDrivePerSecond * elapsedTime) / (sScene.carMesh.wheelScalingMatrix[0][0] / 2)) * -1 * rotationDirX;

    float turningAnglePerMeter = carCalculateTurningAnglePerMeter(scaledCar::carTurningAngle, 3.0f, scaledCar::baseScale[2][2]);

    /* udpate cube transformation matrix to include new rotation if one of the keys was pressed */
    if (rotationDirX != 0 || rotationDirY != 0) {

        //tire rotation while driving
        sScene.carMesh.wheelTransformationMatrix = Matrix4D::rotationZ(alpha) * sScene.carMesh.wheelTransformationMatrix;

        //angle of the tires
        sScene.carMesh.frontAxisTransformationMatrix = Matrix4D::rotationY(scaledCar::carWheelSteeringAngle * rotationDirY);

        //car movement
        float speed = rotationDirX * sScene.carDrivePerSecond * elapsedTime;
        Vector3D forwardVect = Vector3D(speed, 0, 0);
        Vector4D dirVect = sScene.carTransformationMatrix * forwardVect;
        //printf("dirVect: %s\n", toString(dirVect).c_str());
        
        sScene.carTranslationMatrix = sScene.carTranslationMatrix * Matrix4D::translation({ dirVect.x, dirVect.y, dirVect.z });
    }
    /* resetting the tires */
    else {
        sScene.carMesh.frontAxisTransformationMatrix = Matrix4D::identity();
    }
    /* turning */
    if (rotationDirX != 0 && rotationDirY != 0) {
        Matrix4D rotationYMat = Matrix4D::rotationY(turningAnglePerMeter * rotationDirY * rotationDirX * sScene.carDrivePerSecond * elapsedTime);
        sScene.carTransformationMatrix = sScene.carTransformationMatrix * rotationYMat;
    }

    if (sScene.cameraMode == 2) {
        // no idea, why the indizes are messed up (x and y axis shouldn't be [3][0] and [3][1] according to several debug prints)
        sScene.camera.lookAt = { sScene.carTranslationMatrix[3][0], sScene.carTranslationMatrix[3][1], sScene.carTranslationMatrix[3][2] };
    }
}

/* function to draw all objects in the scene */
void sceneDraw()
{
    /* clear framebuffer color */
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*------------ render scene -------------*/
    /* use shader and set the uniforms (names match the ones in the shader) */
    {
        glUseProgram(sScene.shaderColor.id);
        shaderUniform(sScene.shaderColor, "uProj", cameraProjection(sScene.camera));
        shaderUniform(sScene.shaderColor, "uView", cameraView(sScene.camera));

        /* draw ground plane */
        shaderUniform(sScene.shaderColor, "uModel", sScene.planeModelMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", true);
        glBindVertexArray(sScene.planeMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.planeMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw cube, requires to calculate the final model matrix from all transformations 
        shaderUniform(sScene.shaderColor, "uModel", sScene.cubeTranslationMatrix * sScene.cubeTransformationMatrix * sScene.cubeScalingMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.cubeMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.cubeMesh.size_ibo, GL_UNSIGNED_INT, nullptr); */


        /**
        * This section is currently done one by one.
        * This might be more effective to loop through the different car parts in the future.
        */

        /* draw base of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.baseTranslationMatrix * sScene.carMesh.baseScalingMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.baseMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.baseMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw top of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.topTranslationMatrix  * sScene.carMesh.topScalingMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.topMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.topMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw spare tire of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.spareTranslationMatrix  * sScene.carMesh.spareScalingMatrix * sScene.carScalingMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.spareMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.spareMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw front axis of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.frontAxisTranslationMatrix *  sScene.carMesh.frontAxisScalingMatrix * sScene.carScalingMatrix * sScene.carMesh.wheelTransformationMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.frontAxisMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.frontAxisMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw back axis of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.backAxisTranslationMatrix *  sScene.carMesh.backAxisScalingMatrix * sScene.carScalingMatrix * sScene.carMesh.wheelTransformationMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.backAxisMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.backAxisMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw front left wheel of the car */
        shaderUniform(sScene.shaderColor, "uModel",sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.frontLeftWheelTranslationMatrix * sScene.carMesh.frontAxisTransformationMatrix * sScene.carMesh.wheelScalingMatrix * sScene.carScalingMatrix * sScene.carMesh.wheelTransformationMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.frontLeftWheelMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.frontLeftWheelMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw front right wheel of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.frontRightWheelTranslationMatrix * sScene.carMesh.frontAxisTransformationMatrix * sScene.carMesh.wheelScalingMatrix * sScene.carScalingMatrix * sScene.carMesh.wheelTransformationMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.frontRightWheelMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.frontRightWheelMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw back left wheel of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix *  sScene.carMesh.backLeftWheelTranslationMatrix * sScene.carMesh.wheelScalingMatrix * sScene.carScalingMatrix * sScene.carMesh.wheelTransformationMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.backLeftWheelMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.backLeftWheelMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

        /* draw back right wheel of the car */
        shaderUniform(sScene.shaderColor, "uModel", sScene.carTranslationMatrix * sScene.carTransformationMatrix * sScene.carMesh.backRightWheelTranslationMatrix *  sScene.carMesh.wheelScalingMatrix * sScene.carScalingMatrix * sScene.carMesh.wheelTransformationMatrix);
        shaderUniform(sScene.shaderColor, "checkerboard", false);
        glBindVertexArray(sScene.carMesh.backRightWheelMesh.vao);
        glDrawElements(GL_TRIANGLES, sScene.carMesh.backRightWheelMesh.size_ibo, GL_UNSIGNED_INT, nullptr);

    }

    /* cleanup opengl state */
    glBindVertexArray(0);
    glUseProgram(0);
}

int main(int argc, char** argv)
{
    /* create window/context */
    int width = 1280;
    int height = 720;
    GLFWwindow* window = windowCreate("Assignment 1 - Transformations, User Input and Camera", width, height);
    if (!window) { return EXIT_FAILURE; }

    /* set window callbacks */
    glfwSetKeyCallback(window, callbackKey);
    glfwSetCursorPosCallback(window, callbackMousePos);
    glfwSetMouseButtonCallback(window, callbackMouseButton);
    glfwSetScrollCallback(window, callbackMouseScroll);
    glfwSetFramebufferSizeCallback(window, callbackWindowResize);


    /*---------- init opengl stuff ------------*/
    glEnable(GL_DEPTH_TEST);


    /* setup scene */
    sceneInit(width, height);

    /*-------------- main loop ----------------*/
    double timeStamp = glfwGetTime();
    double timeStampNew = 0.0;

    /* loop until user closes window */
    while (!glfwWindowShouldClose(window))
    {
        /* poll and process input and window events */
        glfwPollEvents();

        /* update model matrix of cube */
        timeStampNew = glfwGetTime();
        sceneUpdate(timeStampNew - timeStamp);
        timeStamp = timeStampNew;

        /* draw all objects in the scene */
        sceneDraw();

        /* swap front and back buffer */
        glfwSwapBuffers(window);
    }


    /*-------- cleanup --------*/
    /* delete opengl shader and buffers */
    shaderDelete(sScene.shaderColor);
    meshDelete(sScene.planeMesh);
    meshDelete(sScene.cubeMesh);
    meshDelete(sScene.carMesh.baseMesh);
    meshDelete(sScene.carMesh.topMesh);
    meshDelete(sScene.carMesh.spareMesh);
    meshDelete(sScene.carMesh.frontAxisMesh);
    meshDelete(sScene.carMesh.backAxisMesh);
    meshDelete(sScene.carMesh.frontLeftWheelMesh);
    meshDelete(sScene.carMesh.frontRightWheelMesh);
    meshDelete(sScene.carMesh.backLeftWheelMesh);
    meshDelete(sScene.carMesh.backRightWheelMesh);

    /* cleanup glfw/glcontext */
    windowDelete(window);

    return EXIT_SUCCESS;
}