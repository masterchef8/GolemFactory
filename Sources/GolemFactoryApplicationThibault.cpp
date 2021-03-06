// Golem Factory 1.0.cpp : define console application entry point
//


#include <iostream>
#include <list>
#include <time.h>
#include <sys/types.h>
#include <GL/glew.h>

#include "Utiles/System.h"
#include "Utiles/ToolBox.h"
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
#define DEBUG 0



//	global attributes
GLFWwindow* window = nullptr;
Camera camera, camera2;
bool syncronizedCamera = true;
InstanceAnimatable* avatar = nullptr;

double completeTime = 16.;
double averageCompleteTime = 16.;
//



// prototypes
GLFWwindow* initGLFW();
void initializeForestScene(bool emptyPlace = false);
std::string checkResourcesDirectory();


void initManagers();
void picking(int width, int height);
void events();
void updates(float elapseTime, int width, int height);
//


// program
int main()
{
	// init window and opengl
	window = initGLFW();
	Renderer::getInstance()->initGLEW(0);
	initManagers();

	//	Test scene
		initializeForestScene(true);

		avatar = InstanceManager::getInstance()->getInstanceAnimatable("peasant", "human", "simple_peasant", "skinning");
			float scale = 1.7f / (avatar->getBBMax() - avatar->getBBMin()).z;
			avatar->setSize(glm::vec3(scale));
			avatar->setPosition(glm::vec3(20.f, 0.f, -scale * avatar->getMesh()->aabb_min.z));
			SceneManager::getInstance()->addStaticObject(avatar);

		InstanceDrawable* bigTree = InstanceManager::getInstance()->getInstanceDrawable();
			bigTree->setMesh("firTree1.obj");
			bigTree->setShader("tree");
			bigTree->setSize(glm::vec3(5.f, 5.f, 5.f));
			SceneManager::getInstance()->addStaticObject(bigTree);


	// init loop time tracking
	double elapseTime = 16.;
	double dummy = 0;
	int width, height;

	//	game loop
	std::cout << "game loop initiated" << std::endl;
	while (!glfwWindowShouldClose(window))
	{
		// begin loop
		double startTime = glfwGetTime();
		glfwGetWindowSize(window, &width, &height);
				
		// Render scene & picking
		Renderer::getInstance()->render(&camera);
		picking(width, height);
		Renderer::getInstance()->renderHUD(&camera);

		//  handle events
		events();
		updates((float)elapseTime, width, height);

		//	clear garbages
		InstanceManager::getInstance()->clearGarbage();
		ResourceManager::getInstance()->clearGarbage();

		// End loop
		completeTime = 1000.0*(glfwGetTime() - startTime);
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
	if (!glfwInit()) exit(EXIT_FAILURE);

	glfwSetErrorCallback(errorCallback);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow*window = glfwCreateWindow(1600, 900, "Golem Factory v1.0", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	return window;
}


void initializeForestScene(bool emptyPlace)
{
	// blue sky & green grass!!
	glClearColor(0.6f, 0.85f, 0.91f, 0.f);
	Renderer::getInstance()->setShader(Renderer::GRID, ResourceManager::getInstance()->getShader("greenGrass"));

	// init instance placement
	int fail = 0;
	std::string meshName;
	std::string shaderName;
	float sDispersion;
	float sOffset;
	HouseGenerator hg;
	srand(0);// (unsigned int)time(NULL));

	//	center tree in place
	if (!emptyPlace)
	{
		InstanceDrawable* bigTree = InstanceManager::getInstance()->getInstanceDrawable();
		bigTree->setMesh("firTree1.obj");
		bigTree->setShader("tree");
		bigTree->setSize(glm::vec3(5.f, 5.f, 5.f));
		SceneManager::getInstance()->addStaticObject(bigTree);
	}

	// village
	int vilageHouseCount = 0;
	float villageRadius[] = {20.f, 30.f, 40.f};
	std::vector<glm::vec3> houseCircle;
	/*for (int i = 0; i < 3; i++)
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
			
			InstanceDrawable* house = static_cast<InstanceDrawable*>(hg.getHouse(rand(), 20, 30));
			if (house && InstanceManager::getInstance()->add(house))
			{
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
	}
	*/
	
	
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
				shaderName = "default";
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
	if (DEBUG)
	{
		std::cout << "Instance count : " << InstanceManager::getInstance()->getNumberOfInstances() << std::endl;
		std::cout << "House count : " << vilageHouseCount  << std::endl;
		std::cout << "Insert fail : " << fail << std::endl;
	}
}
std::string checkResourcesDirectory()
{
	//	check for home repository
	std::string directory = "C:/Users/Thibault/Documents/Github/GolemFactory/Resources/";
	if (ToolBox::isPathExist(directory)) return directory;

	//	check for work repository
	directory = "C:/Users/Thibault-SED/Documents/Github/GolemFactory/Resources/";
	if (ToolBox::isPathExist(directory)) return directory;
	
	//	return the default resource path for portable applications
	std::cout << "FATAL WARRNING : Fail to found ressource repo" << std::endl;
	return "Resources/";
}


void initManagers()
{
	std::string resourceRepository = checkResourcesDirectory();
	if (DEBUG) std::cout << "Found resources folder at : " << resourceRepository << std::endl;

	// Init Event handler
	EventHandler::getInstance()->addWindow(window);
	EventHandler::getInstance()->setRepository(resourceRepository);
	EventHandler::getInstance()->loadKeyMapping("RPG Key mapping");
	EventHandler::getInstance()->setCursorMode(false);
	EventHandler::getInstance()->setResizeCallback(WidgetManager::resizeCallback);

	// Init Resources manager & instance manager
	MeshLoader::logVerboseLevel = MeshLoader::ALL;
	ResourceVirtual::logVerboseLevel = ResourceVirtual::ALL;
	ResourceManager::getInstance()->setRepository(resourceRepository);
	InstanceManager::getInstance()->setMaxNumberOfInstances(1000000);

	ResourceManager::getInstance()->getShader("wired");
	ResourceManager::getInstance()->getShader("wiredSkinning");
	ResourceManager::getInstance()->getShader("skeletonDebug");

	//	Renderer
	camera.setMode(Camera::FREEFLY);
	camera.setAllRadius(2.f, 0.5f, 10.f);
	camera.setPosition(glm::vec3(0, -10, 10));
	camera2.setMode(Camera::FREEFLY);
	camera2.setAllRadius(2.f, 0.5f, 10.f);
	camera2.setPosition(glm::vec3(0, -10, 10));
	Renderer::getInstance()->setWindow(window);
	Renderer::getInstance()->initializeGrid(GRID_SIZE, GRID_ELEMENT_SIZE, glm::vec3(24 / 255.f, 202 / 255.f, 230 / 255.f));	// blue tron
	Renderer::getInstance()->setCamera(&camera2);
	Renderer::getInstance()->setShader(Renderer::GRID, ResourceManager::getInstance()->getShader("greenGrass"));

	//	HUD
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	WidgetManager::getInstance()->setInitialWindowSize(width, height);
	WidgetManager::getInstance()->loadHud("");

	// init scene
	SceneManager::getInstance()->setWorldPosition(glm::vec3(0, 0, 25));
	SceneManager::getInstance()->setWorldSize(glm::vec3(GRID_SIZE*GRID_ELEMENT_SIZE, GRID_SIZE*GRID_ELEMENT_SIZE, 50));
}
void picking(int width, int height)
{

	std::vector<std::pair<float, InstanceVirtual*> > rayList;
	SceneManager::getInstance()->getInstanceOnRay(rayList, SceneManager::INSTANCE_MESH, 1000);
	std::sort(rayList.begin(), rayList.end());

	if (!rayList.empty())
	{
		std::string type;
		switch (rayList[0].second->getType())
		{
		case InstanceVirtual::ANIMATABLE:	type = "animatable";	break;
		case InstanceVirtual::DRAWABLE:		type = "drawable";		break;
		case InstanceVirtual::CONTAINER:	type = "container";		break;
		default: type = "virtual"; break;
		}
		glm::vec3 p = camera.getPosition() + rayList[0].first * camera.getForward();

		WidgetManager::getInstance()->setString("interaction", "Distance : " + ToolBox::to_string_with_precision(rayList[0].first, 5) +
			" m\nPosition : (" + ToolBox::to_string_with_precision(p.x, 5) + " , " + ToolBox::to_string_with_precision(p.y, 5) + " , " + ToolBox::to_string_with_precision(p.z, 5) +
			")\nInstance on ray : " + std::to_string(rayList.size()) +
			"\nFirst instance pointed id : " + std::to_string(rayList[0].second->getId()) +
			"\n  type : " + type);

		Mesh::RenderOption option = Renderer::getInstance()->getRenderOption();
		Shader* s = Renderer::getInstance()->getShader(Renderer::INSTANCE_DRAWABLE);
		Renderer::getInstance()->setRenderOption(Mesh::BOUNDING_BOX);
		Renderer::getInstance()->setShader(Renderer::INSTANCE_DRAWABLE, ResourceManager::getInstance()->getShader("wired"));

		glm::mat4 projection = glm::perspective(glm::radians(camera.getFrustrumAngleVertical()), (float)width / height, 0.1f, 1500.f);
		Renderer::getInstance()->drawInstanceDrawable(rayList[0].second, &camera.getViewMatrix()[0][0], &projection[0][0]);

		Renderer::getInstance()->setRenderOption(option);
		Renderer::getInstance()->setShader(Renderer::INSTANCE_DRAWABLE, s);
	}
	else
	{
		WidgetManager::getInstance()->setString("interaction", "Distance : (inf)\nPosition : ()\nInstance on ray : 0\nFirst instance pointed id : (null)\n ");
	}
}
void events()
{
	EventHandler::getInstance()->handleEvent();
	std::vector<UserEventType> v;
	EventHandler::getInstance()->getFrameEvent(v);
	for (unsigned int i = 0; i < v.size(); i++)
	{
		//	micselenious
		if (v[i] == QUIT) glfwSetWindowShouldClose(window, GL_TRUE);
		else if (v[i] == CHANGE_CURSOR_MODE) EventHandler::getInstance()->setCursorMode(!EventHandler::getInstance()->getCursorMode());
		else if (v[i] == ACTION)
		{
			if (camera.getMode() == Camera::TRACKBALL) camera.setMode(Camera::FREEFLY);
			else if (camera.getMode() == Camera::FREEFLY) camera.setMode(Camera::TRACKBALL);
		}

		//	avatar related
		else if (v[i] == SLOT1) avatar->launchAnimation("hello");
		else if (v[i] == SLOT2) avatar->launchAnimation("yes");
		else if (v[i] == SLOT3) avatar->launchAnimation("no");
		else if (v[i] == FORWARD)
		{
			if (EventHandler::getInstance()->isActivated(FORWARD) && camera.getMode() == Camera::TRACKBALL)
			{
				if (EventHandler::getInstance()->isActivated(RUN)) avatar->launchAnimation("run");
				else avatar->launchAnimation("walk");
			}
			else
			{
				avatar->stopAnimation("run");
				avatar->stopAnimation("walk");
			}
		}
		else if (v[i] == RUN)
		{
			if (EventHandler::getInstance()->isActivated(RUN))
			{
				if (EventHandler::getInstance()->isActivated(FORWARD) && camera.getMode() == Camera::TRACKBALL) avatar->launchAnimation("run");
				else avatar->stopAnimation("run");
			}
			else
			{
				avatar->stopAnimation("run");
				if (EventHandler::getInstance()->isActivated(FORWARD) && camera.getMode() == Camera::TRACKBALL) avatar->launchAnimation("walk");
			}
		}

		//	debug action
		else if (v[i] == F12) syncronizedCamera = !syncronizedCamera;
		else if (v[i] == F9) WidgetManager::getInstance()->setActiveHUD((WidgetManager::getInstance()->getActiveHUD() == "debug" ? "" : "debug"));
		else if (v[i] == F3)
		{
			if (Renderer::getInstance()->getRenderOption() == Mesh::DEFAULT)
			{
				Renderer::getInstance()->setRenderOption(Mesh::BOUNDING_BOX);
				Renderer::getInstance()->setShader(Renderer::INSTANCE_DRAWABLE, ResourceManager::getInstance()->getShader("wired"));
				WidgetManager::getInstance()->append("console", "wired");
			}
			else
			{
				Renderer::getInstance()->setRenderOption(Mesh::DEFAULT);
				Renderer::getInstance()->setShader(Renderer::INSTANCE_DRAWABLE, nullptr);
				WidgetManager::getInstance()->append("console", "default");
			}
		}
	}
}
void updates(float elapseTime, int width, int height)
{
	//	animate avatar
	avatar->animate(elapseTime);

	//	Compute HUD picking parameters
	if (EventHandler::getInstance()->getCursorMode())
	{
		glm::vec2 cursor = EventHandler::getInstance()->getCursorNormalizedPosition();
		glm::vec4 ray_eye = glm::inverse(glm::perspective(glm::radians(ANGLE_VERTICAL_HUD_PROJECTION), (float)width / height, 0.01f, 150.f)) * glm::vec4(cursor.x, cursor.y, -1.f, 1.f);
		WidgetManager::getInstance()->setPickingParameters(
			camera.getViewMatrix() * glm::translate(glm::mat4(1.f), DISTANCE_HUD_CAMERA * camera.getForward()) * camera.getModelMatrix(),
			glm::normalize(glm::vec3(ray_eye.x, ray_eye.y, ray_eye.z)),
			camera.getPosition());
	}
	else
	{
		WidgetManager::getInstance()->setPickingParameters(glm::mat4(1.f), glm::vec3(0.f), camera.getPosition());
	}


	//	Update widgets
	averageCompleteTime = 0.99f * averageCompleteTime + 0.01f * completeTime;
	WidgetManager::getInstance()->setString("runtime speed",
		"FPS : " + std::to_string((int)(1000.f / completeTime)) + "\navg : " + std::to_string((int)(1000.f / averageCompleteTime)) +
		"\n\nTime : " + ToolBox::to_string_with_precision(completeTime) + " ms\navg : " + ToolBox::to_string_with_precision(averageCompleteTime) + " ms");
	WidgetManager::getInstance()->setString("drawcalls",
		"Instances :\n  " + std::to_string(Renderer::getInstance()->getNbDrawnInstances() + WidgetManager::getInstance()->getNbDrawnWidgets()) +
		"\n\nTriangles :\n  " + std::to_string(Renderer::getInstance()->getNbDrawnTriangles() + WidgetManager::getInstance()->getNbDrawnTriangles()));
	WidgetManager::getInstance()->update((float)elapseTime, EventHandler::getInstance()->isActivated(USE1));

	//	Move avatar if needed
	if (camera.getMode() == Camera::TRACKBALL)
	{
		glm::vec3 forward = camera.getForward();
		float speed = 0.f;
		if (avatar->isAnimationRunning("walk")) speed = 0.025f;
		if (avatar->isAnimationRunning("run")) speed = 0.1f;
		avatar->setPosition(avatar->getPosition() + speed * glm::vec3(forward.x, forward.y, 0.f));
		if (speed != 0.f) avatar->setOrientation(glm::rotate(glm::pi<float>() / 2.f + atan2(forward.y, forward.x), glm::vec3(0.f, 0.f, 1.f)));
	}

	//Animate camera
	if (camera.getMode() == Camera::TRACKBALL) camera.setTarget(avatar->getJointPosition("Head"));
	camera.animate((float)elapseTime,
		EventHandler::getInstance()->isActivated(FORWARD), EventHandler::getInstance()->isActivated(BACKWARD),
		EventHandler::getInstance()->isActivated(LEFT), EventHandler::getInstance()->isActivated(RIGHT),
		EventHandler::getInstance()->isActivated(RUN), EventHandler::getInstance()->isActivated(SNEAKY));
	if (camera.getMode() == Camera::TRACKBALL)
		camera.setRadius(camera.getRadius() + camera.getSensitivity() * EventHandler::getInstance()->getScrollingRelative().y);
	else
	{
		float angle = camera.getFrustrumAngleVertical() + EventHandler::getInstance()->getScrollingRelative().y;
		if (angle > 70.f) angle = 70.f;
		else if (angle < 3) angle = 3.f;
		camera.setFrustrumAngleVertical(angle);
		camera.setFrustrumAngleHorizontalFromScreenRatio((float)width / height);
	}
	if (syncronizedCamera) camera2 = camera;
}
//
