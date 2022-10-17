#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "handlers/Triangle.h"

bool up_pressed = false;
bool left_pressed = false;
bool right_pressed = false;
bool down_pressed = false;
bool q_pressed = false;
bool e_pressed = false;
bool space_pressed = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Triangle Game", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    {

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

        Renderer renderer;
        handle::Triangle* handle = new handle::Triangle();
        bool shoot = false;
        bool exists = false;
        bool rendered = false;
        handle::Bullet* bullet1 = nullptr;
        handle::Bullet* bullet2 = nullptr;
        handle::Bullet* bullet3 = nullptr;

        long long current_speed = 3;

        auto last_enemy = std::chrono::system_clock::now();

        auto start = std::chrono::system_clock::now();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            renderer.Clear();

            handle->OnRender();

            glfwSetKeyCallback(window, key_callback);

            shoot = handle->OnKeyPress(up_pressed, left_pressed, right_pressed, down_pressed, q_pressed, e_pressed, space_pressed);
          
            if (shoot && !exists)
            {
                bullet1 = new handle::Bullet(handle);
                bullet2 = new handle::Bullet(handle, 120.0f);
                bullet3 = new handle::Bullet(handle, 240.0f);
                exists = true;
                rendered = false;
                start = std::chrono::system_clock::now();
            }
            else if (exists)
            {
                auto now = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = now - start;
                if (elapsed_seconds.count() >= 0.5)
                {
                    exists = false;
                    delete bullet1;
                    delete bullet2;
                    delete bullet3;
                    bullet1 = nullptr;
                    bullet2 = nullptr;
                    bullet3 = nullptr;
                }
            }

            if (exists && !rendered)
            {
                bullet1->OnRender();
                bullet2->OnRender();
                bullet3->OnRender();
                rendered = true;
            }
            else if (exists && rendered)
            {
                bullet1->OnUpdate(0.0f);
                bullet2->OnUpdate(0.0f);
                bullet3->OnUpdate(0.0f);
            }

            auto current_time = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_time = current_time - last_enemy;
            if (elapsed_time.count() > 0.3)
            {
                for (int i = 0; i < handle->num_enemies; i++)
                {
                    if (handle->enemies[i] == nullptr) {
                        handle->enemies[i] = new handle::Enemy(current_speed);
                        handle->enemies[i]->OnRender();
                        last_enemy = std::chrono::system_clock::now();
                        break;
                    }

                    if (i == handle->num_enemies - 1)
                    {
                        for (int j = 0; j < handle->num_enemies; j++)
                        {
                            delete handle->enemies[j];
                            handle->enemies[j] = nullptr;
                        }
                        current_speed += 2;
                    }
                }
            }

            for (int i = 0; i < handle->num_enemies; i++)
            {
                if (handle->enemies[i])
                    handle->enemies[i]->OnUpdate(0.0f);
            }

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }   

        if (handle)
            delete handle;
        if (bullet1)
            delete bullet1;
        if (bullet2)
            delete bullet2;
        if (bullet3)
            delete bullet3;

        for (int i = 0; i < handle->num_enemies; i++)
        {
            if (handle->enemies[i])
                delete handle->enemies[i];
        }
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_PRESS)
        up_pressed = true;
    else if ((key == GLFW_KEY_W || key == GLFW_KEY_UP) && action == GLFW_RELEASE)
        up_pressed = false;
    else if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_PRESS)
        left_pressed = true;
    else if ((key == GLFW_KEY_A || key == GLFW_KEY_LEFT) && action == GLFW_RELEASE)
        left_pressed = false;
    else if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_PRESS)
        right_pressed = true;
    else if ((key == GLFW_KEY_D || key == GLFW_KEY_RIGHT) && action == GLFW_RELEASE)
        right_pressed = false;
    else if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_PRESS)
        down_pressed = true;
    else if ((key == GLFW_KEY_S || key == GLFW_KEY_DOWN) && action == GLFW_RELEASE)
        down_pressed = false;
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        q_pressed = true;
    else if (key == GLFW_KEY_Q && action == GLFW_RELEASE)
        q_pressed = false;
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
        e_pressed = true;
    else if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        e_pressed = false;
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        space_pressed = true;
    else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        space_pressed = false;
}