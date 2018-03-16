#ifndef ALERTSYSTEM_H
#define ALERTSYSTEM_H

#include <SFML/System.hpp>
#include <queue>

class AlertSystem {
	friend class GameplayState;
public:
	enum Message : int {
		ALERT_NULL,
		PLAYER_DEAD,
		GOT_ONEUP,
		GOTO_LEVEL,
		GOTO_NEXT_CHECKPOINT,
		TRANSITION_ENTER,
		TRANSITION_EXIT,
		BREAK,
		PAUSE,
		CAM_SHAKE,
		BEAT_THE_GAME
	};

	struct Alert {
		
		Alert(Message type, int num = 0, sf::Vector2f pos = sf::Vector2f()) {
			alertType = type;
			argPos = pos;
			argNum = num;
		}

		Alert(Message type, std::string argStr, sf::Vector2f pos = sf::Vector2f()) {
			alertType = type;
			argPos = pos;
			argString = argStr;
		}

		Message alertType = Message::ALERT_NULL;
		
		sf::Vector2f argPos;
		std::string argString;
		int argNum;

	};


	static void pushAlert(Alert a);
	static void pushAlert(Message m);

protected:

	static Alert popAlert();
	static void clearAlerts();
	static bool hasAlerts();

private:
	static std::queue<Alert> alertBuffer;
	
};

#endif