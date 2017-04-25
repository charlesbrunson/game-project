#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <SFML/Audio.hpp>

#include <list>

class MusicPlayer {
public:
	struct SongRequest {
		int musicFile = -1;
		bool loop = true;
	};

	static void update(sf::Time deltaTime);

	static void pause();
	static void resume();

	static void stopAll();
	static void skipCurrent();

	//static void addSong(Resources::musicNames song, bool toLoop = true);
	//static void addSong(SongRequest s);
	
	static int getCurrentSongName() {
		return currentSong.musicFile;
	}

	static const bool isPlaying() {
		return playing;
	}

	static void run() {
		loop = true;

		//while (loop) {
		//	update(updateRate);
		//	sf::sleep(updateRate);
		//}
	}
	static void stopRun() {
		loop = false;
	}

private:

	static std::list<SongRequest> playlist;

	static bool flipper;
	static sf::Music m1;
	static sf::Music m2;
	static sf::Music *music;

	static void flipMusic() {
		flipper = !flipper;
		music = (flipper ? &m1: &m2);
	}

	static sf::Music* getInactiveMusic() {
		return (!flipper ? &m1 : &m2);
	}

	static SongRequest currentSong;

	static bool playing;
	static sf::Time updateRate;

	static bool loop;
};


#endif