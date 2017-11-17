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
		//

		//  Set/get functions
		void setColor(const glm::vec4& c);
		//
};