#pragma once

/*!
 *	\file NodeVirtual.h
 *	\brief Declaration of the Event class.
 *	\author Thibault LAINE
 */

#include <iostream>
#include <algorithm>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Instances/InstanceManager.h"

/*! \class NodeVirtual
 *  \brief Base class for node implementation.
 *
 *	 A node can be represented by a cube container.
 *  For more information about scene manager node read about quadtree or octtree implementation in literature
 */
class NodeVirtual
{
	friend class SceneManager;

	public:
		//  Miscellaneous
		/*! \enum NodeDivisionFlags
		 *	\brief Used to code and decode node division on different axis.
		 */
		enum NodeDivisionFlags
		{
			X = 0xFF0000,	//!< X mask to decode bitfield
			dX = 16,		//!< X shift to code/decode bitfield
			Y = 0x00FF00,	//!< Y mask to decode bitfield
			dY = 8,			//!< Y shift to code/decode bitfield
			Z = 0x0000FF,	//!< Z mask to decode bitfield
			dZ = 0			//!< Z shift to code/decode bitfield
		};
		//

		//	Default
		/*!
		 *  \brief Constructor
		 *  \param p : the node parent
		 *  \param d : the node division byte
		 */
		NodeVirtual(NodeVirtual* p = nullptr, unsigned int d = 0x000000);

		/*!
		 *  \brief Destructor
		 *  
		 *  Release instance attached to, and merge
		 */
		virtual ~NodeVirtual();
		//

		//	Debug
		void print(int lvl);
		//

		//	Public functions
		/*!
		 *	\brief Get number of children
		 *	\return number of children, zero if node is a leaf
		 */
		int getChildrenCount() const;

		/*!
		 *	\brief Check is node is last branch
		 *	\return true if node children are leaves
		 */
		bool isLastBranch() const;

		/*!
		 *	\brief Attach instance to node
		 *	\return true if instance successfully added
		 *  
		 *  Check if instance size compatible to node size and add instance to container if yes.
		 *  If not check recursively with children.
		 */
		virtual bool addObject(InstanceVirtual* obj);

		/*!
		 *	\brief Detach instance to node
		 *	\return true if instance successfully removed
		 *
		 *  Check if instance present in personal container, and remove it if yes.
		 *  If not check recursively with children.
		 */
		virtual bool removeObject(InstanceVirtual* obj);

		/*!
		 *	\brief Compute instance list present in frustrum
		 *	\param list : the list of instance in frustrum to complete
		 */
		virtual void getInstanceList(std::vector<std::pair<int, InstanceVirtual*> >& list);

		/*!
		 *	\brief Divide (split) node depending on node division byte
		 */
		virtual void split();

		/*!
		 *	\brief Merge node (delete children)
		 */
		virtual void merge();
		//

		//Set/Get functions
		/*!
		 *  \brief Change node position
		 *  \param p : the new node position
		 */
		void setPosition(glm::vec3 p);

		/*!
		 *  \brief Change node size
		 *  \param s : the new node size
		 */
		void setSize(glm::vec3 s);

		/*!
		 *  \brief Get node position
		 *  \return the new node position.
		 */
		glm::vec3 getPosition() const;
		
		/*!
		 *  \brief Get node size
		 *  \return the new node size.
		 */
		glm::vec3 getSize() const;
		//

	protected:
		//	Protected functions
		/*!
		 *  \brief Get node level/depth in tree
		 *  \return the node level/depth.
		 */
		uint8_t getLevel() const;
		
		/*!
		 *  \brief Compute child index from target position
		 *  \param p : key position
		 *  \return child index responsible of this position, -1 if position is non valid
		 */
		int getChildrenKey(glm::vec3 p) const;
		
		/*!
		 *  \brief Check if node is in frustrum
		 *  \return the node distance to camera, or std::numeric_limits<int>::lowest if not in frustrum.
		 */
		int isInFrustrum() const;
		//

		//	Attributes
		NodeVirtual* parent;						//!< Pointer to parent node (nullptr if root)
		std::vector<NodeVirtual*> children;			//!< Children container (empty if leaf)
		glm::vec3 position;							//!< Node position in scene coordinate
		glm::vec3 size;								//!< Node size

		std::vector<InstanceVirtual*> instanceList;	//!< Instance container (list of instance attached to node)
		unsigned int division;						//!< Division byte (see NodeDivisionFlags enum to code/decode it), it's a bitfield

		InstanceDrawable* debuginstance;			//!< Debug
		//

		//	Static attributes
		static glm::vec3 camPosition;				//!< Camera position
		static glm::vec3 camDirection;				//!< Camera forward vector
		static glm::vec3 camVertical;				//!< Camera relative vertical vector
		static glm::vec3 camLeft;					//!< Camera left vector
		static float camVerticalAngle;				//!< Frustrum angle vertical
		static float camHorizontalAngle;			//!< Frustrum angle horizontal
		//
};


