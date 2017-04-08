#include "UIButton.hpp"

const sf::Vector2f UIButton::buttonSize(64.f, 16.f);

Animation UIButton::anim_button_notselected(
	sf::IntRect(0, 0, UIButton::buttonSize.x, UIButton::buttonSize.y), 
	sf::Vector2f(0.f, 0.f), 
	1, sf::Time::Zero);

Animation UIButton::anim_button_selected(
	sf::IntRect(0, UIButton::buttonSize.y, UIButton::buttonSize.x, UIButton::buttonSize.y), 
	sf::Vector2f(0.f, 0.f),
	{25,25,25,25,25,25,25,25,25,25,25,25,700});

Animation UIButton::anim_button_activated(
	sf::IntRect(0, UIButton::buttonSize.y * 2.f, UIButton::buttonSize.x, UIButton::buttonSize.y), 
	sf::Vector2f(0.f, 0.f), 
	1, sf::Time::Zero);

UIButton::UIButton(std::string buttonLabel) {
	spr.getSprite()->setTexture(RL()->getTexture("button.png"));
	spr.setAnimation(anim_button_notselected);

	interactive = true;
	delayedActivation = true;

	//button text
	sf::Text text;
	text.setFont(RL()->getFont("pixelated.ttf"));
	text.setCharacterSize(8);
	text.setFillColor(sf::Color::White);
	//text.setOutlineColor(sf::Color::Black);
	//text.setOutlineThickness(1.f);

	bText.copyTextOptions(text);
	bText.setString(buttonLabel);
	bText.setAlignment(UIText::CENTER, UIText::CENTER);
}

void UIButton::setActiveState(ActiveState st) {

	if (getActiveState() != st) {
		if (getActiveState() == ACTIVATED) {
			bText.shiftText(sf::Vector2f(0.f, -1.f));
		}

		if (st == NOT_SELECTED) {
			bText.setTextColor(sf::Color::White);

			if (!spr.isPlaying(anim_button_notselected))
				spr.setAnimation(anim_button_notselected);
		}
		else if (st == SELECTED) {
			bText.setTextColor(sf::Color(244, 226, 116, 255));

			if (!spr.isPlaying(anim_button_selected))
				spr.setAnimation(anim_button_selected);
		}
		else if (st == ACTIVATED) {
			bText.setTextColor(sf::Color(244, 226, 116, 255));

			bText.shiftText(sf::Vector2f(0.f, 1.f));
			if (!spr.isPlaying(anim_button_activated))
				spr.setAnimation(anim_button_activated);
		}
	}

	UIElement::setActiveState(st);

}

void UIButton::setArea(sf::FloatRect a) {
	//center button into area
	sf::Vector2f topLeft = Math::center(a) - buttonSize / 2.f;
	area = sf::FloatRect(topLeft.x, topLeft.y, buttonSize.x, buttonSize.y);

	spr.getSprite()->setPosition(topLeft);
	bText.setArea(area);
	bText.updateText();
}


void UIButton::update(sf::Time deltaTime) {
	spr.update(deltaTime);
	spr.updateFrame();
}

void UIButton::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	UIElement::draw(target, states);
	target.draw(spr, states);
	target.draw(bText, states);
}