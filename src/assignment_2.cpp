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

    bool keyPressed[Car::CONTROL_COUNT + 1] = { false, false, false, false, true };
} sInput;

Vector3D globalDirectionalLightColorRGB;
Vector3D globalDirectionalLightDirection;

struct SpotLight
{
    Vector3D color;
    Vector3D position;
    Vector3D direction;
};

SpotLight spotLights[4];


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
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        sInput.keyPressed[Car::CONTROL_COUNT] = !sInput.keyPressed[Car::CONTROL_COUNT];
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



    // LIGHTS ON
    if (sInput.keyPressed[Car::CONTROL_COUNT] == true) {
        Vector4D fRotat = sScene.car.rotation * Vector4D(0, -0.5, 1, 1); //front lights rotation with car
        spotLights[0] = {
            Vector3D(0.8,0.8,0.6),
            Vector3D(
                (sScene.car.transformation * Vector3D(0.4, 1.0, 2.0)).x,
                (sScene.car.transformation * Vector3D(0.4, 1.0, 2.0)).y,
                (sScene.car.transformation * Vector3D(0.4, 1.0, 2.0)).z
            ),
            Vector3D(fRotat.x, fRotat.y, fRotat.z)
        };

        spotLights[1] = {
            Vector3D(0.8,0.8,0.6),
            Vector3D(
                (sScene.car.transformation * Vector3D(-0.4, 1.0, 2.0)).x,
                (sScene.car.transformation * Vector3D(-0.4, 1.0, 2.0)).y,
                (sScene.car.transformation * Vector3D(-0.4, 1.0, 2.0)).z
            ),
            Vector3D(fRotat.x, fRotat.y, fRotat.z),

        };
        Vector4D bRotat = sScene.car.rotation * Vector4D(0, -0.3, -1, 1); //back lights rotation with car

        spotLights[2] = {
            Vector3D(0.75,0,0),
            Vector3D(
                (sScene.car.transformation * Vector3D(0.6, 1.0, -1.5)).x,
                (sScene.car.transformation * Vector3D(0.6, 1.0, -1.5)).y,
                (sScene.car.transformation * Vector3D(0.6, 1.0, -1.5)).z
            ),
            Vector3D(bRotat.x, bRotat.y, bRotat.z),
        };

        spotLights[3] = {
            Vector3D(0.75,0,0),
            Vector3D(
                (sScene.car.transformation * Vector3D(-0.6, 1.0, -1.5)).x,
                (sScene.car.transformation * Vector3D(-0.6, 1.0, -1.5)).y,
                (sScene.car.transformation * Vector3D(-0.6, 1.0, -1.5)).z
            ),
            Vector3D(bRotat.x, bRotat.y, bRotat.z),

        };

    }

    // LIGHTS OFF
    if (sInput.keyPressed[Car::CONTROL_COUNT] == false) {
        spotLights[0].color = Vector3D(0, 0, 0);
        spotLights[1].color = Vector3D(0, 0, 0);
        spotLights[2].color = Vector3D(0, 0, 0);
        spotLights[3].color = Vector3D(0, 0, 0);
    }
    
    shaderUniform(sScene.shader, "spotLights[0].position", spotLights[0].position);
    shaderUniform(sScene.shader, "spotLights[0].direction", spotLights[0].direction);
    shaderUniform(sScene.shader, "spotLights[0].cutoffAngle", 0.9f);
    //shaderUniform(sScene.shader, "spotLights[0].ambient", spotLights[0].color);
    //shaderUniform(sScene.shader, "spotLights[0].diffuse", spotLights[0].color);
    //shaderUniform(sScene.shader, "spotLights[0].specular", spotLights[0].color);
    shaderUniform(sScene.shader, "spotLights[0].color", spotLights[0].color);
    shaderUniform(sScene.shader, "spotLights[0].constant", 1.0f);
    shaderUniform(sScene.shader, "spotLights[0].linear", 0.14f);
    shaderUniform(sScene.shader, "spotLights[0].quadratic", 0.07f);

    shaderUniform(sScene.shader, "spotLights[1].position", spotLights[1].position);
    shaderUniform(sScene.shader, "spotLights[1].direction", spotLights[1].direction);
    shaderUniform(sScene.shader, "spotLights[1].cutoffAngle", 0.9f);
    //shaderUniform(sScene.shader, "spotLights[1].ambient", spotLights[1].color);
    //shaderUniform(sScene.shader, "spotLights[1].diffuse", spotLights[1].color);
    //shaderUniform(sScene.shader, "spotLights[1].specular", spotLights[1].color);
    shaderUniform(sScene.shader, "spotLights[1].color", spotLights[1].color);
    shaderUniform(sScene.shader, "spotLights[1].constant", 1.0f);
    shaderUniform(sScene.shader, "spotLights[1].linear", 0.14f);
    shaderUniform(sScene.shader, "spotLights[1].quadratic", 0.07f);

    shaderUniform(sScene.shader, "spotLights[2].position", spotLights[2].position);
    shaderUniform(sScene.shader, "spotLights[2].direction", spotLights[2].direction);
    shaderUniform(sScene.shader, "spotLights[2].cutoffAngle", 0.8f);
    //shaderUniform(sScene.shader, "spotLights[2].ambient", spotLights[2].color);
    //shaderUniform(sScene.shader, "spotLights[2].diffuse", spotLights[2].color);
    //shaderUniform(sScene.shader, "spotLights[2].specular", spotLights[2].color);
    shaderUniform(sScene.shader, "spotLights[2].color", spotLights[2].color);
    shaderUniform(sScene.shader, "spotLights[2].constant", 1.0f);
    shaderUniform(sScene.shader, "spotLights[2].linear", 0.14f);
    shaderUniform(sScene.shader, "spotLights[2].quadratic", 0.07f);

    shaderUniform(sScene.shader, "spotLights[3].position", spotLights[3].position);
    shaderUniform(sScene.shader, "spotLights[3].direction", spotLights[3].direction);
    shaderUniform(sScene.shader, "spotLights[3].cutoffAngle", 0.8f);
    //shaderUniform(sScene.shader, "spotLights[3].ambient", spotLights[3].color);
    //shaderUniform(sScene.shader, "spotLights[3].diffuse", spotLights[3].color);
    //shaderUniform(sScene.shader, "spotLights[3].specular", spotLights[3].color);
    shaderUniform(sScene.shader, "spotLights[3].color", spotLights[3].color);
    shaderUniform(sScene.shader, "spotLights[3].constant", 1.0f);
    shaderUniform(sScene.shader, "spotLights[3].linear", 0.09f);
    shaderUniform(sScene.shader, "spotLights[3].quadratic", 0.032f);

    
    shaderUniform(sScene.shader, "viewPosition", sScene.camera.position);

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
