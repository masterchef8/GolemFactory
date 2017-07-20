// Golem Factory 1.0.cpp : define consol application entry point
//

#include <iostream>
#include <list>
#include <time.h>
#include <GL/glew.h>

#include "Utiles/System.h"
#include "Events/EventHandler.h"
#include "Renderer/Renderer.h"
#include "Generators/HouseGenerator.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>

#define GRID_SIZE 100
#define GRID_ELEMENT_SIZE 5.f

// prototypes
GLFWwindow* initGLFW();
void initializeForestScene();
//


//
//void generateRagdoll(Skeleton* s);
//


//std::string resourceRepository = "C:/Users/Thibault/Documents/Github/GolemFactory/Resources/";
std::string resourceRepository = "C:/Users/Thibault/Documents/Github/GolemFactory/Resources/";
//std::string resourceRepository = "Resources/";



// program
int main()
{
	// init window and opengl
	GLFWwindow* window = initGLFW();
	Renderer::getInstance()->initGLEW(0);

	// Init Event handler
	EventHandler::getInstance()->addWindow(window);
	EventHandler::getInstance()->setRepository(resourceRepository);
	EventHandler::getInstance()->loadKeyMapping("RPG Key mapping");
	EventHandler::getInstance()->setCursorMode(false);
	
	// Init Resources manager & instance manager
	ResourceManager::getInstance()->setRepository(resourceRepository);
	InstanceManager::getInstance()->setMaxNumberOfInstances(1000000);
	
	// Init Renderer;
	Camera camera;
		camera.setPosition(glm::vec3(0,-3,3));
	Renderer::getInstance()->setCamera(&camera);
	Renderer::getInstance()->setWindow(window);
	Renderer::getInstance()->initializeGrid(GRID_SIZE, GRID_ELEMENT_SIZE);
	ResourceManager::getInstance()->getShader("tree");

	// init scene
	SceneManager::getInstance()->setWorldPosition(glm::vec3(0,0,25));
	SceneManager::getInstance()->setWorldSize(glm::vec3(GRID_SIZE*GRID_ELEMENT_SIZE, GRID_SIZE*GRID_ELEMENT_SIZE, 50));
	

		//initializeForestScene();
		InstanceDrawable* peasant = InstanceManager::getInstance()->getInstanceDrawable("Peasant8.dae", "skeletonDebug");
		peasant->setSize(glm::vec3(0.01, 0.01, 0.01));
		peasant->setPosition(glm::vec3(0.f, 0.f, 0.94f));
		SceneManager::getInstance()->addStaticObject(peasant);

		InstanceDrawable* peasant2 = InstanceManager::getInstance()->getInstanceDrawable("Peasant8.dae", "skinning");
		peasant2->setSize(peasant->getSize());
		peasant2->setPosition(peasant->getPosition() + glm::vec3(0.f, 0.5f, 0.f));
		SceneManager::getInstance()->addStaticObject(peasant2);




	// init loop time tracking
	double elapseTime = 16.;
	bool FPScam = false;

	std::cout << "game loop initiated" << std::endl;

	int frame = 0;
	//std::mt19937 randomEngine;

	while (!glfwWindowShouldClose(window))
	{
		//frame++;
		if (frame > 30)
		{
			frame = 0;
			EventHandler::getInstance()->addFrameEvent(DOUBLE_CLICK_LEFT);
		}

		// begin loop
		double startTime = glfwGetTime();
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float angle = camera.getFrustrumAngleVertical() + EventHandler::getInstance()->getScrollingRelative().y;
		if (angle > 70.f) angle = 70.f;
		else if (angle < 3) angle = 3.f;
		camera.setFrustrumAngleVertical(angle);
		camera.setFrustrumAngleHorizontalFromScreenRatio((float)width / height);

		if (FPScam)
		{
			glm::vec3 cp = camera.getPosition();
				cp.z = 1.7f;
			camera.setPosition(cp);
		}


		// Render frame
		Renderer::getInstance()->render();

		//  handle events
		EventHandler::getInstance()->handleEvent();
		std::vector<UserEventType> v;
		EventHandler::getInstance()->getFrameEvent(v);
		for (unsigned int i = 0; i<v.size(); i++)
		{
			if(v[i] == QUIT) glfwSetWindowShouldClose(window, GL_TRUE);
			else if(v[i] == CHANGE_CURSOR_MODE) EventHandler::getInstance()->setCursorMode(!EventHandler::getInstance()->getCursorMode());
			else if (v[i] == ACTION) FPScam = !FPScam;
			/*else if (v[i] == DOUBLE_CLICK_LEFT)
			{
				if (!SceneManager::getInstance()->removeObject(house))
					std::cout << "fail remove instance from scene" << std::endl;
				InstanceManager::getInstance()->release(house);

				double time = glfwGetTime();
				house = hg.getHouse(randomEngine()%10000, 20, 30);
				std::cout << 1000.f*(glfwGetTime() - time) << std::endl;
				InstanceManager::getInstance()->add(house);
				SceneManager::getInstance()->addStaticObject(house);
			}*/
		}

		//Animate camera
		camera.animate((float)elapseTime,
			EventHandler::getInstance()->isActivated(FORWARD),	EventHandler::getInstance()->isActivated(BACKWARD),
			EventHandler::getInstance()->isActivated(LEFT),		EventHandler::getInstance()->isActivated(RIGHT),
			EventHandler::getInstance()->isActivated(RUN),		EventHandler::getInstance()->isActivated(SNEAKY)    );

		//	clear garbages
		InstanceManager::getInstance()->clearGarbage();
		ResourceManager::getInstance()->clearGarbage();

		//	Debug
		//std::cout << 1000.f*(glfwGetTime() - startTime) << std::endl;

		// End loop
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		elapseTime = 1000.0*(glfwGetTime() - startTime);
	}

	//	end
	std::cout << "ending game" << std::endl;
	InstanceManager::getInstance()->clearGarbage();
	ResourceManager::getInstance()->clearGarbage();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
//

//	functions implementation
static void errorCallback(int error, const char* description) { std::cerr << "GLFW ERROR : " << description << std::endl; }
GLFWwindow* initGLFW()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwSetErrorCallback(errorCallback);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow*window = glfwCreateWindow(640, 480, "Golem Factory v1.0", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	return window;
}

void initializeForestScene()
{
	// blue sky & green grass!!
	glClearColor(0.6f, 0.85f, 0.91f, 0.f);
	Renderer::getInstance()->setGridShader(ResourceManager::getInstance()->getShader("greenGrass"));

	// init instance placement
	int fail = 0;
	std::string meshName;
	std::string shaderName;
	float sDispersion;
	float sOffset;
	HouseGenerator hg;
	srand((unsigned int)time(NULL));

	//	center tree
	InstanceDrawable* bigTree = InstanceManager::getInstance()->getInstanceDrawable();
	bigTree->setMesh("firTree1.obj");
	bigTree->setShader("tree");
	bigTree->setSize(glm::vec3(5.f, 5.f, 5.f));
	SceneManager::getInstance()->addStaticObject(bigTree);

	// village
	int vilageHouseCount = 0;
	float villageRadius[] = {20.f, 30.f, 40.f};
	std::vector<glm::vec3> houseCircle;
	for (int i = 0; i < 3; i++)
	{
		int houseCount = 5 + i * 7;
		vilageHouseCount += houseCount;
		float angleOffset = 6.28f * ((rand() % 100) / 100.f);

		for (int j = 0; j < houseCount; j++)
		{
			float radius = villageRadius[i] + 3.f * (((rand() % 100) / 50.f) - 1.f);
			float angle = angleOffset + 6.28f * j / houseCount + ((((rand() % 100) / 50.f) - 1.f)) / houseCount;
			
			glm::mat4 a(1.0);
			if(rand() % 2)
				a = glm::rotate(a, 3.14f + angle + 0.4f * ((((rand() % 100) / 50.f) - 1.f)), glm::vec3(0, 0, 1));
			else
				a = glm::rotate(a, angle + 0.4f * ((((rand() % 100) / 50.f) - 1.f)), glm::vec3(0, 0, 1));
			
			InstanceDrawable* house = dynamic_cast<InstanceDrawable*>(hg.getHouse(rand(), 20, 30));
			InstanceManager::getInstance()->add(house);
			
			glm::vec3 p = glm::vec3(radius * cos(angle), radius * sin(angle), house->getBSRadius());
			house->setOrientation(glm::rotate(glm::mat4(1.0), 1.57f + angle, glm::vec3(0, 0, 1)));

			for (unsigned int k = 0; k < houseCircle.size(); k++)
			{
				float delta = glm::length(glm::vec3(houseCircle[k].x - p.x, houseCircle[k].y - p.y, 0.f));
				if (delta < houseCircle[k].z + p.z - 0.5f)
				{
					p += (1.f + houseCircle[k].z + p.z - delta) * glm::normalize(glm::vec3(p.x - houseCircle[k].x, p.y - houseCircle[k].y, 0.f));
					k = 0;
				}
			}

			houseCircle.push_back(p);
			house->setPosition(glm::vec3(p.x, p.y, 0.f));
			SceneManager::getInstance()->addStaticObject(house);
		}
	}

	// forest
	for (int i = 0; i < GRID_SIZE; i++)
		for (int j = 0; j < GRID_SIZE; j++)
		{
			int r = rand() % 100;
			
			if (r < 20)
			{
				meshName = "rock1.obj";
				shaderName = "default";
				sDispersion = 0.4f;
				sOffset = 1.f;
			}
			else if (r < 80)
			{
				meshName = "firTree1.obj";
				shaderName = "tree";
				sDispersion = 0.2f;
				sOffset = 1.7f;
			}
			else continue;
			
			glm::vec3 p(GRID_ELEMENT_SIZE*i - (GRID_SIZE * GRID_ELEMENT_SIZE) / 2 + GRID_ELEMENT_SIZE * ((rand() % 10) / 20.f - 0.25f),
						GRID_ELEMENT_SIZE*j - (GRID_SIZE * GRID_ELEMENT_SIZE) / 2 + GRID_ELEMENT_SIZE * ((rand() % 10) / 20.f - 0.25f),
						0);

			if (glm::length(p) < villageRadius[2] + 5.f * GRID_ELEMENT_SIZE)
				continue;
				

			float s = sOffset + sDispersion * ((rand() % 100) / 50.f - 1.f);
			glm::mat4 a = glm::rotate(glm::mat4(1.0), glm::radians((rand() % 3600) / 10.f), glm::vec3(0, 0, 1));
			InstanceDrawable* ins = InstanceManager::getInstance()->getInstanceDrawable();
			if (!ins) continue;

			ins->setMesh(meshName);
			ins->setShader(shaderName);
			
			ins->setPosition(p);
			ins->setSize(glm::vec3(s,s,s));
			ins->setOrientation(a);

			if(!SceneManager::getInstance()->addStaticObject(ins))
				fail++;
		}

	//	debug
	std::cout << "Instance count : " << InstanceManager::getInstance()->getNumberOfInstances() << std::endl;
	std::cout << "House count : " << vilageHouseCount  << std::endl;
	std::cout << "Insert fail : " << fail << std::endl;
}
//

//
/*
void createBone(Skeleton* s, unsigned int joint, glm::vec3 parentPosition)
{
	InstanceDrawable* ins = InstanceManager::getInstance()->getInstanceDrawable("cube2.obj");
	if (!ins) return;

	if (joint == s->root)
	{
		ins->setSize(glm::vec3(0, 0, 0));
		ins->setPosition(parentPosition);
	}
	else
	{
		ins->setSize(glm::vec3(0, 0, 0));
		ins->setPosition(parentPosition);

		ins->setPosition(parentPosition + 0.5f * s->jointList[joint].position);
		ins->setSize(glm::vec3(0.1, 0.1, 0.48f * glm::length(s->jointList[joint].position)));
		glm::vec3 v = glm::normalize(s->jointList[joint].position);
		if (glm::dot(v, glm::vec3(0, 0, 1)) != 1.f)
			ins->setOrientation(glm::orientation(-v, glm::vec3(0, 0, 1)));
		//s->jointList[joint].orientation = glm::quat_cast(glm::orientation(-v, glm::vec3(0, 0, 1)));

		if (s->jointList[joint].sons.empty())
		{
			InstanceDrawable* leaf = InstanceManager::getInstance()->getInstanceDrawable("icosphere.obj");

			leaf->setSize(glm::vec3(0.3, 0.3, 0.3));
			leaf->setPosition(parentPosition + s->jointList[joint].position);
			SceneManager::getInstance()->addStaticObject(leaf);
		}
	}
	SceneManager::getInstance()->addStaticObject(ins);

	for (unsigned int i = 0; i < s->jointList[joint].sons.size(); i++)
		createBone(s, s->jointList[joint].sons[i], parentPosition + s->jointList[joint].position);
}
void generateRagdoll(Skeleton* s)
{
	createBone(s, s->root, glm::vec3(0, 0, 3));
}
*/
//

