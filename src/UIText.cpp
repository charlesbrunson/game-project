#include "UIText.hpp"

#include "Math.hpp"
#include "PixelSnap.hpp"

#include "ResourceLoader.hpp"

UIText::UIText() {
	//interactive = true;
	//delayedActivation = true;
}

void UIText::update(sf::Time deltaTime) {
	if (dirtyFlag)
		updateText();
}

void UIText::copy(UIText& copy) {
	styleGuide = copy.styleGuide;
	str = copy.str;
	area = copy.getArea();
	halign = copy.halign;
	valign = copy.valign;
	lines = copy.lines;
	dirtyFlag = true;
}

void UIText::copyTextOptions(sf::Text guide) {
	styleGuide = guide;
	dirtyFlag = true;
}
void UIText::setString(std::string s) {
	dirtyFlag |= str != s;
	str = s;
}

void UIText::setArea(sf::FloatRect a) {
	dirtyFlag |= area != a;
	area = a;
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
		if (halign == START) {
			xPos = area.left;
		}
		else if (halign == CENTER) {
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
		if (valign == START) {
			pos.y = area.top + vPos;
		}
		else if (valign == CENTER) {
			pos.y = area.top + (area.height / 2.f) - (vSpace / 2.f) + vPos;
		}
		else {
			pos.y = area.top + area.height - vSpace + vPos;
		}
		t.setPosition(snapToPixel(pos + sf::Vector2f(1.f, -2.f)));
		vPos += bounds.height;
	}
	dirtyFlag = false;
}
void UIText::shiftText(sf::Vector2f m) {
	for (sf::Text& t : lines) {
		t.setPosition(t.getPosition() + m);
	}
}
void UIText::setTextColor(sf::Color c) {
	styleGuide.setFillColor(c);
	for (sf::Text& t : lines) {
		t.setFillColor(c);
	}
}

void UIText::setAlignment(Align h, Align v) {
	if (halign != h || valign != v) {
		halign = h;
		valign = v;
		dirtyFlag = true;
	}
}

UIText::TextLines* UIText::getText() {
	return &lines;
}

void UIText::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	UIElement::draw(target, states);
	for (const auto& t : lines) {

#if _DEBUG
		/*
		if (UI_Globals::Debug::UIDebug) {
			sf::RectangleShape rect(sf::Vector2f(t.getGlobalBounds().width, t.getGlobalBounds().height));
			rect.setPosition(sf::Vector2f(t.getGlobalBounds().left, t.getGlobalBounds().top));
			rect.setFillColor(sf::Color(255, 255, 255, 128));
			target.draw(rect, states);
		}
		*/
#endif
		if (outlined)
			target.draw(t, RL()->getShader("shaders/noalpha.frag"));
		else
			target.draw(t, states);
	}
}