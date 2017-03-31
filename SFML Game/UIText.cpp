#include "UIText.hpp"

#include "Math.hpp"
#include "PixelSnap.hpp"

UIText::UIText(ResourceLoader* r) : ResourceUser(r) {

}


void UIText::copy(UIText& copy) {
	styleGuide = copy.styleGuide;
	str = copy.str;
	UIElement::setArea(copy.getArea());
	halign = copy.halign;
	valign = copy.valign;
	lines = copy.lines;
}

void UIText::copyTextOptions(sf::Text guide) {
	styleGuide = guide;
}
void UIText::setString(std::string s) {
	str = s;
}

void UIText::setArea(sf::FloatRect a) {
	UIElement::setArea(a);
}

void UIText::updateText() {
	std::stringstream ss(str);
	std::string line;

	float vSpace = 0;
	const sf::FloatRect area = getArea();

	lines.clear();
	while (std::getline(ss, line, '\n')){
		lines.push_back(styleGuide);
		lines.back().setString(line);

		sf::Text *t = &lines.back();
		sf::FloatRect bounds = t->getLocalBounds();

		float xPos;

		//horizontal alignment
		if (halign == LEFT) {
			xPos = area.left;
		}
		else if (halign == HCENTER) {
			xPos = area.left + (area.width / 2.f) - (bounds.width / 2.f);
		}
		else {
			xPos = area.left + area.width - bounds.width;
		}
		vSpace += bounds.height;
		t->setPosition(sf::Vector2f(xPos, 0.f));
	}
	float vPos = 0;
	for (sf::Text& t : lines) {
		//vertical alignment
		sf::Vector2f pos = t.getPosition();
		sf::FloatRect bounds = t.getLocalBounds();
		if (valign == TOP) {
			pos.y = area.top + vPos;
		}
		else if (valign == VCENTER) {
			pos.y = area.top + (area.height / 2.f) - (vSpace / 2.f) + vPos;
		}
		else {
			pos.y = area.top + area.height - vSpace + vPos;
		}
		t.setPosition(snapToPixel(pos));
		vPos += bounds.height;
	}
}

void UIText::setAlignment(HAlign h, VAlign v) {
	halign = h;
	valign = v;
}

UIText::TextLines* UIText::getText() {
	return &lines;
}

void UIText::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	UIElement::draw(target, states);
	for (const auto& t : lines)
		target.draw(t, rMan->getShader("noalpha.frag"));
}