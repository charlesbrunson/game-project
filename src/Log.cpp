#include <iostream>

#include "util/Log.hpp"

std::list<std::string> Log::messages;
std::map<std::string, std::string> Log::trackedValues;
std::set<LogListener*> Log::listeners;

void Log::message(const std::string& m) {
#ifdef _DEBUG
	std::cout << m << std::endl;
	messages.push_back(m + "\n");
	if (messages.size() > messagesMax)
		messages.pop_front();

	for (auto itr = listeners.cbegin(); itr != listeners.cend(); itr++) {
		(*itr)->messagesChanged();
	}
#endif
}

void Log::msg(const std::string& m) {
	message(m);
}

//value tracking
void Log::track(const std::string& label, const std::string& val) {
	trackedValues.insert(std::make_pair(label, val));
	for (auto itr = listeners.begin(); itr != listeners.end(); itr++) {
		(*itr)->trackersChanged();
	}
}

//do this before deleting any tracked number
void Log::remove(const std::string& label) {
	trackedValues.erase(label);
	for (auto itr = listeners.begin(); itr != listeners.end(); itr++) {
		(*itr)->trackersChanged();
	}
}

std::string Log::getVal(std::string label) {
	return trackedValues.at(label);
}

void Log::updateValue(std::string label, std::string nVal) {
	trackedValues.at(label) = nVal;
	for (auto itr = listeners.begin(); itr != listeners.end(); itr++) {
		(*itr)->trackersChanged();
	}
}

const std::list<std::string>& Log::getMessages() {
	return messages;
}

const std::map<std::string, std::string>& Log::getTracked() {
	return trackedValues;
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
