#include "TestKeyboard.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test {

    TestKeyboard::TestKeyboard() : m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f)),
                                   m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))),
                                   m_Translation(200, 200, 0), m_Rotation(0.0f)
    {
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
    }

    TestKeyboard::~TestKeyboard()
	{
	}

	void TestKeyboard::OnUpdate(float deltaTime)
	{
	}

	void TestKeyboard::OnRender()
	{
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

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

	void TestKeyboard::OnImGuiRender()
	{
	}

    bool TestKeyboard::OnKeyPress(bool up_pressed, bool left_pressed, bool right_pressed, bool down_pressed, bool q_pressed, bool e_pressed, bool space_pressed)
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

    Bullet::Bullet(TestKeyboard* test) : test(test), saved_rotation(0.0f)
    {
        m_BulletTranslation = test->m_Translation;

        float positions[] = {
            -20.0f, -20.0f, 0.0f, 0.0f,  // 0
             20.0f, -20.0f, 1.0f, 0.0f,  // 1
             20.0f,  20.0f, 1.0f, 1.0f,  // 2
            -20.0f,  20.0f, 0.0f, 1.0f   // 3      
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

        m_Texture = std::make_unique<Texture>("res/textures/Bullet.png");
        m_Shader->SetUniform1i("u_Texture", 0);
    }

    Bullet::~Bullet()
    {
    }

    void Bullet::OnUpdate(float deltaTime)
    {
        Renderer renderer;

        m_BulletTranslation.y += 30 * cos(glm::radians(saved_rotation));
        m_BulletTranslation.x += 30 * sin(glm::radians(saved_rotation));

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_BulletTranslation);
            model = glm::translate(model, glm::vec3(0.0f, -22.0f, 0.0f));
            model = glm::rotate(model, glm::radians(saved_rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0.0f, 22.0f, 0.0f));
            glm::mat4 mvp = test->m_Proj * test->m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void Bullet::OnRender()
    {
        Renderer renderer;

        m_Texture->Bind();
        m_BulletTranslation = test->m_Translation;
        saved_rotation = test->m_Rotation;
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_BulletTranslation);
            model = glm::translate(model, glm::vec3(0.0f, -22.0f, 0.0f));
            model = glm::rotate(model, glm::radians(test->m_Rotation), glm::vec3(0, 0, -1));
            model = glm::translate(model, glm::vec3(0.0f, 22.0f, 0.0f));
            glm::mat4 mvp = test->m_Proj * test->m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
    }

    void Bullet::OnImGuiRender()
    {
    }
}