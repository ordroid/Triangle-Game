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
bool enter_pressed = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void instructions_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

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

        handler::Instructions* instructions = new handler::Instructions();

        handler::Triangle* handler = new handler::Triangle();
        bool shoot = false;
        int exists = 0;
        bool rendered_first = false;
        bool rendered_second = false;

        handler::Bullet* bullets[6]{};
        for (int i = 0; i < 6; i++)
        {
            bullets[i] = nullptr;
        }
        int shooting = 0;

        handler::Levelup* lvl = nullptr;
        bool show = false;
        int show_count = 0;

        long long current_speed = 3;

        int loop_instructions = 0;
        auto start_time = std::chrono::system_clock::now();

        auto last_enemy = std::chrono::system_clock::now();

        auto start = std::chrono::system_clock::now();

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            renderer.Clear();
            
            glfwSetKeyCallback(window, instructions_callback);

            if (!instructions->OnKeyPress(enter_pressed))
            {
                instructions->OnRender();
            }
            else
            {
                handler->OnRender();

                glfwSetKeyCallback(window, key_callback);

                shoot = handler->OnKeyPress(up_pressed, left_pressed, right_pressed, down_pressed, q_pressed, e_pressed, space_pressed);

                if (shoot && exists < 2)
                {
                    bullets[0 + shooting] = new handler::Bullet(handler);
                    bullets[1 + shooting] = new handler::Bullet(handler, 120.0f);
                    bullets[2 + shooting] = new handler::Bullet(handler, 240.0f);

                    start = std::chrono::system_clock::now();

                    exists++;

                    if (shooting == 0)
                        rendered_first = false;
                    else
                        rendered_second = false;

                    shooting = 3 - shooting;
                }
                else
                {
                    auto now = std::chrono::system_clock::now();
                    std::chrono::duration<double> elapsed_seconds = now - start;
                    if (elapsed_seconds.count() >= 0.3)
                    {
                        if (bullets[0 + shooting])
                        {
                            delete bullets[0 + shooting];
                            delete bullets[1 + shooting];
                            delete bullets[2 + shooting];
                            bullets[0 + shooting] = nullptr;
                            bullets[1 + shooting] = nullptr;
                            bullets[2 + shooting] = nullptr;
                            exists--;
                        }
                    }
                }

                bool change_rendered_first = false;
                bool change_rendered_second = false;
                if (exists > 0)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        if (!rendered_first)
                        {
                            if (bullets[i])
                            {
                                bullets[i]->OnRender();
                                change_rendered_first = true;
                            }
                        }
                        else
                        {
                            if (bullets[i])
                                bullets[i]->OnUpdate(0.0f);
                        }

                        if (!rendered_second)
                        {
                            if (bullets[i + 3])
                            {
                                bullets[i + 3]->OnRender();
                                change_rendered_second = true;
                            }
                        }
                        else
                        {
                            if (bullets[i + 3])
                                bullets[i + 3]->OnUpdate(0.0f);
                        }
                    }

                    if (change_rendered_first)
                        rendered_first = true;

                    if (change_rendered_second)
                        rendered_second = true;
                }

                if (show)
                {
                    lvl->OnUpdate(0.0f);
                    show_count++;
                }

                if (show_count == 30)
                {
                    show = false;
                    show_count = 0;
                }

                auto current_time = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_time = current_time - last_enemy;
                if (elapsed_time.count() > 0.3)
                {
                    for (int i = 0; i < handler->num_enemies; i++)
                    {
                        if (handler->enemies[i] == nullptr)
                        {
                            handler->enemies[i] = new handler::Enemy(current_speed);
                            handler->enemies[i]->OnRender();
                            last_enemy = std::chrono::system_clock::now();
                            break;
                        }

                        if (i == handler->num_enemies - 1)
                        {
                            lvl = new handler::Levelup();
                            lvl->OnRender();
                            show = true;
                            show_count = 0;

                            for (int j = 0; j < handler->num_enemies; j++)
                            {
                                delete handler->enemies[j];
                                handler->enemies[j] = nullptr;
                            }
                            current_speed += 2;
                        }
                    }
                }

                for (int i = 0; i < handler->num_enemies; i++)
                {
                    if (handler->enemies[i])
                        handler->enemies[i]->OnUpdate(0.0f);
                }
            }

            /* Swap front and back buffers */
            GLCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCall(glfwPollEvents());
        }   

        if (bullets[0])
            delete bullets[0];
        if (bullets[1])
            delete bullets[1];
        if (bullets[2])
            delete bullets[2];

        if (lvl)
            delete lvl;

        if (instructions)
            delete instructions;

        for (int i = 0; i < handler->num_enemies; i++)
        {
            if (handler->enemies[i])
                delete handler->enemies[i];
        }

        if (handler)
            delete handler;


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

void instructions_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        enter_pressed = true;
}