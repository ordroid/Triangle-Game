#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <memory>


namespace test {
	class TestKeyboard : public Test
	{
	public:
		TestKeyboard();
		~TestKeyboard();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

		bool OnKeyPress(bool up_pressed, bool left_pressed, bool right_pressed, bool down_pressed, bool q_pressed, bool e_pressed, bool space_pressed);

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::vec3 m_Translation;
		glm::mat4 m_MVP;

		float m_Rotation;
		
		friend class Bullet;
	};
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Bullet : public Test
	{
	public:
		Bullet(TestKeyboard* test);
		~Bullet();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;

	private:
		TestKeyboard* test;

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::vec3 m_BulletTranslation;
		float saved_rotation;
	};
}