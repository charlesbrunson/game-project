#include "ui/GLog.hpp"

#include "util/Math.hpp"

#include <string>

#include "game/CamRef.hpp"

#include "res/ResourceLoader.hpp"

#include "util/PixelSnap.hpp"

GLog::GLog() {
	messageBox.setFont(RL()->getFont("fonts/pixelated.ttf"));
	messageBox.setCharacterSize(8);
	messageBox.setFillColor(sf::Color::White);
	messageBox.setOutlineColor(sf::Color::Black);
	messageBox.setOutlineThickness(1.f);

	trackerBox = messageBox;
}

void GLog::update() {
	sf::FloatRect camArea = getCameraArea();

	if (trackDirty)
		fixTracker();
	if (msgDirty)
		fixMessage();

	if (camArea != curArea) {
		updateTextPosition(camArea);
	}
	
	curArea = camArea;
}


void GLog::fixMessage() {
	std::string m;
	for (const auto& i : Log::getMessages()) {
		m += i;
	}
	//align top right
	messageBox.setString(m);
	updateTextPosition(curArea);
	msgDirty = false;
}

void GLog::fixTracker() {
	std::string t;
	for (const auto& i : Log::getTracked()) {
		t += i.first + ": " + i.second + "\n";
	}
	trackerBox.setString(t);
	trackDirty = false;
}

void GLog::updateTextPosition(const sf::FloatRect& camArea) {
	//align top right
	messageBox.setPosition(Math::topright(camArea)
		- sf::Vector2f(std::max(messageBox.getGlobalBounds().width, 80.f), 0.f)
		+ sf::Vector2f(-3.f, 0.f));

	//align top left
	trackerBox.setPosition(Math::topleft(camArea)
		+ sf::Vector2f(3.f, 0.f));

	messageBox.setPosition(snapToPixel(messageBox.getPosition()));
	trackerBox.setPosition(snapToPixel(trackerBox.getPosition()));
}

void GLog::draw(sf::RenderTarget &target, sf::RenderStates) const {
	target.draw(messageBox, RL()->getShader("shaders/noalpha.frag"));
	target.draw(trackerBox, RL()->getShader("shaders/noalpha.frag"));
}
