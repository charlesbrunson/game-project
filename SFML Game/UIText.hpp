#ifndef UITEXT_H
#define UITEXT_H

#include "UIElement.hpp"
#include "Math.hpp"

class UIText : public UIElement, public ResourceUser {
public:
	UIText(const sf::Text &t, ResourceLoader* r) : ResourceUser(r) {
		text = t;

		interactive = true;
	}

	void setArea(sf::FloatRect a) {
		UIElement::setArea(a);
		text.setPosition(snapToPixel(Math::topleft(a)));
	}

	sf::Text* getText() {
		return &text;
	}
	
protected:

	sf::Text text;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		UIElement::draw(target, states);
		target.draw(text, rMan->getShader("noalpha.frag"));
	}
};

#endif