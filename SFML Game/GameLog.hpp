#ifndef GAMELOG_H
#define GAMELOG_H

#include <sstream>

#include <SFML/Graphics.hpp>

#include "ResourceUser.hpp"
#include "Log.hpp"


// Stores most recent debug messages, is also drawable
class GameLog : public ResourceUser, public sf::Drawable {
public:
	GameLog(ResourceLoader *rloader) : ResourceUser(rloader) {
		// Initialize drawable text
		logText.setFont(*rMan->getFont("pixelated.ttf"));
		logText.setCharacterSize(textSize);
		logText.setFillColor(sf::Color::White);
		logText.setOutlineColor(sf::Color::Black);
		logText.setOutlineThickness(1.f);
	}

	// Update drawable text contents, with log message queue
	void updateText() {
		std::string buffer;
		for (auto i = logQueue.begin(); i != logQueue.end(); i++) {
			buffer += *i;
		}
		logText.setString(buffer);
	}

	// Set drawable text position
	void setPosition(sf::Vector2f p) {
		logText.setPosition(p);
	}

private:
	// Debug text to be drawn to the screen
	sf::Text logText;
	// Debug text font size
	const int textSize = 8;
	
	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		target.draw(logText, rMan->getShader("noalpha.frag"));
	}
};

#endif