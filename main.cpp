#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <filesystem>
#include "EntityManager.h"
#include "Components/Camera.h"
#include "Components/Drawable.h"
#include "Components/Orbit.h"
#include "Components/PointLight.h"
#include "Components/Skybox.h"
#include "Components/Transform.h"
#include "Core/Registry.h"
#include "Rendering/RenderBackend.h"
#include "Systems/CameraSystem.h"
#include "Systems/OribitSystem.h"
#include "Systems/RenderSystem.h"

using std::filesystem::current_path;

static void framebuffer_size(GLFWwindow*, int w, int h){glViewport(0,0,w,h);}
static bool keyDown(GLFWwindow* w, int k) {return glfwGetKey(w, k) == GLFW_PRESS;}

int main()
{
    if (!glfwInit()) return -1;
    std::cout << "[cwd] " << current_path() << "\n";
    std::cout << "[exists sky.png?] " << (std::filesystem::exists("sky.png)") ? "yes" : "no") << "\n";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(1200, 700, "Space Sim", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int w, int h)
    {
        glViewport(0, 0, w, h);
    });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);

    EntityManager EM;
    Registry R;

    CameraSystem camSys(&R);

    auto loadText = [](const char* path)
    {
        std::ifstream f(path);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    };

    R.registerComponent<Skybox>();
    R.registerComponent<Camera>();
    R.registerComponent<Transform>();
    R.registerComponent<Drawable>();
    R.registerComponent<PointLight>();
    R.registerComponent<Orbit>();

    Entity camE = EM.createEntity();
    R.add<Camera>(camE, Camera{});

    // Backend
    RenderBackend backend;

    // ----- Sky Box -------------------------------------------
    std::string sbVsSrc = loadText("shaders/skybox.vert");
    std::string sbFsSrc= loadText("shaders/skybox.frag");
    if (!backend.initSkyBoxProgram(sbVsSrc, sbFsSrc))
    {
        std::fprintf(stderr, "Skybox shader init failed\n");
    }
    backend.createSkyBoxCube();

    std::string path = "assets/images/sky.png";
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    std::array<std::string,6> files =
        {
            path, path, path,
            path, path, path
        };
    int skyTex = backend.createCubeMapFromFile(files, true);
    std::cout << "[skybox] tex id = " << skyTex << "\n";
    Entity sky = EM.createEntity();
    R.add<Skybox>(sky, Skybox{ skyTex });
    // ---------------------------------------------------------------

    std::string vsSrc = loadText("shaders/planet.vert");
    std::string fsSrc = loadText("shaders/planet.frag");
    std::fprintf(stderr, "[SHADERS] VS len=%zu, FS len=%zu\n", vsSrc.size(), fsSrc.size());
    std::fprintf(stderr, "[SHADERS] VS starts: %.40s\n", vsSrc.c_str());
    std::fprintf(stderr, "[SHADERS] FS starts: %.40s\n", fsSrc.c_str());
    if (!backend.init(vsSrc, fsSrc))
    {
        fprintf(stderr, "Shader init failed\n");
    }

    int sphereMesh = backend.createUnitSphere(48, 64);

    // Render System
    RenderSystem renderSys(&R, &backend, 1280, 720);

    // Orbit System
    OrbitSystem orbitSys(&R);

    // Sun Entity
    Entity sun = EM.createEntity();
    R.add<Transform>(sun, Transform{glm::vec3(0), glm::vec3(0), glm::vec3(2.0f)});
    R.add<Drawable>(sun, Drawable{sphereMesh, glm::vec3(1.0f, 0.95f, 0.7f)});
    R.add<PointLight>(sun, PointLight{glm::vec3(1.0f, 0.95f, 0.85f), 200.0f});

    // Planet Entity
    Entity planet = EM.createEntity();
    R.add<Transform>(planet, Transform{glm::vec3(8,0,0), glm::vec3(0), glm::vec3(1.0f)});
    R.add<Drawable>(planet, Drawable{sphereMesh, glm::vec3(0.2f, 0.4f, 1.0f)});
    R.add<Orbit>(planet, Orbit{20, 0.01671022,
               glm::radians(0.00005), glm::radians(-11.26064),
          glm::radians(102.94719), glm::radians(100.46435),
                 1.32712440018e20, 0.0 });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double lastX=0, lastY=0;
    bool firstMouse=true;

    double last = glfwGetTime();
    double acc = 0;
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = float(now - last);
        last = now;
        acc += dt;

        double x,y;
        glfwGetCursorPos(window, &x, &y);
        float dx=0.f, dy=0.f;
        if (firstMouse){lastX=x; lastY=y; firstMouse=false;}
        dx = float(x - lastX);
        dy = float(lastY - y);
        lastX = x;
        lastY = y;

        InputState in{};
        in.W = keyDown(window, GLFW_KEY_W);
        in.S = keyDown(window, GLFW_KEY_S);
        in.D = keyDown(window, GLFW_KEY_D);
        in.A = keyDown(window, GLFW_KEY_A);
        in.Space = keyDown(window, GLFW_KEY_SPACE);
        in.Ctrl = keyDown(window, GLFW_KEY_LEFT_CONTROL);
        if (keyDown(window, GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        in.mouseDx = dx;
        in.mouseDy = dy;
        camSys.setInput(in);

        camSys.update(dt);
        orbitSys.update(dt);

        glClearColor(0.02f, 0.02f, 0.04f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderSys.update(dt);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (acc > 1.0)
        {
            const auto& c = R.get<Camera>(camE);
            std::printf("Cam pos: %.2f %.2f %.2f | yaw=%.2f pitch=%.2f\n",
                        c.pos.x, c.pos.y, c.pos.z, c.yaw, c.pitch);
            acc = 0;
        }
    }
    glfwTerminate();
    return 0;
}