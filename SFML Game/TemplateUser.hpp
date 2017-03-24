#ifndef TEMPLATEUSER_H
#define TEMPLATEUSER_H

#include "Node.hpp"

class TemplateUser {
public:
	void setTemplateNode(objNode node) {
		templateNode = node;
		templateName = node.name;
		initialized = true;
	};

	objNode& getTemplateNode() {
		return templateNode;
	}

	sf::String getTemplateName() {
		return templateName;
	}

protected:
	sf::String findTemplateName(objNode n) {
		for (std::pair<sf::String, sf::String> arg : n.args) {
			if (arg.first == "TARGETTEMPLATE") {
				return arg.second;
			}
		}
		return "";
	}

	void setTemplateName(sf::String name) {
		templateName = name;
	}

	const bool hasTemplate() {
		return initialized;
	}

private:
	sf::String templateName = "";
	objNode templateNode;

	bool initialized = false;
};


#endif // !TEMPLATEUSER_H
