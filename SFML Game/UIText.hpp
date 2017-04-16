#ifndef UITEXT_H
#define UITEXT_H

#include <sstream>

#include "UIElement.hpp"

class UIText : public UIElement {
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

	UIText();

	void copy(UIText& copy);

	void copyTextOptions(sf::Text guide);
	void setString(std::string s);

	void setArea(sf::FloatRect a);


	void update(sf::Time deltaTime);

	void setAlignment(Align h, Align v);

	TextLines* getText();
	
	void shiftText(sf::Vector2f m);
	void setTextColor(sf::Color c);

	// not implemented yet
	// will be for adding to text without having to fully recomposite
	// void appendText(std::string);

protected:

	void updateText();

	void setPosition(sf::Vector2f pos) {};

	bool outlined = true;

	bool dirtyFlag = true;

	//not implemented yet
	//bool autoWrap = false;

	//text object and offset from area topleft
	TextLines lines;
	sf::Text styleGuide;
	std::string str;

	Align halign;
	Align valign;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif