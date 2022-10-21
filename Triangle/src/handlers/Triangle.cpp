#include "Triangle.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace handler {

    Triangle::Triangle() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                           m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                           m_Translation(0, 0, 0), m_Rotation(0),
                           m_bg_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                           m_bg_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                           m_bg_Translation(0, 0, 0)
    {

        float bg_positions[] = {
             0.0f,   0.0f, 0.0f, 0.0f,  // 0
           960.0f,   0.0f, 1.0f, 0.0f,  // 1
           960.0f, 540.0f, 1.0f, 1.0f,  // 2
             0.0f, 540.0f, 0.0f, 1.0f   // 3
        };

        unsigned int bg_indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_bg_VAO = std::make_unique<VertexArray>();

        m_bg_VertexBuffer = std::make_unique<VertexBuffer>(bg_positions, 4 * 4 * sizeof(float));
        VertexBufferLayout bg_layout;
        bg_layout.Push<float>(2);
        bg_layout.Push<float>(2);

        m_bg_VAO->AddBuffer(*m_bg_VertexBuffer, bg_layout);
        m_bg_IndexBuffer = std::make_unique<IndexBuffer>(bg_indices, 6);

        m_bg_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_bg_Shader->Bind();

        m_bg_Texture = std::make_unique<Texture>("res/textures/Background.png");
        m_bg_Shader->SetUniform1i("u_Texture", 0);



        float positions[] = {
            // equilateral triangle at the middle of the window
            430.0f, (float)(270 - 0.5 * sqrt(100 * 100 - 50 * 50)), 0.0f, 0.0f,             // 0
            530.0f, (float)(270 - 0.5 * sqrt(100 * 100 - 50 * 50)), 1.0f, 0.0f,             // 1
            480.0f, (float)(270 + 0.5 * sqrt(100 * 100 - 50 * 50)), 0.5f, (float)sqrt(3)/2  // 2
        };

        unsigned int indices[] = {
            0, 1, 2
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 3 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 3);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();

        m_Texture = std::make_unique<Texture>("res/textures/Triangle.png");
        m_Shader->SetUniform1i("u_Texture", 0);

        for (int i = 0; i < num_enemies; i++)
            enemies[i] = nullptr;
    }

    Triangle::~Triangle()
	{
	}

	void Triangle::OnUpdate(float deltaTime)
	{
	}

	void Triangle::OnRender()
	{
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));


        Renderer bg_renderer;

        m_bg_Texture->Bind();

        {
            glm::mat4 bg_model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
            glm::mat4 bg_mvp = m_bg_Proj * m_bg_View * bg_model;
            m_bg_Shader->Bind();
            m_bg_Shader->SetUniformMat4f("u_MVP", bg_mvp);
            bg_renderer.Draw(*m_bg_VAO, *m_bg_IndexBuffer, *m_bg_Shader);
        }



        Renderer renderer;

        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            model = glm::translate(model, glm::vec3(480, 270, 0));
            model = glm::translate(model, glm::vec3(0, -15, 0));
            model = glm::rotate(model, glm::radians((float)m_Rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0, 15, 0));
            model = glm::translate(model, glm::vec3(-480, -270, 0));
            glm::mat4 mvp = m_Proj * m_View * model;
            m_MVP = mvp;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    bool Triangle::OnKeyPress(bool up_pressed, bool left_pressed, bool right_pressed, bool down_pressed, bool q_pressed, bool e_pressed, bool space_pressed)
    {
        float half_height = (float)(0.5 * sqrt(100 * 100 - 50 * 50));
        float half_width = 100 / 2;

        if (up_pressed && m_Translation.y < 270 - half_height)
            m_Translation.y += 10;
        if (down_pressed && m_Translation.y > -270 + half_height)
            m_Translation.y -= 10;
        if (right_pressed && m_Translation.x < 480 - half_width)
            m_Translation.x += 10;
        if (left_pressed && m_Translation.x > -480 + half_width)
            m_Translation.x -= 10;
        if (q_pressed)
        {
            m_Rotation -= 10;
            m_Rotation %= 360;
        }
        if (e_pressed)
        {
            m_Rotation += 10;
            m_Rotation %= 360;
        }
        if (space_pressed)
        {
            return true;
        }

        return false;
    }

    float Triangle::getVert1x()
    {
        float pos_x = -50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x) * cos(glm::radians((float)(-m_Rotation))) - (pos_y) * sin(glm::radians((float)(-m_Rotation))) + m_Translation.x + 480;
    }

    float Triangle::getVert1y()
    {
        float pos_x = -50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x) * sin(glm::radians((float)(-m_Rotation))) + (pos_y) * cos(glm::radians((float)(-m_Rotation))) + m_Translation.y + 270;
    }

    float Triangle::getVert2x()
    {
        float pos_x = 50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x) * cos(glm::radians((float)(-m_Rotation))) - (pos_y) * sin(glm::radians((float)(-m_Rotation))) + m_Translation.x + 480;
    }

    float Triangle::getVert2y()
    {
        float pos_x = 50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x) * sin(glm::radians((float)(-m_Rotation))) + (pos_y) * cos(glm::radians((float)(-m_Rotation))) + m_Translation.y + 270;
    }

    float Triangle::getVert3x()
    {
        float pos_x = 0;
        float pos_y = (float)(0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x) * cos(glm::radians((float)(-m_Rotation))) - (pos_y) * sin(glm::radians((float)(-m_Rotation))) + m_Translation.x + 480;
    }

    float Triangle::getVert3y()
    {
        float pos_x = 0;
        float pos_y = (float)(0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x) * sin(glm::radians((float)(-m_Rotation))) + (pos_y) * cos(glm::radians((float)(-m_Rotation))) + m_Translation.y + 270;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Bullet::Bullet(Triangle* handler, float init_rotation) : handler(handler), saved_rotation(init_rotation + 0.0f)
    {
        m_BulletTranslation = handler->m_Translation;

        float positions[] = {
            -20.0, -20.0,                      0.0f, 0.0f,               // 0
             20.0, -20.0,                      1.0f, 0.0f,               // 1
              0.0,  40 * ((float)sqrt(3) / 2), 0.5f, (float)sqrt(3) / 2  // 2
        };

        unsigned int indices[] = {
            0, 1, 2
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 3 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 3);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();

        m_Texture = std::make_unique<Texture>("res/textures/Bullet.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    Bullet::~Bullet()
    {
    }

    void Bullet::OnUpdate(float deltaTime)
    {
        Renderer renderer;

        m_Texture->Bind();
        m_BulletTranslation.y += 30 * cos(glm::radians(saved_rotation));
        m_BulletTranslation.x += 30 * sin(glm::radians(saved_rotation));
        
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_BulletTranslation);
            model = glm::translate(model, glm::vec3(0.0f, -22.0f, 0.0f));
            model = glm::rotate(model, glm::radians(saved_rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0.0f, 22.0f, 0.0f));

            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(480, 270, 0));
            //glm::mat4 mvp = handler->m_Proj * handler->m_View * model;
            glm::mat4 mvp = handler->m_Proj * view * model;

            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void Bullet::OnRender()
    {
        Renderer renderer;

        m_Texture->Bind();
        m_BulletTranslation = handler->m_Translation;
        saved_rotation += handler->m_Rotation;
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_BulletTranslation);
            model = glm::translate(model, glm::vec3(0.0f, -22.0f, 0.0f));
            model = glm::rotate(model, glm::radians((float)handler->m_Rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0.0f, 22.0f, 0.0f));

            glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(480, 270, 0));
            //glm::mat4 mvp = handler->m_Proj * handler->m_View * model;
            glm::mat4 mvp = handler->m_Proj * view * model;

            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    float Bullet::getVert1x()
    {
        float pos_x = -50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x)*cos(glm::radians((float)(-saved_rotation))) - (pos_y)*sin(glm::radians((float)(-saved_rotation))) + m_BulletTranslation.x + 480;
    }

    float Bullet::getVert1y()
    {
        float pos_x = -50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x)*sin(glm::radians((float)(-saved_rotation))) + (pos_y)*cos(glm::radians((float)(-saved_rotation))) + m_BulletTranslation.y + 270;
    }

    float Bullet::getVert2x()
    {
        float pos_x = 50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x)*cos(glm::radians((float)(-saved_rotation))) - (pos_y)*sin(glm::radians((float)(-saved_rotation))) + m_BulletTranslation.x + 480;
    }

    float Bullet::getVert2y()
    {
        float pos_x = 50;
        float pos_y = (float)(-0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x)*sin(glm::radians((float)(-saved_rotation))) + (pos_y)*cos(glm::radians((float)(-saved_rotation))) + m_BulletTranslation.y + 270;
    }

    float Bullet::getVert3x()
    {
        float pos_x = 0;
        float pos_y = (float)(0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x)*cos(glm::radians((float)(-saved_rotation))) - (pos_y)*sin(glm::radians((float)(-saved_rotation))) + m_BulletTranslation.x + 480;
    }

    float Bullet::getVert3y()
    {
        float pos_x = 0;
        float pos_y = (float)(0.5 * (float)sqrt(100 * 100 - 50 * 50));

        return (pos_x)*sin(glm::radians((float)(-saved_rotation))) + (pos_y)*cos(glm::radians((float)(-saved_rotation))) + m_BulletTranslation.y + 270;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Enemy::Enemy(long long speed) : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                                    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                                    m_Translation(0, 0, 0), speed(speed)
    {
        radius = float(rand() % 500 + 500);
        degrees = float(rand() % 360);

        float pos_x = radius * sin(glm::radians(degrees));
        float pos_y = radius * cos(glm::radians(degrees));

        float positions[] = {
             455.0f + pos_x, 270.0f + pos_y, 0.0f, 0.0f,  // 0
             485.0f + pos_x, 270.0f + pos_y, 1.0f, 0.0f,  // 1
             485.0f + pos_x, 300.0f + pos_y, 1.0f, 1.0f,  // 2
             455.0f + pos_x, 300.0f + pos_y, 0.0f, 1.0f   // 3      
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();

        m_Texture = std::make_unique<Texture>("res/textures/Enemy.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    Enemy::~Enemy()
    {
    }

    void Enemy::OnUpdate(float deltaTime)
    {
        Renderer renderer;
        m_Texture->Bind();

        m_Translation.x += speed * (-sin(glm::radians(degrees)));
        m_Translation.y += speed * (-cos(glm::radians(degrees)));

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        } 
    }

    void Enemy::OnRender()
    {
        Renderer renderer;

        m_Texture->Bind();
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

    }

    float Enemy::getVert1x()
    {
        float pos_x = radius * sin(glm::radians(degrees));
        return m_Translation.x + pos_x + 455;
    }

    float Enemy::getVert1y()
    {
        float pos_y = radius * cos(glm::radians(degrees));
        return m_Translation.y + pos_y + 270;
    }

    float Enemy::getVert2x()
    {
        float pos_x = radius * sin(glm::radians(degrees));
        return m_Translation.x + pos_x + 485;
    }

    float Enemy::getVert2y()
    {
        float pos_y = radius * cos(glm::radians(degrees));
        return m_Translation.y + pos_y + 270;
    }

    float Enemy::getVert3x()
    {
        float pos_x = radius * sin(glm::radians(degrees));
        return m_Translation.x + pos_x + 485;
    }

    float Enemy::getVert3y()
    {
        float pos_y = radius * cos(glm::radians(degrees));
        return m_Translation.y + pos_y + 300;
    }

    float Enemy::getVert4x()
    {
        float pos_x = radius * sin(glm::radians(degrees));
        return m_Translation.x + pos_x + 455;
    }

    float Enemy::getVert4y()
    {
        float pos_y = radius * cos(glm::radians(degrees));
        return m_Translation.y + pos_y + 300;
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Levelup::Levelup() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                         m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                         m_Translation(0, 0, 0)
    {
        float positions[] = {
             410.0f, 240.0f, 0.0f, 0.0f,  // 0
             550.0f, 240.0f, 1.0f, 0.0f,  // 1
             550.0f, 300.0f, 1.0f, 1.0f,  // 2
             410.0f, 300.0f, 0.0f, 1.0f   // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();

        m_Texture = std::make_unique<Texture>("res/textures/Levelup.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    Levelup::~Levelup()
    {
    }

    void Levelup::OnUpdate(float deltaTime)
    {
        Renderer renderer;
        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void Levelup::OnRender()
    {
        Renderer renderer;
        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Instructions::Instructions() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                                   m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                                   m_Translation(0, 0, 0)
    {
        float positions[] = {
             280.0f, 140.0f, 0.0f, 0.0f,  // 0
             660.0f, 140.0f, 1.0f, 0.0f,  // 1
             660.0f, 440.0f, 1.0f, 1.0f,  // 2
             280.0f, 440.0f, 0.0f, 1.0f   // 3
        };

        float positions2[] = {
            280.0f, 100.0f, 0.0f, 0.0f,  // 0
            660.0f, 100.0f, 1.0f, 0.0f,  // 1
            660.0f, 140.0f, 1.0f, 1.0f,  // 2
            280.0f, 140.0f, 0.0f, 1.0f   // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        unsigned int indices2[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);

        m_VAO->AddBuffer(*m_VertexBuffer, layout);
        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();

        m_Texture = std::make_unique<Texture>("res/textures/Instructions.png");
        m_Shader->SetUniform1i("u_Texture", 0);


        m_VAO2 = std::make_unique<VertexArray>();

        m_VertexBuffer2 = std::make_unique<VertexBuffer>(positions2, 4 * 4 * sizeof(float));
        VertexBufferLayout layout2;
        layout2.Push<float>(2);
        layout2.Push<float>(2);

        m_VAO2->AddBuffer(*m_VertexBuffer2, layout2);
        m_IndexBuffer2 = std::make_unique<IndexBuffer>(indices2, 6);

        m_Shader2 = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader2->Bind();

        m_Texture2 = std::make_unique<Texture>("res/textures/Instructions2.png");
        m_Shader2->SetUniform1i("u_Texture", 0);
    }
    
    Instructions::~Instructions()
    {
    }
    
    void Instructions::OnUpdate(float deltaTime)
    {
    }
    
    void Instructions::OnRender()
    {
        Renderer renderer;
        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

        m_Texture2->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader2->Bind();
            m_Shader2->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO2, *m_IndexBuffer2, *m_Shader2);
        }
    }

    bool Instructions::OnKeyPress(bool enter_pressed)
    {
        if (enter_pressed)
            return true;
        return false;
    }
}