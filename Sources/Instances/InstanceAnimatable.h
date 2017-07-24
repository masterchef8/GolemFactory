#pragma once

#include <glm/gtx/matrix_interpolation.hpp>

#include "InstanceDrawable.h"

class InstanceAnimatable : public InstanceDrawable
{
	friend struct AnimationTrack;

	public:
		//	Miscellaneous
		enum AnimationConfigurationFlags
		{
			PLAY = 1 << 0,
			LOOPED = 1 << 1
		};
		//

		//  Default
		InstanceAnimatable(std::string meshName, std::string shaderName);
		virtual ~InstanceAnimatable();
		//

		//	Public functions
		void animate(float step);
		void launchAnimation(const std::string& labelName, const bool& flaged = false);
		//

		//	Set/get functions
		void setSkeleton(std::string skeletonName);
		void setSkeleton(Skeleton* s);
		void setAnimation(std::string animationName);
		void setAnimation(Animation* a);

		Skeleton* getSkeleton() const;
		Animation* getAnimation() const;
		std::vector<glm::mat4> getPose();
		//

	protected:
		//	Protected functions
		void computePose(std::vector<glm::mat4>& result, const std::vector<JointPose>& input, const glm::mat4& parentPose, unsigned int joint);
		float blendingMagnitude(const unsigned int& joint);
		//
		
		//	Miscellaneous
		struct AnimationTrack
		{
			AnimationTrack(const unsigned int& poseSize, const std::string& n = "");
			bool animate(const float& step, const InstanceAnimatable* const parent);

			std::string name;
			int start, stop, previous, next;
			float time, distortion;
			bool loop, flag;
			std::vector<JointPose> pose;
		};
		//

		// Attributes
		Skeleton* skeleton;		//!< Skeleton resource pointer
		Animation* animation;	//!< Animation resource

		Mutex locker;
		std::vector<glm::mat4> pose;
		std::list<AnimationTrack> currentAnimations;
		//
};
