#include "GLog.hpp"

#include "Math.hpp"

#include <string>

#include "CamRef.hpp"

GLog::GLog(ResourceLoader *r) : ResourceUser(r) {
	//Log::addListener(this);

	messageBox.setFont(*r->getFont("pixelated.ttf"));
	messageBox.setCharacterSize(8);
	messageBox.setFillColor(sf::Color::White);
	messageBox.setOutlineColor(sf::Color::Black);
	messageBox.setOutlineThickness(1.f);

	trackerBox = messageBox;
}
GLog::~GLog() {
	//Log::removeListener(this);
}

void GLog::update(sf::Vector2f camCenter) {
	sf::FloatRect camArea = getCameraArea();

	trackersChanged();
	if (msgDirty)
		messagesChanged();

	if (camArea != curArea) {
		updateTextPosition(camArea);
	}
	
	curArea = camArea;
}


void GLog::messagesChanged() {
	std::string m;
	for (const auto& i : Log::getMessages()) {
		m += i;
	}
	//align top right
	messageBox.setString(m);
	updateTextPosition(curArea);
	msgDirty = false;
}

void GLog::trackersChanged() {
	std::string t;
	for (const auto& i : Log::getTracked()) {
		t += i.first + ": " + std::to_string(*i.second) + "\n";
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
}
	
void GLog::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(messageBox, rMan->getShader("noalpha.frag"));
	target.draw(trackerBox, rMan->getShader("noalpha.frag"));
}