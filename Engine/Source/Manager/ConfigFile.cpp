//#include <pch.h>
#include "ConfigFile.h"

#include <include/glm_utils.h>
#include <include/pugixml.h>

static const string _PATH("Config/");

pugi::xml_node *config;
pugi::xml_document *doc;

ConfigFile::ConfigFile() {
}

ConfigFile::~ConfigFile() {
}

void ConfigFile::Load(const char *fileName) {

	doc = pugi::LoadXML((_PATH + "config.xml").c_str());
	config = new pugi::xml_node(doc->child("config"));

	// Read resolution
	resolution = glm::ExtractVector<glm::ivec2>(config->child_value("resolution"));
	position = glm::ExtractVector<glm::ivec2>(config->child_value("position"));

}

const char* ConfigFile::GetResourceFileLoc(const char *resourceID) {
	const char* fileName = config->child_value(resourceID);
	if (!fileName) {
		printf("<%s> entry was not specified\n", resourceID);
		assert(false);
	}

	string *file = new string(_PATH);
	*file += fileName;
	return file->c_str();
};