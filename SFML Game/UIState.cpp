#include "UIState.hpp"
#include "Controls.hpp"
#include "Log.hpp"

const UIElement::Direction UIState::dirMap[4] = {
	UIElement::Direction::NORTH,
	UIElement::Direction::WEST,
	UIElement::Direction::SOUTH,
	UIElement::Direction::EAST,
};

UIState::UIState(ResourceLoader *r) : State(r) {

}

UIState::~UIState() {
	for (const auto i : uiElements) {
		delete i.second;
	}
};

void UIState::update(sf::Time deltaTime) {
	//read input

	//selection
	//start with mouse
	if (Controls::mouseLastMoved == sf::Time::Zero) {

		const sf::Vector2f m_pos = Controls::mousePosition;

		auto e = std::find_if(uiElements.begin(), uiElements.end(), [&m_pos](std::pair<std::string, UIElement*> e) {
			return e.second->getArea().contains(m_pos);
		});

		UIElement* nElement = (e != uiElements.end()) ? e->second : nullptr;

		//moving cursor to new element || moving cursor off current element
		if (Controls::mouseInWindow) {
			if ((sElement != nElement && nElement) || (!nElement && sElement)) {
				changeSelection(nElement);
			}
		}
		else if (sElement) {
			changeSelection(nullptr);
		}
	}

	//keyboard and joystick
	static auto evalInput = [this](Controls::Input in) {

		if (in < 0 || in >= 4)
			return;

		UIElement::Direction d = dirMap[in];

		if (Controls::isPressed(in)) {

			if (!sElement && lastElement)
				changeSelection(lastElement);

			if (sElement) {
				Controls::confirmPress(in);

				if (!sElement->capturesDir(d)) {
					if (sElement->connections[d])
						changeSelection(sElement->connections[d]);
				}
				else {
					//do a thing
				}
			}
		}

	};

	evalInput(Controls::UP);
	evalInput(Controls::RIGHT);
	evalInput(Controls::DOWN);
	evalInput(Controls::LEFT);

	//activation
	//mouse
	if (sElement && Controls::isMousePressed() && sElement->getArea().contains(Controls::mouseActive.position)) {
			Controls::confirmedMousePress();
			Log("do a mouse thing\n");
	}

	//joystick
	if (Controls::isPressed(Controls::JUMP)) {
		if (!sElement && lastElement)
			changeSelection(lastElement);

		if (sElement) {
			Controls::confirmPress(Controls::JUMP);
			Log("do a joystick or keyboard thing\n");
		}
	}

	//update last valid element
	if (lastElement != sElement && sElement)
		lastElement = sElement;
};

void UIState::changeSelection(UIElement* to) {
	if (sElement)
		sElement->setActiveState(UIElement::ActiveState::NOT_SELECTED);
	sElement = to;
	if (sElement)
		sElement->setActiveState(UIElement::ActiveState::SELECTED);
};

void UIState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	for (const auto &i : uiElements) {
		target.draw(*i.second, states);
	}
};

