#include "Skeleton.h"
#include "Utiles/Parser/Reader.h"

//  Static attributes
std::string Skeleton::extension = ".skeleton";
//

//	Default
Skeleton::Skeleton(const std::string& skeletonName, const std::vector<unsigned int>& rootsList, const std::vector<Joint>& jointsList, const glm::mat4& globalMatrix)
	: ResourceVirtual(skeletonName, ResourceVirtual::SKELETON), global(globalMatrix), roots(rootsList), joints(jointsList)
{
	for (unsigned int i = 0; i < roots.size(); i++)
		computeLocalBindTransform(roots[i], glm::mat4(1.f));
}

Skeleton::Skeleton(const std::string& path, const std::string& skeletonName) : ResourceVirtual(skeletonName, ResourceVirtual::SKELETON)
{
	std::cerr << "Skeleton loading in GF format not supported yet" << std::endl;
}
Skeleton::~Skeleton()
{

}
//

//	Public functions
bool Skeleton::isValid() const
{
	return roots.empty() || joints.empty();
}
//

//	Private functions
void Skeleton::computeLocalBindTransform(unsigned int joint, glm::mat4 localParentTransform)
{
	glm::mat4 m = localParentTransform * joints[joint].relativeBindTransform;
	joints[joint].inverseLocalBindTransform = glm::inverse(m);
	for (unsigned int i = 0; i < joints[joint].sons.size(); i++)
		computeLocalBindTransform(joints[joint].sons[i], m);
}
//

///	Debug
void Skeleton::debug()
{
	for (unsigned int i = 0; i < roots.size(); i++)
		printJoint(roots[i], 0);
}
void Skeleton::printJoint(unsigned int joint, int depth)
{
	for (int i = 0; i < depth; i++)
		std::cout << "  ";
	std::cout << joints[joint].name << std::endl;

	for (unsigned int i = 0; i < joints[joint].sons.size(); i++)
		printJoint(joints[joint].sons[i],depth+1);
}
//