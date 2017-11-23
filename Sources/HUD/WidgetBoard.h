#pragma once


#include <vector>

#include "WidgetVirtual.h"


class WidgetBoard : public WidgetVirtual
{
	public:
		//  Miscellaneous
		enum CornerConfig
		{
			TOP_RIGHT = 1 << 0,
			TOP_LEFT = 1 << 1,
			BOTTOM_RIGHT = 1 << 2,
			BOTTOM_LEFT = 1 << 3
		};
		//

		//  Default
		WidgetBoard(const uint8_t& config = VISIBLE, const std::string& shaderName = "defaultWidget");
		virtual ~WidgetBoard();
		//

		//  Public functions
		void initialize(const float& borderThickness, const float& borderWidth, const uint8_t& corner = 0x00);
		void draw(Shader* s, uint8_t& stencilMask);
		void update(const float& elapseTime);
		//

	protected:
		//	Protected functions
		void updateBuffers(const bool& firstInit = false);
		void updateVBOs();
		//

		//	Attributes
		uint8_t cornerConfiguration;
		float borderWidth;
		float borderThickness;
		float updateCooldown;
		//
};