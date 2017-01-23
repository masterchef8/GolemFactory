#pragma once

#include <iostream>

#include <GL/glew.h>

#include "Utiles/Mutex.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Utiles/Camera.h"
#include "Utiles/Singleton.h"
#include "Resources/ResourceManager.h"
#include "Instances/InstanceManager.h"
#include "Scene/SceneManager.h"

class Renderer : public Singleton<Renderer>
{
	friend class Singleton<Renderer>;

	public:
		//  Public functions
		void initGLEW(int verbose = 1);
		void render();
		
		void setGridVisible(bool enable);
		bool gridVisible();
		void initializeGrid(unsigned int gridSize, float elementSize = 1.f);
		//

		//  Set/get functions
		void setCamera(Camera* cam);
		void setWindow(GLFWwindow* win);
		void setDefaultShader(Shader* s);
		void setGridShader(Shader* s);
		
		Camera* getCamera();
		GLFWwindow* getWindow();
		Shader* getDefaultShader();
		//

	private:
		//  Default
		Renderer();
		~Renderer();
		//

		//	Protected functions
		void loadMVPMatrix(Shader* shader, const float* model, const float* view, const float* projection) const;
		void drawInstanceDrawable(InstanceDrawable* ins, const float* view, const float* projection) const;
		void drawInstanceContainer(InstanceContainer* ins, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model);
		//

		//  Attributes
		GLFWwindow* window;
		Camera* camera;

		Shader* gridShader;
		Shader* defaultShader;
		bool drawGrid;
		unsigned int vboGridSize;
		GLuint gridVAO, vertexbuffer, arraybuffer;
		float* vertexBufferGrid;
		uint32_t* indexBufferGrid;

		double dummy;
		//
};

