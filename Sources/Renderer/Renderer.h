#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Utiles/Singleton.h"
#include "Utiles/Camera.h"
#include "Resources/ResourceManager.h"
#include "Instances/InstanceManager.h"
#include "Scene/SceneManager.h"



class Renderer : public Singleton<Renderer>
{
	friend class Singleton<Renderer>;

	public:
		//  Public functions
		void render();
		
		void setGridVisible(bool enable);
		bool gridVisible();
		void initializeGrid(unsigned int gridSize, float elementSize = 1.f);
		//

		//  Set/get functions
		void setCamera(Camera* cam);
		void setWindow(GLFWwindow* win);
		void setDefaultShader(Shader* shad);
		
		Camera* getCamera();
		GLFWwindow* getWindow();
		Shader* getDefaultShader();
		//

	private:
		//  Default
		Renderer();
		~Renderer();
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
		uint16_t* indexBufferGrid;
		//
};

