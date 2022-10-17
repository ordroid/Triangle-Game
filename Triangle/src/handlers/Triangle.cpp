#include "Triangle.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace handler {

    Triangle::Triangle() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                           m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                           m_Translation(460, 260, 0), m_Rotation(0.0f),
                           m_bg_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                           m_bg_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                           m_bg_Translation(460, 260, 0)
    {

        float bg_positions[] = {
            -460.0f, -260.0f, 0.0f, 0.0f,  // 0
             500.0f, -260.0f, 1.0f, 0.0f,  // 1
             500.0f,  280.0f, 1.0f, 1.0f,  // 2
            -460.0f,  280.0f, 0.0f, 1.0f   // 3
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
            -50.0f, -50.0f,        0.0f, 0.0f,          // 0
             50.0f, -50.0f,        1.0f, 0.0f,          // 1
              0.0f,  43.30127019f, 0.5f, 0.8660254038f  // 2
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
            glm::mat4 bg_model = glm::translate(glm::mat4(1.0f), glm::vec3(460, 260, 0));
            glm::mat4 bg_mvp = m_bg_Proj * m_bg_View * bg_model;
            m_bg_Shader->Bind();
            m_bg_Shader->SetUniformMat4f("u_MVP", bg_mvp);
            bg_renderer.Draw(*m_bg_VAO, *m_bg_IndexBuffer, *m_bg_Shader);
        }





        Renderer renderer;

        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Translation);
            model = glm::translate(model, glm::vec3(0.0f, -22.0f, 0.0f));
            model = glm::rotate(model, glm::radians(m_Rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0.0f, 22.0f, 0.0f));
            glm::mat4 mvp = m_Proj * m_View * model;
            m_MVP = mvp;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    bool Triangle::OnKeyPress(bool up_pressed, bool left_pressed, bool right_pressed, bool down_pressed, bool q_pressed, bool e_pressed, bool space_pressed)
    {
        if (up_pressed && m_Translation.y < 500)
            m_Translation.y += 10;
        if (down_pressed && m_Translation.y > 50)
            m_Translation.y -= 10;
        if (right_pressed && m_Translation.x < 910)
            m_Translation.x += 10;
        if (left_pressed && m_Translation.x > 50)
            m_Translation.x -= 10;
        if (q_pressed)
            m_Rotation -= 10.0f;
        if (e_pressed)
            m_Rotation += 10.0f;
        if (space_pressed)
        {
            return true;
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Bullet::Bullet(Triangle* handler, float init_rotation) : handler(handler), saved_rotation(init_rotation + 0.0f)
    {
        m_BulletTranslation = handler->m_Translation;

        float positions[] = {
            -20.0f, -20.0f, 0.0f, 0.0f,  // 0
             20.0f, -20.0f, 1.0f, 0.0f,  // 1
              0.0f,  37.2f, 1.0f, 1.0f,  // 2
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
            glm::mat4 mvp = handler->m_Proj * handler->m_View * model;
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
            model = glm::rotate(model, glm::radians(handler->m_Rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0.0f, 22.0f, 0.0f));
            glm::mat4 mvp = handler->m_Proj * handler->m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Enemy::Enemy(long long speed) : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                                    m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                                    m_Translation(200, 200, 0), speed(speed)
    {
        radius = float(rand() % 500 + 400);
        degrees = float(rand() % 360);

        float pos_x = radius * sin(glm::radians(degrees));
        float pos_y = radius * cos(glm::radians(degrees));

        float positions[] = {
             245.0f + pos_x, 100.0f + pos_y, 0.0f, 0.0f,  // 0
             275.0f + pos_x, 100.0f + pos_y, 1.0f, 0.0f,  // 1
             275.0f + pos_x, 130.0f + pos_y, 1.0f, 1.0f,  // 2
             245.0f + pos_x, 130.0f + pos_y, 0.0f, 1.0f   // 3      
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

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    Levelup::Levelup() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                         m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                         m_Translation(0, 0, 0)
    {
        float positions[] = {
             430.0f, 240.0f, 0.0f, 0.0f,  // 0
             530.0f, 240.0f, 1.0f, 0.0f,  // 1
             530.0f, 300.0f, 1.0f, 1.0f,  // 2
             430.0f, 300.0f, 0.0f, 1.0f   // 3
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
}