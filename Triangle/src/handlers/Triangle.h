#pragma once

#include "Handle.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <memory>

#define ENEMIES 100


namespace handle {
	class Triangle : public Handle
	{
	public:
		Triangle();
		~Triangle();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

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
		friend class Enemy;

	public:
		Enemy* enemies[ENEMIES];
		int num_enemies = ENEMIES;
	};
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Bullet : public Handle
	{
	public:
		Bullet(Triangle* handle, float init_rotation = 0);
		~Bullet();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		Triangle* handle;

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::vec3 m_BulletTranslation;
		float saved_rotation;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Enemy : public Handle
	{
	public:
		Enemy(long long speed);
		~Enemy();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		Triangle* handle;

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::vec3 m_Translation;

		float radius;
		float degrees;

		long long speed;
	};
}