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


#define UNINITIALIZED -1000.0f

bool up_pressed = false;
bool left_pressed = false;
bool right_pressed = false;
bool down_pressed = false;
bool q_pressed = false;
bool e_pressed = false;
bool space_pressed = false;
bool n_pressed = false;
bool esc_pressed = false;
bool enter_pressed = false;
bool end_game = false;
int killer = -1;
bool start_game = true;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void instructions_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void check_collisions(handler::Triangle* triangle, handler::Bullet* bullets[]);

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

    //std::cout << glGetString(GL_VERSION) << std::endl;
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

        handler::Gameover* gameover;
        gameover = nullptr;
        bool just_ended = true;
        auto end_time = std::chrono::system_clock::now();
        bool ended_two_seconds_ago = false;

        long long current_speed = 3;

        bool* already_shot = new bool[sizeof(bool) * handler->num_enemies];
        for (int i = 0; i < handler->num_enemies; i++)
            already_shot[i] = false;

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
                handler->OnRender();
                instructions->OnRender();
                start_time = std::chrono::system_clock::now();
            }
            else if(!end_game)
            {

                if (start_game)
                {
                    start_game = false;
                    end_game = false;

                    delete handler;
                    delete lvl;
                    delete gameover;
                    delete[] already_shot;

                    handler = new handler::Triangle();
                    shoot = false;
                    exists = 0;
                    rendered_first = false;
                    rendered_second = false;

                    handler::Bullet* bullets[6]{};
                    for (int i = 0; i < 6; i++)
                    {
                        if(bullets[i])
                            delete bullets[i];
                        bullets[i] = nullptr;
                    }
                    shooting = 0;

                    lvl = nullptr;
                    show = false;
                    show_count = 0;

                    gameover = nullptr;
                    just_ended = true;
                    end_time = std::chrono::system_clock::now();
                    ended_two_seconds_ago = false;

                    current_speed = 3;

                    already_shot = new bool[sizeof(bool) * handler->num_enemies];
                    for (int i = 0; i < handler->num_enemies; i++)
                        already_shot[i] = false;

                    loop_instructions = 0;
                    start_time = std::chrono::system_clock::now();

                    last_enemy = std::chrono::system_clock::now();

                    start = std::chrono::system_clock::now();
                }

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
                        if (!handler->enemies[i] && !already_shot[i])
                        {
                            already_shot[i] = true;
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
                                already_shot[j] = false;
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

                check_collisions(handler, bullets);
            }

            if (end_game)
            {
                if (just_ended)
                {
                    end_time = std::chrono::system_clock::now();
                    gameover = new handler::Gameover();
                    handler->OnBurn();
                    handler->enemies[killer]->OnBurn();
                    handler->enemies[killer]->stopMotion();
                    just_ended = false;
                }

                if (ended_two_seconds_ago)
                {
                    gameover->OnRender();
                }
                else
                {
                    handler->OnRender();
                    handler->enemies[killer]->OnUpdate(0.0f);
                    auto now_burning = std::chrono::system_clock::now();
                    std::chrono::duration<double> burning_time = now_burning - end_time;
                    if (burning_time.count() >= 2)
                        ended_two_seconds_ago = true;
                }

                std::chrono::duration<double> game_time = end_time - start_time;

                std::cout << "GAME OVER!\nGame time: " << game_time.count() << std::endl;
                

                glfwSetKeyCallback(window, key_callback);

                if (esc_pressed)
                    break;

                if (n_pressed)
                {
                    start_game = true;
                    end_game = false;
                }

                if(ended_two_seconds_ago)
                    gameover->OnUpdate(0.0f);
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

        if (gameover)
            delete gameover;

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
    else if (key == GLFW_KEY_N && action == GLFW_PRESS)
        n_pressed = true;
    else if (key == GLFW_KEY_N && action == GLFW_RELEASE)
        n_pressed = false;
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        esc_pressed = true;
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        esc_pressed = false;
}

void instructions_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        enter_pressed = true;
}

void check_collisions(handler::Triangle* triangle, handler::Bullet* bullets[])
{
    handler::Enemy** enemies = triangle->enemies;

    // Collision of Triangle and Enemies:

    float triangle_vertices[3][2] = { {triangle->getVert1x(), triangle->getVert1y()},
                                      {triangle->getVert2x(), triangle->getVert2y()},
                                      {triangle->getVert3x(), triangle->getVert3y()} };

    float bullet0_vertices[3][2], bullet1_vertices[3][2], bullet2_vertices[3][2], 
          bullet3_vertices[3][2], bullet4_vertices[3][2], bullet5_vertices[3][2];


    if (bullets[0])
    {
        bullet0_vertices[0][0] = bullets[0]->getVert1x();
        bullet0_vertices[0][1] = bullets[0]->getVert1y();
        bullet0_vertices[1][0] = bullets[0]->getVert2x();
        bullet0_vertices[1][1] = bullets[0]->getVert2y();
        bullet0_vertices[2][0] = bullets[0]->getVert3x();
        bullet0_vertices[2][1] = bullets[0]->getVert3y();

        bullet1_vertices[0][0] = bullets[1]->getVert1x();
        bullet1_vertices[0][1] = bullets[1]->getVert1y();
        bullet1_vertices[1][0] = bullets[1]->getVert2x();
        bullet1_vertices[1][1] = bullets[1]->getVert2y();
        bullet1_vertices[2][0] = bullets[1]->getVert3x();
        bullet1_vertices[2][1] = bullets[1]->getVert3y();

        bullet2_vertices[0][0] = bullets[2]->getVert1x();
        bullet2_vertices[0][1] = bullets[2]->getVert1y();
        bullet2_vertices[1][0] = bullets[2]->getVert2x();
        bullet2_vertices[1][1] = bullets[2]->getVert2y();
        bullet2_vertices[2][0] = bullets[2]->getVert3x();
        bullet2_vertices[2][1] = bullets[2]->getVert3y();
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                bullet0_vertices[i][j] = UNINITIALIZED;
                bullet1_vertices[i][j] = UNINITIALIZED;
                bullet2_vertices[i][j] = UNINITIALIZED;
            }
        }
    }

    if (bullets[3])
    {
        bullet3_vertices[0][0] = bullets[3]->getVert1x();
        bullet3_vertices[0][1] = bullets[3]->getVert1y();
        bullet3_vertices[1][0] = bullets[3]->getVert2x();
        bullet3_vertices[1][1] = bullets[3]->getVert2y();
        bullet3_vertices[2][0] = bullets[3]->getVert3x();
        bullet3_vertices[2][1] = bullets[3]->getVert3y();

        bullet4_vertices[0][0] = bullets[4]->getVert1x();
        bullet4_vertices[0][1] = bullets[4]->getVert1y();
        bullet4_vertices[1][0] = bullets[4]->getVert2x();
        bullet4_vertices[1][1] = bullets[4]->getVert2y();
        bullet4_vertices[2][0] = bullets[4]->getVert3x();
        bullet4_vertices[2][1] = bullets[4]->getVert3y();

        bullet5_vertices[0][0] = bullets[5]->getVert1x();
        bullet5_vertices[0][1] = bullets[5]->getVert1y();
        bullet5_vertices[1][0] = bullets[5]->getVert2x();
        bullet5_vertices[1][1] = bullets[5]->getVert2y();
        bullet5_vertices[2][0] = bullets[5]->getVert3x();
        bullet5_vertices[2][1] = bullets[5]->getVert3y();
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                bullet3_vertices[i][j] = UNINITIALIZED;
                bullet4_vertices[i][j] = UNINITIALIZED;
                bullet5_vertices[i][j] = UNINITIALIZED;
            }
        }
    }

    //std::cout << "Triangle vertex #0: x: " << triangle_vertices[0][0] << ", y: " << triangle_vertices[0][1] << std::endl;

    for (int i = 0; i < triangle->num_enemies; i++)
    {
        if (!enemies[i])
            continue;

        float enemy_vertices[4][2] = { {enemies[i]->getVert1x(), enemies[i]->getVert1y()},
                                       {enemies[i]->getVert2x(), enemies[i]->getVert2y()},
                                       {enemies[i]->getVert3x(), enemies[i]->getVert3y()},
                                       {enemies[i]->getVert4x(), enemies[i]->getVert4y()} };

        if (((enemy_vertices[0][0] <= triangle_vertices[0][0] && triangle_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= triangle_vertices[1][0] && triangle_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= triangle_vertices[2][0] && triangle_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= triangle_vertices[0][1] && triangle_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= triangle_vertices[1][1] && triangle_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= triangle_vertices[2][1] && triangle_vertices[2][1] <= enemy_vertices[2][1])))
        {
            //std::cout << "BOOM! Vertex: X: " << enemies[i]->getVert1x() << ", Y : " << enemies[i]->getVert1y() << std::endl;
            end_game = true;
            killer = i;
            return;
        }


        if ((((enemy_vertices[0][0] <= bullet0_vertices[0][0] && bullet0_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet0_vertices[1][0] && bullet0_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet0_vertices[2][0] && bullet0_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= bullet0_vertices[0][1] && bullet0_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet0_vertices[1][1] && bullet0_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet0_vertices[2][1] && bullet0_vertices[2][1] <= enemy_vertices[2][1])))
            || (((enemy_vertices[0][0] <= bullet1_vertices[0][0] && bullet1_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet1_vertices[1][0] && bullet1_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet1_vertices[2][0] && bullet1_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= bullet1_vertices[0][1] && bullet1_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet1_vertices[1][1] && bullet1_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet1_vertices[2][1] && bullet1_vertices[2][1] <= enemy_vertices[2][1])))
            || (((enemy_vertices[0][0] <= bullet2_vertices[0][0] && bullet2_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet2_vertices[1][0] && bullet2_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet2_vertices[2][0] && bullet2_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= bullet2_vertices[0][1] && bullet2_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet2_vertices[1][1] && bullet2_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet2_vertices[2][1] && bullet2_vertices[2][1] <= enemy_vertices[2][1])))
            || (((enemy_vertices[0][0] <= bullet3_vertices[0][0] && bullet3_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet3_vertices[1][0] && bullet3_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet3_vertices[2][0] && bullet3_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= bullet3_vertices[0][1] && bullet3_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet3_vertices[1][1] && bullet3_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet3_vertices[2][1] && bullet3_vertices[2][1] <= enemy_vertices[2][1])))
            || (((enemy_vertices[0][0] <= bullet4_vertices[0][0] && bullet4_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet4_vertices[1][0] && bullet4_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet4_vertices[2][0] && bullet4_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= bullet4_vertices[0][1] && bullet4_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet4_vertices[1][1] && bullet4_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet4_vertices[2][1] && bullet4_vertices[2][1] <= enemy_vertices[2][1])))
            || (((enemy_vertices[0][0] <= bullet5_vertices[0][0] && bullet5_vertices[0][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet5_vertices[1][0] && bullet5_vertices[1][0] <= enemy_vertices[1][0])
            || (enemy_vertices[0][0] <= bullet5_vertices[2][0] && bullet5_vertices[2][0] <= enemy_vertices[1][0]))
            && ((enemy_vertices[1][1] <= bullet5_vertices[0][1] && bullet5_vertices[0][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet5_vertices[1][1] && bullet5_vertices[1][1] <= enemy_vertices[2][1])
            || (enemy_vertices[1][1] <= bullet5_vertices[2][1] && bullet5_vertices[2][1] <= enemy_vertices[2][1]))))
        {
            delete enemies[i];
            enemies[i] = nullptr;
        }
    }
}