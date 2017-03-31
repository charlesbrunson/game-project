#ifndef RESOURCE_USER
#define RESOURCE_USER

#include "ResourceLoader.hpp"

// Class that stores a pointer to a ResourceLoader object
class ResourceUser {
public:

	ResourceUser(ResourceLoader *rloader);

	void setResources(ResourceLoader *rloader);
	ResourceLoader* getResources();

protected:
	ResourceLoader *rMan;
};

#endif