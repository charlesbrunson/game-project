#ifndef UITEXT_H
#define UITEXT_H

#include <sstream>

#include "ResourceUser.hpp"
#include "UIElement.hpp"

class UIText : public UIElement, public ResourceUser {
public:
	typedef std::vector<sf::Text> TextLines;

	enum HAlign : int {
		LEFT,
		HCENTER,
		RIGHT
	};
	enum VAlign : int {
		TOP,
		VCENTER,
		BOTTOM
	};

	UIText(ResourceLoader* r);

	void copy(UIText& copy);

	void copyTextOptions(sf::Text guide);
	void setString(std::string s);

	void setArea(sf::FloatRect a);

	void updateText();

	void setAlignment(HAlign h, VAlign v);

	TextLines* getText();
	
protected:

	//text object and offset from area topleft
	TextLines lines;
	sf::Text styleGuide;
	std::string str;

	HAlign halign;
	VAlign valign;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif