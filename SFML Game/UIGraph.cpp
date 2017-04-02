#include "UIGraph.hpp"
#include "Controls.hpp"
#include "Log.hpp"

UIGraph::UIGraph(ResourceLoader *r) : ResourceUser(r) {

}

UIGraph::~UIGraph() {
	for (const auto& i : uiStatic) {
		delete i;
	}
	for (const auto& i : uiInteractive) {
		delete i;
	}
};

UIText* UIGraph::createUIText() {
	uiStatic.push_back(new UIText(getResources()));
	return (UIText*)uiStatic.back();
}

void UIGraph::addElement(UIElement* e) {
	if (e->isInteractive()) {
		uiInteractive.push_back(e);
	}
	else {
		uiStatic.push_back(e);
	}
}


UIElement* UIGraph::findElementUnderMouse() {

	const sf::Vector2f m_pos = Controls::mousePosition;

	auto e = std::find_if(uiInteractive.begin(), uiInteractive.end(),
	[&m_pos](UIElement* e) {
		return e->isInteractive() && e->getArea().contains(m_pos);
	});

	return (e != uiInteractive.end()) ? *e : nullptr;
};

void UIGraph::update(sf::Time deltaTime) {
	//read input

	if (sElement && sElement->getActiveState() == UIElement::ActiveState::ACTIVATED)
		sElement->setActiveState(UIElement::ActiveState::SELECTED);

	//selection
	//start with mouse
	if (Controls::mouseLastMoved == sf::Time::Zero) {
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

		UIElement *old = sElement;
		changeSelection(findElementUnderMouse());

		//activate delayed button if it's still selected
		if (mousePressedLastFrame && old == sElement 
			&& sElement && sElement->isActivationDelayed()) {

			activateElement();
		}
	}


	//keyboard and joystick directional input
	static auto evalInput = [this](Cardinal dir) {

		//directions only
		if (dir < 0 || dir >= 4)
			return;

		Controls::Input input = Controls::dirMap[dir];

		if (Controls::isPressed(input)) {

			if (!sElement && lastElement)
				changeSelection(lastElement);

			if (sElement) {

				Controls::JumpActive.active = false;
				Controls::JumpActive.confirmed = false;
				Controls::confirmPress(input);

				if (!sElement->capturesDir(dir)) {
					if (sElement->connections[dir])
						changeSelection(sElement->connections[dir]);
				}
				else {
					//do a thing
					activateElement();
					sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
				}
			}
		}
	};

	evalInput(Cardinal::NORTH);
	evalInput(Cardinal::EAST);
	evalInput(Cardinal::SOUTH);
	evalInput(Cardinal::WEST);

	//activation
	//mouse
	if (sElement) {
		if (Controls::isMousePressed() && sElement->getArea().contains(Controls::mouseActive.position)) {
			Controls::confirmedMousePress();

			if (!sElement->isActivationDelayed()) 
				activateElement();

			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
		else if (Controls::isMouseHeld()) {
			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
	}

	//joystick/keyboard
	if (Controls::isPressed(Controls::JUMP)) {
		//if no element selected, revert to last selected
		if (!sElement && lastElement)
			changeSelection(lastElement);

		if (sElement) {
			Controls::confirmPress(Controls::JUMP);

			if (!sElement->isActivationDelayed()) {
				activateElement();
			}
			else {
				//store for activation on release
				lastPressed = sElement;
			}

			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
	}
	else if (sElement) {
		if (Controls::isHeld(Controls::JUMP)) {
			sElement->setActiveState(UIElement::ActiveState::ACTIVATED);
		}
		else if (inputPressedLastFrame && sElement->isActivationDelayed() && lastPressed && lastPressed == sElement) {

			//delay activation
			activateElement();
			lastPressed = nullptr;

		}
	}

	//update last valid element
	if (lastElement != sElement && sElement)
		lastElement = sElement;

	mousePressedLastFrame = Controls::mouseActive.input.active;
	inputPressedLastFrame = Controls::isHeld(Controls::JUMP);
};

void UIGraph::activateElement() {
	if (sElement && sElement->onActivate) {
		sElement->onActivate();
	}
}

void UIGraph::changeSelection(UIElement* to) {
	if (sElement)
		sElement->setActiveState(UIElement::ActiveState::NOT_SELECTED);
	sElement = to;
	if (sElement)
		sElement->setActiveState(UIElement::ActiveState::SELECTED);
};

void UIGraph::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	for (const auto &i : uiStatic) {
		target.draw(*i, states);
	}
	for (const auto &i : uiInteractive) {
		target.draw(*i, states);
	}
};

