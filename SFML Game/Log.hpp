
#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <queue>
#include <SFML/System/String.hpp>


static std::vector<std::string> logQueue;

static void Log(sf::String s, bool out = true) {
#ifdef _DEBUG
	if (out) std::cout << (std::string) s;
	logQueue.insert(logQueue.end(), (std::string) s);
	if (logQueue.size() > 5) {
		logQueue.erase(logQueue.begin());
	}
#endif
}

static void Log(std::string s, bool out = true) {
#ifdef _DEBUG
	if (out) std::cout << s;
	logQueue.insert(logQueue.end(), s);
	if (logQueue.size() > 5) {
		logQueue.erase(logQueue.begin());
	}
#endif
}

template<size_t N>
static void Log(const char(&charArray)[N], bool out = true) {
#ifdef _DEBUG
	if (out) std::cout << charArray;
	logQueue.insert(logQueue.end(), charArray);
	if (logQueue.size() > 5) {
		logQueue.erase(logQueue.begin());
	}
#endif
}

static void ClearLog() {
#ifdef _DEBUG
	logQueue.clear();
#endif
}

#endif