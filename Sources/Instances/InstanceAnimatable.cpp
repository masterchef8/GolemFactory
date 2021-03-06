#include "InstanceAnimatable.h"

//  Default
InstanceAnimatable::InstanceAnimatable(const std::string& meshName, const std::string& shaderName) : InstanceDrawable(meshName, shaderName), skeleton(nullptr), animation(nullptr)
{
	type = InstanceVirtual::ANIMATABLE;
	if (mesh && !mesh->isFromGolemFactoryFormat())
	{
		if (mesh->hasSkeleton()) skeleton = ResourceManager::getInstance()->getSkeleton(meshName);
		if (mesh->isAnimable())  animation = ResourceManager::getInstance()->getAnimation(meshName);
	}
}
InstanceAnimatable::~InstanceAnimatable()
{
	ResourceManager::getInstance()->release(mesh);
	ResourceManager::getInstance()->release(shader);
	ResourceManager::getInstance()->release(skeleton);
	ResourceManager::getInstance()->release(animation);
}
//

//	Public functions
void InstanceAnimatable::animate(float step)
{
	if (!skeleton) return;
	if (animation)
	{
		//	update all current animations
		for (std::list<AnimationTrack>::iterator it = currentAnimations.begin(); it != currentAnimations.end();)
		{
			it->jointCounter = 0;
			if (it->animate(step, this)) 
			{
				if (it->flag) std::cout << "end animation " << it->name << std::endl;
				it = currentAnimations.erase(it);
			}
			else ++it;
		}
		if (currentAnimations.empty()) return;

		//	blend all animations
		std::vector<JointPose> blendPose(skeleton->joints.size());
		for (unsigned int i = 0; i < blendPose.size(); i++)
		{
			//	check for highest priority animation track
			float pl = -1.f; float ph = -1.f;
			std::list<AnimationTrack>::iterator pl_it = currentAnimations.begin();
			std::list<AnimationTrack>::iterator ph_it = currentAnimations.begin();
			for (std::list<AnimationTrack>::iterator it = currentAnimations.begin(); it != currentAnimations.end(); ++it)
			{
				if (it->pose[i].priority > ph)
				{
					pl = ph;
					ph = it->pose[i].priority;
					pl_it = ph_it;
					ph_it = it;
				}
			}

			//	interpolate between the two highest priority animation track or just get the highest one
			if (ph - (int)ph > 0.f && pl > 0.f && ph - pl < 1.f)
			{
				blendPose[i].position = glm::mix(pl_it->pose[i].position, ph_it->pose[i].position, ph - pl);
				blendPose[i].rotation = glm::slerp(pl_it->pose[i].rotation, ph_it->pose[i].rotation, ph - pl);
				blendPose[i].scale = glm::mix(pl_it->pose[i].scale, ph_it->pose[i].scale, ph - pl);
				pl_it->jointCounter++;
				ph_it->jointCounter++;
				pl_it->uselessTime = 0.f;
				ph_it->uselessTime = 0.f;
			}
			else if(ph_it != currentAnimations.end())
			{
				blendPose[i].position = ph_it->pose[i].position;
				blendPose[i].scale = ph_it->pose[i].scale;
				blendPose[i].rotation = ph_it->pose[i].rotation;
				ph_it->jointCounter++;
				ph_it->uselessTime = 0.f;
			}
		}

		//	remove useless animation track
		for (std::list<AnimationTrack>::iterator it = currentAnimations.begin(); it != currentAnimations.end();)
		{
			if (it->jointCounter == 0 && it->uselessTime >= 1.f) it = currentAnimations.erase(it);
			else if (it->jointCounter == 0)
			{
				it->uselessTime += step/1000.f;
				++it;
			}
			else ++it;
		}

		//	compute matrix list pose
		std::vector<glm::mat4> blendMatrix(blendPose.size(), glm::mat4(1.f));
		for (unsigned int i = 0; i < skeleton->roots.size(); i++)
			computePose(blendMatrix, blendPose, glm::mat4(1.f), skeleton->roots[i]);

		//swap
		locker.lock();
		pose = blendMatrix;
		locker.unlock();
	}
	else
	{
		locker.lock();
		pose = skeleton->getBindPose();
		locker.unlock();
	}
}
void InstanceAnimatable::launchAnimation(const std::string& labelName, const bool& flaged)
{
	if (!animation || !skeleton) return;

	//	create and add a new track to current animation
	std::map<std::string, KeyLabel>::iterator it = animation->labels.find(labelName);
	if (it != animation->labels.end())
	{
		AnimationTrack at(skeleton->joints.size(), labelName);
			at.start = it->second.start;
			at.stop = it->second.stop;
			at.exit = it->second.exit_key;
			at.loop = it->second.loop;
			at.flag = flaged;
			at.previous = it->second.entry_key;
			at.next = at.previous + 1;
		currentAnimations.insert(currentAnimations.end(), at);
	}
}
void InstanceAnimatable::stopAnimation(const std::string& labelName)
{
	for (std::list<AnimationTrack>::iterator it = currentAnimations.begin(); it != currentAnimations.end(); ++it)
		if (it->name == labelName)
			it->loop = false;
}
//

//	Set/get functions
bool InstanceAnimatable::isAnimationRunning(const std::string& animationName)
{
	for (std::list<AnimationTrack>::iterator it = currentAnimations.begin(); it != currentAnimations.end(); ++it)
		if (it->name == animationName && it->uselessTime <= 0.f) return true;
	return false;
}

void InstanceAnimatable::setAnimation(std::string animationName)
{
	currentAnimations.clear();
	ResourceManager::getInstance()->release(animation);
	animation = ResourceManager::getInstance()->getAnimation(animationName);

	locker.lock();
	if (animation && skeleton && pose.empty())
		pose = animation->getKeyPose(0, skeleton->roots, skeleton->joints);
	locker.lock();
}
void InstanceAnimatable::setAnimation(Animation* a)
{
	currentAnimations.clear();
	ResourceManager::getInstance()->release(animation);
	if (a) animation = ResourceManager::getInstance()->getAnimation(a->name);
	else animation = nullptr;

	locker.lock();
	if (animation && skeleton && pose.empty())
		pose = animation->getKeyPose(0, skeleton->roots, skeleton->joints);
	locker.lock();
}

void InstanceAnimatable::setSkeleton(std::string skeletonName)
{
	ResourceManager::getInstance()->release(skeleton);
	skeleton = ResourceManager::getInstance()->getSkeleton(skeletonName);

	locker.lock();
	if (animation && skeleton && pose.empty())
		pose = animation->getKeyPose(0, skeleton->roots, skeleton->joints);
	locker.lock();
}
void InstanceAnimatable::setSkeleton(Skeleton* s)
{
	ResourceManager::getInstance()->release(skeleton);
	if (s) skeleton = ResourceManager::getInstance()->getSkeleton(s->name);
	else skeleton = nullptr;

	locker.lock();
	if (animation && skeleton && pose.empty())
		pose = animation->getKeyPose(0, skeleton->roots, skeleton->joints);
	locker.lock();
}

Skeleton* InstanceAnimatable::getSkeleton() const { return skeleton; }
Animation* InstanceAnimatable::getAnimation() const { return animation; }
std::vector<glm::mat4> InstanceAnimatable::getPose() const { return pose; }
glm::vec3 InstanceAnimatable::getJointPosition(const std::string& jointName)
{
	locker.lock();
	bool b = pose.empty();
	locker.lock();
	
	if (!skeleton || b) return  glm::vec3(0.f);
	int index = -1;
	for (unsigned int i = 0; i < skeleton->joints.size(); i++)
		if (skeleton->joints[i].name == jointName)
		{
			index = i;
			break;
		}
	if (index < 0) return  glm::vec3(0.f);

	locker.lock();
	glm::vec3 p = position + size.x * glm::vec3(pose[index][3][0], pose[index][3][1], pose[index][3][2]);
	locker.lock();
	return p;
}
//

//	Private functions
void InstanceAnimatable::computePose(std::vector<glm::mat4>& result, const std::vector<JointPose>& input, const glm::mat4& parentPose, unsigned int joint)
{
	result[joint] = parentPose * glm::translate(input[joint].position) * glm::toMat4(input[joint].rotation) * glm::scale(input[joint].scale);
	for (unsigned int i = 0; i < skeleton->joints[joint].sons.size(); i++)
		computePose(result, input, result[joint], skeleton->joints[joint].sons[i]);
}
//

//	Miscellaneous
InstanceAnimatable::AnimationTrack::AnimationTrack(const unsigned int& poseSize, const std::string& n)
: name(n), start(0), stop(0), exit(0), previous(0), next(0), time(0.f), uselessTime(0.f), loop(false), flag(false), jointCounter(0)
{
	pose.assign(poseSize, JointPose());
}
bool InstanceAnimatable::AnimationTrack::animate(const float& step, const InstanceAnimatable* const parent)
{
	//	Increment time and create aliases
	time += step / 1000.f;
	const std::vector<KeyFrame>& animationSet = parent->animation->timeLine;
	float dt = animationSet[next].time - animationSet[previous].time;

	//	end of keyframes interpolation
	if (time > dt)
	{
		std::pair<int, int> bound = parent->animation->getBoundingKeyFrameIndex(animationSet[previous].time + time);
		time -= dt;
		if (loop && (bound.first >= stop || bound.first < 0))
		{
			if (loop) bound = parent->animation->getBoundingKeyFrameIndex(animationSet[start].time + time);
			else return true;
		}
		else if (!loop && (bound.first >= exit || bound.first < 0)) return true;
		previous = bound.first;
		next = bound.second;
	}

	//	interpolate joint parameters
	float t = time / (animationSet[next].time - animationSet[previous].time);
	for (unsigned int i = 0; i < pose.size(); i++)
	{
		pose[i].priority = glm::mix(animationSet[previous].poses[i].priority, animationSet[next].poses[i].priority, t);
		pose[i].position = glm::mix(animationSet[previous].poses[i].position, animationSet[next].poses[i].position, t);
		pose[i].rotation = glm::slerp(animationSet[previous].poses[i].rotation, animationSet[next].poses[i].rotation, t);
		pose[i].scale = glm::mix(animationSet[previous].poses[i].scale, animationSet[next].poses[i].scale, t);
	}
	return false;
}
//