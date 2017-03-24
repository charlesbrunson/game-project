#include "ResourceUser.hpp"

ResourceUser::ResourceUser(ResourceLoader *rloader) {
	rMan = rloader;
};

void ResourceUser::setResources(ResourceLoader *rloader){
	rMan = rloader;
};

ResourceLoader* ResourceUser::getResources() {
	return rMan;
};