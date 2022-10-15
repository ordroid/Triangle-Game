#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>

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

#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"
#include "tests/TestKeyboard.h"

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
    window = glfwCreateWindow(960, 540, "Triangle Game", NULL, NULL);
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
        test::TestKeyboard* test = new test::TestKeyboard();
        bool shoot = false;
        bool exists = false;
        bool rendered = false;
        test::Bullet* bullet = nullptr;

        auto start = std::chrono::system_clock::now();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            renderer.Clear();

            test->OnRender();

            glfwSetKeyCallback(window, key_callback);

            shoot = test->OnKeyPress(up_pressed, left_pressed, right_pressed, down_pressed, q_pressed, e_pressed, space_pressed);
          
            if (shoot && !exists)
            {
                bullet = new test::Bullet(test);
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
                    delete bullet;
                    bullet = nullptr;
                }
            }

            if (exists && !rendered)
            {
                bullet->OnRender();
                rendered = true;
            }
            else if (exists && rendered)
                bullet->OnUpdate(0.0f);

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }   

        if (test)
            delete test;
        if (bullet)
            delete bullet;
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        up_pressed = true;
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        up_pressed = false;
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        left_pressed = true;
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        left_pressed = false;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        right_pressed = true;
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        right_pressed = false;
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        down_pressed = true;
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
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