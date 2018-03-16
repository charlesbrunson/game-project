#ifndef FILEHOTSWAP_H
#define FILEHOTSWAP_H

#include <map>
#include <mutex>

#include "res/GameFile.hpp"

class FileHotSwap {
	public:
		FileHotSwap(const std::map<std::string, GameFile*>* files, const std::mutex* mutex);

		void run();

		inline void stop() { running = false; }
		inline bool isRunning() { return running; };

	private:
		const std::map<std::string, GameFile*>* filesToSwap;
		std::map<std::string, long> lastModification;
		const std::mutex* m;
		bool running = false;

		long int getLastModicationTime(const std::string& filepath);
		sf::Time interval = sf::Time::Zero;
};

#endif
