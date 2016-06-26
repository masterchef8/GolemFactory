#include "InstanceDrawable.h"


//  Default
InstanceDrawable::InstanceDrawable(std::string meshName) : InstanceVirtual()
{
	mesh = ResourceManager::getInstance()->getMesh(meshName);
}
InstanceDrawable::~InstanceDrawable()
{
	ResourceManager::getInstance()->release(mesh);
}
//

//	Public functions
void InstanceDrawable::draw(Shader* shaderToUse)
{
	if (!shaderToUse || !mesh) return;

	glm::mat4 model = glm::translate(glm::mat4(1.0),position);
	model = glm::scale(model, size);
	model = glm::rotate(model, glm::radians(90.f), glm::vec3(1, 0, 0));

	shaderToUse->loadUniformMatrix('m',&model[0][0]);
	mesh->draw();
}
//