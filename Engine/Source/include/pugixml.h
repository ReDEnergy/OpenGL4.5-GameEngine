#pragma once
#include <assert.h>
#include <pugixml/pugixml.hpp>

namespace pugi {
	inline xml_document* LoadXML(const char* fileName) {
		xml_document *doc = new xml_document();
		xml_parse_result result = doc->load_file(fileName);
		if (!result) {
			printf("Error loading XLM file: %s\n", result.description());
			assert(false);
		}
		return doc;
	}
}