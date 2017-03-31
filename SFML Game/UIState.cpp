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

UIElement* UIState::findElementUnderMouse() {

	const sf::Vector2f m_pos = Controls::mousePosition;

	auto e = std::find_if(uiElements.begin(), uiElements.end(), [&m_pos](std::pair<std::string, UIElement*> e) {
		return e.second->getArea().contains(m_pos);
	});

	return (e != uiElements.end()) ? e->second : nullptr;
};

void UIState::update(sf::Time deltaTime) {
	//read input

	if (sElement && sElement->getActiveState() == UIElement::ActiveState::ACTIVATED)
		sElement->setActiveState(UIElement::ActiveState::SELECTED);

	//selection
	//start with mouse
	if (Controls::mouseLastMoved == sf::Time::Zero /* && !Controls::isMousePressed()*/) {
		if (!Controls::mouseActive.input.active) {

			UIElement* nElement = findElementUnderMouse();

			//moving cursor to new element || moving cursor off current element
			if (Controls::mouseInWindow) {
				if ((sElement != nElement && nElement) 
					|| (!nElement && sElement)) {

					changeSelection(nElement);
				}
			}
			else if (sElement) {
				changeSelection(nullptr);
			}
		}
	}
	//mouse button changed in state, update selection
	if (Controls::mouseActive.input.active != mousePressedLastFrame 
		&& Controls::mouseInWindow) {

		changeSelection(findElementUnderMouse());
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

				Controls::JumpActive.active = false;
				Controls::JumpActive.confirmed = false;
				Controls::confirmPress(in);

				if (!sElement->capturesDir(d)) {
					if (sElement->connections[d])
						changeSelection(sElement->connections[d]);
				}
				else {
					//do a thing
					activateElement();
					sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
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
	if (sElement) {
		if (Controls::isMousePressed() && sElement->getArea().contains(Controls::mouseActive.position)) {
			Controls::confirmedMousePress();
			//Log("do a mouse thing\n");
			activateElement();
			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
		else if (Controls::isMouseHeld()) {
			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
	}

	//joystick
	if (Controls::isPressed(Controls::JUMP)) {
		if (!sElement && lastElement)
			changeSelection(lastElement);

		if (sElement) {
			Controls::confirmPress(Controls::JUMP);
			//Log("do a joystick or keyboard thing\n");
			activateElement();
			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
	}
	else if (sElement && Controls::isHeld(Controls::JUMP)) {
		sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
	}

	//update last valid element
	if (lastElement != sElement && sElement)
		lastElement = sElement;

	mousePressedLastFrame = Controls::mouseActive.input.active;
};

void UIState::activateElement() {
	if (sElement && sElement->onActivate) {
		sElement->onActivate();
	}
}

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

