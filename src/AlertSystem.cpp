#include "AlertSystem.hpp"

#include <iostream>

std::queue<AlertSystem::Alert> AlertSystem::alertBuffer;

void AlertSystem::pushAlert(Alert a) {
	alertBuffer.push(a);
}

void AlertSystem::pushAlert(Message m) {
	alertBuffer.push(Alert(m));
}

AlertSystem::Alert AlertSystem::popAlert() {
	Alert a = alertBuffer.front();
	alertBuffer.pop();
	return a;
}

void AlertSystem::clearAlerts() {
	while (!alertBuffer.empty())
		alertBuffer.pop();
}

bool AlertSystem::hasAlerts() {
	return !alertBuffer.empty();
}