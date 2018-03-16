#include "MusicPlayer.hpp"
#include "Globals.hpp"

bool MusicPlayer::playing = true;
std::list<MusicPlayer::SongRequest> MusicPlayer::playlist;

sf::Music MusicPlayer::m1;
sf::Music MusicPlayer::m2;
sf::Music *MusicPlayer::music = &m1;
bool MusicPlayer::flipper = true;

MusicPlayer::SongRequest MusicPlayer::currentSong;

sf::Time MusicPlayer::updateRate = sf::milliseconds(10);
bool MusicPlayer::loop;

void MusicPlayer::update(sf::Time deltaTime) {
	bool nextSongLoaded = false;

	music->setVolume(volumeMusic);
	getInactiveMusic()->setVolume(volumeMusic);
	if ((music->getStatus() == sf::Music::Stopped || (music->getPlayingOffset() + deltaTime >= music->getDuration() 
		&& !music->getLoop())) && MusicPlayer::playing && !playlist.empty()) {
		

		currentSong = playlist.back();
		playlist.pop_back();

		if (!nextSongLoaded) {
			//getInactiveMusic()->openFromFile(Resources::musicPaths[currentSong.musicFile]);
			getInactiveMusic()->stop();
			nextSongLoaded = false;
		}

		//wait until current music stops
		if (music->getPlayingOffset() + deltaTime >= music->getDuration()) {
			sf::sleep(music->getDuration() - music->getPlayingOffset());
		}

		if (currentSong.musicFile > -1){


			flipMusic();
			getInactiveMusic()->stop();
			music->play();
			music->setLoop(currentSong.loop);
		}
	}
	if (!nextSongLoaded && !playlist.empty() && !music->getLoop()) {
		//getInactiveMusic()->openFromFile(Resources::musicPaths[(playlist.back()).musicFile]);
		nextSongLoaded = true;
	}
}


void MusicPlayer::pause() {
	music->pause();
}
void MusicPlayer::resume() {
	if (music->getStatus() == sf::Music::Paused) {
		music->play();
	}
	if (!MusicPlayer::playing) {
		MusicPlayer::playing = true;
	}
}

void MusicPlayer::stopAll() {
	MusicPlayer::playing = false;
	music->stop();
	playlist.clear();
}
void MusicPlayer::skipCurrent() {
	music->stop();
	MusicPlayer::update(sf::Time::Zero);
}

/*
void MusicPlayer::addSong(Resources::musicNames song, bool toLoop) {
	MusicPlayer::SongRequest s;
	s.musicFile = song;
	s.loop = toLoop;
	addSong(s);
}
void MusicPlayer::addSong(MusicPlayer::SongRequest s) {
	playlist.push_front(s);
}*/
