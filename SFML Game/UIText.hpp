#ifndef UITEXT_H
#define UITEXT_H

#include <sstream>

#include "ResourceUser.hpp"
#include "UIElement.hpp"

class UIText : public UIElement, public ResourceUser {
public:
	typedef std::vector<sf::Text> TextLines;

	// if horizontal, START = leftside, END = rightside
	// if vertical, START = top, END = bottom
	// could be made different depending on localization
	enum Align : int {
		START,
		CENTER,
		END
	};

	UIText(ResourceLoader* r);

	void copy(UIText& copy);

	void copyTextOptions(sf::Text guide);
	void setString(std::string s);

	void setArea(sf::FloatRect a);

	void updateText();

	void setAlignment(Align h, Align v);

	TextLines* getText();
	
protected:

	//text object and offset from area topleft
	TextLines lines;
	sf::Text styleGuide;
	std::string str;

	Align halign;
	Align valign;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif