#ifndef LOG_H
#define LOG_H

#include <string>
#include <list>
#include <map>
#include <set>

class LogListener {
public:

	LogListener();
	virtual ~LogListener();

	void messagesChanged() {
		msgDirty = true;
	};
	void trackersChanged() {
		trackDirty = true;
	};

protected:
	bool msgDirty = true;
	bool trackDirty = true;
};

//message log
class Log {
public:
	static void msg(const std::string& m);

	//send message to log
	template <class T, typename std::enable_if<std::is_fundamental<T>::value, T>::type>
	static void msg(const T& t)
	{
		msg(std::to_string(t) + "\n");
	};

	template<size_t N>
	static void msg(const char(&charArray)[N]) {
		msg(std::string(charArray));
	};

	//value tracking
	static void track(const std::string& label, const std::string& val);
	static void remove(const std::string& label);

	static std::string getVal(std::string label);
	static void updateValue(std::string label, std::string nVal);

	static const std::list<std::string>& getMessages();
	static const std::map<std::string, std::string>& getTracked();

	static void addListener(LogListener* l);
	static void removeListener(LogListener* l);

private:

	static void message(const std::string& m);

	static std::list<std::string> messages;
	static std::map<std::string, std::string> trackedValues;
	static std::set<LogListener*> listeners;
	static const int messagesMax = 6;
};

#endif