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

	std::string getTemplateName() {
		return templateName;
	}

protected:
	std::string findTemplateName(objNode n) {
		for (std::pair<std::string, std::string> arg : n.args) {
			if (arg.first == "TARGETTEMPLATE") {
				return arg.second;
			}
		}
		return "";
	}

	void setTemplateName(std::string name) {
		templateName = name;
	}

	const bool hasTemplate() {
		return initialized;
	}

private:
	std::string templateName = "";
	objNode templateNode;

	bool initialized = false;
};


#endif // !TEMPLATEUSER_H
