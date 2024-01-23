#pragma once

#include "Handler.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include <memory>
#include <time.h>

#define ENEMIES 100


namespace handler {
	class Triangle : public Handler
	{
	public:
		Triangle();
		~Triangle();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

		bool OnKeyPress(bool up_pressed, bool left_pressed, bool right_pressed, bool down_pressed, bool q_pressed, bool e_pressed, bool space_pressed);
		void OnBurn();

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

		std::unique_ptr<VertexArray> m_bg_VAO;
		std::unique_ptr<VertexBuffer> m_bg_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_bg_IndexBuffer;
		std::unique_ptr<Shader> m_bg_Shader;
		std::unique_ptr<Texture> m_bg_Texture;

		glm::mat4 m_bg_Proj;
		glm::mat4 m_bg_View;
		glm::vec3 m_bg_Translation;
		glm::mat4 m_bg_MVP;

		int m_Rotation;
		
		friend class Bullet;
		friend class Enemy;

	public:
		Enemy* enemies[ENEMIES];
		int num_enemies = ENEMIES;

		float getVert1x();
		float getVert1y();
		float getVert2x();
		float getVert2y();
		float getVert3x();
		float getVert3y();

		float getRotation();
	};
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Bullet : public Handler
	{
	public:
		Bullet(Triangle* handler, float init_rotation = 0);
		~Bullet();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		Triangle* handler;

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::vec3 m_BulletTranslation;
		float saved_rotation;

	public:
		float getVert1x();
		float getVert1y();
		float getVert2x();
		float getVert2y();
		float getVert3x();
		float getVert3y();
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Enemy : public Handler
	{
	public:
		Enemy(long long speed);
		~Enemy();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

		void OnBurn();

	private:
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

	public:
		float getVert1x();
		float getVert1y();
		float getVert2x();
		float getVert2y();
		float getVert3x();
		float getVert3y();
		float getVert4x();
		float getVert4y();

		void stopMotion();
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Levelup : public Handler
	{
	public:
		Levelup();
		~Levelup();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::vec3 m_Translation;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Instructions : public Handler
	{
	public:
		Instructions();
		~Instructions();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

		bool OnKeyPress(bool enter_pressed);

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		std::unique_ptr<VertexArray> m_VAO2;
		std::unique_ptr<VertexBuffer> m_VertexBuffer2;
		std::unique_ptr<IndexBuffer> m_IndexBuffer2;
		std::unique_ptr<Shader> m_Shader2;
		std::unique_ptr<Texture> m_Texture2;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::vec3 m_Translation;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Gameover : public Handler
	{
	public:
		Gameover();
		~Gameover();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		std::unique_ptr<VertexArray> m_inst_VAO;
		std::unique_ptr<VertexBuffer> m_inst_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_inst_IndexBuffer;
		std::unique_ptr<Shader> m_inst_Shader;
		std::unique_ptr<Texture> m_inst_Texture;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::vec3 m_Translation;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Timer : public Handler
	{
	public:
		Timer(int* digit_array, int size);
		~Timer();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		std::unique_ptr<VertexArray> m_header_VAO;
		std::unique_ptr<VertexBuffer> m_header_VertexBuffer;

		glm::mat4 m_Proj;
		glm::mat4 m_View;
		glm::vec3 m_Translation;

		int* digit_array;
		int size;
	};
}