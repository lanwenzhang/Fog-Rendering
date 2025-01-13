#include <iostream>
#include <string>
#include <assert.h>
#include "wrapper/checkError.h"

#include "application/application.h"
#include "application/camera/perspectiveCamera.h"
#include "application/camera/gameCameraControl.h"
#include "application/assimpLoader.h"
#include "application/assimpInstanceLoader.h"

#include "glframework/core.h"
#include "glframework/shader.h"
#include "glframework/renderer/renderer.h"
#include "glframework/scene.h"
#include "glframework/mesh/mesh.h"
#include "glframework/geometry.h"
#include "glframework/texture.h"
#include "glframework/material/cubeMaterial.h"
#include "glframework/light/directionalLight.h"


Renderer* renderer = nullptr;
Scene* scene = nullptr;

int WIDTH = 800;
int HEIGHT = 600;

DirectionalLight* dirLight = nullptr;
AmbientLight* ambLight = nullptr;

Camera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;

glm::vec3 clearColor{};

void OnResize(int width, int height) {

    GL_CALL(glViewport(0, 0, width, height));
}

void OnKey(int key, int action, int mods) {

    cameraControl->onKey(key, action, mods);
}

void OnMouse(int button, int action, int mods) {

    double x, y;
    glApp->getCursorPosition(&x, &y);
    cameraControl->onMouse(button, action, x, y);

}

void OnCursor(double xpos, double ypos) {

    cameraControl->onCursor(xpos, ypos);
}

void OnScroll(double offset) {

    cameraControl->onScroll(offset);
}

void prepare() {

    renderer = new Renderer();
    scene= new Scene();

    auto sphereGeo = Geometry::createSphere(1.0f);
    auto sphereMat = new CubeMaterial();
    sphereMat->mDiffuse = new Texture("assets/textures/bk.jpg", 0);
    auto sphereMesh = new Mesh(sphereGeo, sphereMat);
    scene->addChild(sphereMesh);

    auto rock = AssimpLoader::load("assets/grid.obj");
    rock->setScale(glm::vec3(10.0f, 10.0f, 10.0f));
    scene->addChild(rock);

    dirLight = new DirectionalLight();
    dirLight->mDirection = glm::vec3(-1.0f);
    dirLight->mSpecularIntensity = 0.1f;

    ambLight = new AmbientLight();
    ambLight->mColor = glm::vec3(0.1f);
}

void prepareCamera() {

    camera = new PerspectiveCamera(
        60.0f,
        (float)glApp->getWidth() / glApp->getHeight(),
        0.1f,
        1000.0f);

    cameraControl = new GameCameraControl();
    cameraControl->setCamera(camera);
    cameraControl->setSensitivity(0.4f);
    cameraControl->setSpeed(0.02f);
}

int main() {

    // 1 Initial the window
    if (!glApp->init(WIDTH, HEIGHT)) {
        return -1;
    }

    // 2 Size and keyboard callback
    glApp->setResizeCallback(OnResize);
    glApp->setKeyBoardCallback(OnKey);
    glApp->setMouseCallback(OnMouse);
    glApp->setCursorCallback(OnCursor);
    glApp->setScrollCallback(OnScroll);

    // 3 Set openGl rendering viewport and clear canvas color
    GL_CALL(glViewport(0, 0, WIDTH, HEIGHT));
    GL_CALL(glClearColor(0.0f, 0.18f, 0.65f, 1.0f));

    prepareCamera();
    prepare();

    // 4 Set window loop
    while (glApp->update()) {

        cameraControl->update();
        renderer->setClearColor(clearColor);

        renderer->render(scene, camera, dirLight, ambLight);
    }

    glApp->destroy();

    return 0;
}