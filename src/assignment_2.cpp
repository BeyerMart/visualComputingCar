#include <cstdlib>
#include <iostream>

#include "mygl/shader.h"
#include "mygl/model.h"
#include "mygl/camera.h"

#include "car.h"

struct
{
    Camera camera;

    Car car;
    Model modelGround;

    ShaderProgram shader;
} sScene;

struct
{
    bool mouseButtonPressed = false;
    Vector2D mousePressStart;

    bool keyPressed[Car::CONTROL_COUNT] = { false, false, false, false };
} sInput;

Vector3D globalDirectionalLightColorRGB;
Vector3D globalDirectionalLightDirection;

struct PointLight
{
    Vector3D pointLightColor;
    Vector3D position;
};

PointLight pointLights[4];


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /* input for car control */
    if (key == GLFW_KEY_W)
    {
        sInput.keyPressed[Car::FORWARD] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    if (key == GLFW_KEY_S)
    {
        sInput.keyPressed[Car::BACKWARD] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }

    if (key == GLFW_KEY_A)
    {
        sInput.keyPressed[Car::LEFT] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }
    if (key == GLFW_KEY_D)
    {
        sInput.keyPressed[Car::RIGHT] = (action == GLFW_PRESS || action == GLFW_REPEAT);
    }

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

    if (key == GLFW_KEY_0 && action == GLFW_PRESS) //day
    {
        globalDirectionalLightColorRGB = Vector3D(0.9, 0.9, 0.9);
        globalDirectionalLightDirection = Vector3D(1, 1, 0.5);
    }

    if (key == GLFW_KEY_9 && action == GLFW_PRESS) //evening
    {
        globalDirectionalLightColorRGB = Vector3D(1, 0.7, 0.3);
        globalDirectionalLightDirection = Vector3D(1, 1, 0.5);
    }

    if (key == GLFW_KEY_8 && action == GLFW_PRESS) //night
    {
        globalDirectionalLightColorRGB = Vector3D(0.1, 0.1, 0.4);
        globalDirectionalLightDirection = Vector3D(1, 1, 0.5);
    }
}

void mouseCallbackPos(GLFWwindow* window, double x, double y)
{
    if (sInput.mouseButtonPressed)
    {
        Vector2D diff = sInput.mousePressStart - Vector2D(x, y);
        cameraUpdateOrbit(sScene.camera, diff, 0.0f);
        sInput.mousePressStart = Vector2D(x, y);
    }
}

void mouseCallbackButton(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        sInput.mouseButtonPressed = (action == GLFW_PRESS);

        double x, y;
        glfwGetCursorPos(window, &x, &y);
        sInput.mousePressStart = Vector2D(x, y);
    }
}

void windowCallbackScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraUpdateOrbit(sScene.camera, { 0, 0 }, 0.1 * yoffset);
}


void windowCallbackResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    sScene.camera.width = width;
    sScene.camera.height = height;
}

void sceneInit()
{
    /*---------- init opengl stuff ------------*/
    glEnable(GL_DEPTH_TEST);

    sScene.car = carLoad("assets/jeep_lowpoly/jeep.obj");
    sScene.modelGround = modelLoad("assets/ground/ground.obj").front();
    sScene.shader = shaderLoad("shader/default.vert", "shader/color.frag");
    sScene.camera = cameraCreate(1280, 720, to_radians(45.0), 0.01, 100.0, { 12.0, 4.0, 12.0 });

    globalDirectionalLightColorRGB = Vector3D(0.9, 0.9, 0.9);
    globalDirectionalLightDirection = Vector3D(1, 1, 0.5);

    //TODO gescheit machen
    pointLights[0] = { Vector3D(0,0,1), Vector3D(1,1,1) };
    pointLights[1] = { Vector3D(1,1,1), Vector3D(0,1,0) };
    pointLights[2] = { Vector3D(0,1,0), Vector3D(3,3,3) };
    pointLights[3] = { Vector3D(1,0,0), Vector3D(1,0,0) };

}

void sceneUpdate(float dt)
{
    carMove(sScene.car, sInput.keyPressed, dt);
}

void sceneDraw()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* setup camera and model matrices */
    Matrix4D proj = cameraProjection(sScene.camera);
    Matrix4D view = cameraView(sScene.camera);

    glUseProgram(sScene.shader.id);
    shaderUniform(sScene.shader, "uProj", proj);
    shaderUniform(sScene.shader, "uView", view);
    shaderUniform(sScene.shader, "uModel", sScene.car.transformation);

    shaderUniform(sScene.shader, "globalDirectionalLightColorRGB", globalDirectionalLightColorRGB);
    shaderUniform(sScene.shader, "globalDirectionalLightDirection", globalDirectionalLightDirection);
    shaderUniform(sScene.shader, "viewPosition", sScene.camera.position);


    //point light
    //shaderUniform(sScene.shader, "pointLights[0].position", pointLights[0].position);
    //shaderUniform(sScene.shader, "pointLights[0].ambient", pointLights[0].pointLightColor);
    //shaderUniform(sScene.shader, "pointLights[0].diffuse", pointLights[0].pointLightColor);
    //shaderUniform(sScene.shader, "pointLights[0].specular", pointLights[0].pointLightColor);
    //shaderUniform(sScene.shader, "pointLights[0].constant", 1.0f);
    //shaderUniform(sScene.shader, "pointLights[0].linear", 0.14f);
    //shaderUniform(sScene.shader, "pointLights[0].quadratic", 0.07f);


    /* render car */
    for (unsigned int i = 0; i < sScene.car.parts.size(); i++)
    {
        Model& model = sScene.car.parts[i];
        Matrix4D& transform = sScene.car.partTranformation[i];
        glBindVertexArray(model.mesh.vao);

        shaderUniform(sScene.shader, "uModel", sScene.car.transformation * transform);

        for (Material& material : model.material)
        {
            /* set material properties */
            shaderUniform(sScene.shader, "uMaterial.diffuse", material.diffuse);
            shaderUniform(sScene.shader, "uMaterial.ambient", material.ambient);
            shaderUniform(sScene.shader, "uMaterial.specular", material.specular);
            shaderUniform(sScene.shader, "uMaterial.shininess", material.shininess);



            glDrawElements(GL_TRIANGLES, material.indexCount, GL_UNSIGNED_INT, (const void*)(material.indexOffset * sizeof(unsigned int)));
        }
    }

    /* render ground */
    shaderUniform(sScene.shader, "uModel", Matrix4D::identity());
    glBindVertexArray(sScene.modelGround.mesh.vao);

    for (Material& material : sScene.modelGround.material)
    {
        /* set material properties */
        shaderUniform(sScene.shader, "uMaterial.diffuse", material.diffuse);
        shaderUniform(sScene.shader, "uMaterial.ambient", material.ambient);
        shaderUniform(sScene.shader, "uMaterial.specular", material.specular);
        shaderUniform(sScene.shader, "uMaterial.shininess", material.shininess);
        //printf("%s\n", toString(material.ambient).c_str());

        glDrawElements(GL_TRIANGLES, material.indexCount, GL_UNSIGNED_INT, (const void*)(material.indexOffset * sizeof(unsigned int)));
    }

    /* cleanup opengl state */
    glBindVertexArray(0);
    glUseProgram(0);
}

int main(int argc, char** argv)
{
    /*---------- init window ------------*/
    GLFWwindow* window = windowCreate("Assignment 2 - Shading", 1280, 720);
    if (!window) { return EXIT_FAILURE; }

    /* set window callbacks */
    glfwSetKeyCallback(window, keyboardCallback);
    glfwSetCursorPosCallback(window, mouseCallbackPos);
    glfwSetMouseButtonCallback(window, mouseCallbackButton);
    glfwSetScrollCallback(window, windowCallbackScroll);
    glfwSetFramebufferSizeCallback(window, windowCallbackResize);

    sceneInit();

    /*-------------- main loop ----------------*/
    double timeStamp = glfwGetTime();
    double timeStampNew = 0.0;
    while (!glfwWindowShouldClose(window))
    {
        /* poll and process input and window events */
        glfwPollEvents();

        sceneUpdate(timeStampNew - timeStamp);

        timeStampNew = glfwGetTime();
        sceneUpdate(timeStampNew - timeStamp);
        timeStamp = timeStampNew;

        sceneDraw();

        glfwSwapBuffers(window);
    }

    /*-------- cleanup --------*/
    carDelete(sScene.car);
    modelDelete(sScene.modelGround);
    shaderDelete(sScene.shader);
    windowDelete(window);

    return EXIT_SUCCESS;
}
