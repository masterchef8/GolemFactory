#pragma once

#include "WidgetVirtual.h"

class WidgetLabel : public WidgetVirtual
{
	public:
		//	Miscellaneous
		enum AlignmentMode
		{
			MIDDLE_V = 0 << 0,
			TOP = 1 << 0,
			BOTTOM = 2 << 0,

			MIDDLE_H = 0 << 2,
			LEFT = 1 << 2,
			RIGHT = 2 << 2,

			CENTER = MIDDLE_H | MIDDLE_V,
			HORIZONTAL_MASK = 0x0C,
			VERTICAL_MASK = 0x03,
		};
		enum FlagsConfig
		{
			ITALIC = 1 << 4,
			CLIPPING = 1 << 5
		};
		//

		//  Default
		WidgetLabel(const std::string& fontName = "default", const uint8_t& config = VISIBLE, const std::string& shaderName = "defaultWidget");
		virtual ~WidgetLabel();
		//

		//	Public functions
		void update(const float& elapseTime);
		virtual void initialize(const std::string& t, uint8_t textConfiguration = AlignmentMode::CENTER);
		virtual void draw(Shader* s, uint8_t& stencilMask);
		//

		//	Set / get functions
		void setString(const std::string& newText);
		void setFont(const std::string& fontName);
		void setSizeChar(const float& f);

		std::string getString() const;
		Font* getFont() const;
		float getSizeChar() const;
		//

	protected:
		//	Protected functions
		void updateBuffers();
		void updateVBOs();
		void parseText();
		glm::vec2 getLineOrigin(const unsigned int& lineIndex, const uint8_t& textConfiguration) const;
		//

		//  Attributes
		Font* font;
		std::string text;
		std::vector<float> linesLength;
		uint8_t textConfiguration;
		float sizeChar;
		bool needUpdate;
		float updateCooldown;
		//
};