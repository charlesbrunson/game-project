#include <iostream>

#include "Log.hpp"

std::list<std::string> Log::messages;
std::map<std::string, float*> Log::trackedNums;
std::set<LogListener*> Log::listeners;

void Log::message(const std::string& m) {
	std::cout << m;
	messages.push_back(m);
	if (messages.size() > messagesMax)
		messages.pop_front();

	for (auto itr = listeners.cbegin(); itr != listeners.cend(); itr++) {
		(*itr)->messagesChanged();
	}
}

void Log::msg(const std::string& m) {
	message(m);
}

//value tracking
void Log::trackNum(std::string label, float* num) {
	trackedNums.insert(std::make_pair(label, num));
	for (auto itr = listeners.cbegin(); itr != listeners.cend(); itr++) {
		(*itr)->trackersChanged();
	}
}

//do this before deleting any tracked number
void Log::freeNum(std::string label) {
	trackedNums.erase(label);
	for (auto itr = listeners.cbegin(); itr != listeners.cend(); itr++) {
		(*itr)->trackersChanged();
	}
}

float Log::getNum(std::string label) {
	return *trackedNums.at(label);
}

const std::list<std::string>& Log::getMessages() {
	return messages;
}

const std::map<std::string, float*>& Log::getTracked() {
	return trackedNums;
}

void Log::addListener(LogListener* l) {
	if (listeners.find(l) == listeners.end())
		listeners.insert(l);
}

void Log::removeListener(LogListener* l) {
	if (listeners.find(l) != listeners.end())
		listeners.erase(l);
}


// log listener add and remove
LogListener::LogListener() {
	Log::addListener(this);
}
LogListener::~LogListener() {
	Log::removeListener(this);
}