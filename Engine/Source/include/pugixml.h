#pragma once
#include <assert.h>
#include <pugixml/pugixml.hpp>

namespace pugi
{
	inline xml_document* LoadXML(const char* fileName)
	{
		xml_document *doc = new xml_document();
		xml_parse_result result = doc->load_file(fileName);
		if (!result) {
			printf("Error loading XLM file: %s\n", result.description());
			assert(false);
		}
		return doc;
	}

	inline xml_node CreateNode(const char* name, const char* value, xml_node &parent)
	{
		auto node = parent.append_child(name);
		node.append_child(pugi::node_pcdata).set_value(value);
		return node;
	}

	inline xml_node CreateNode(const char* name, unsigned int value, xml_node &parent)
	{
		auto node = parent.append_child(name);
		node.append_child(pugi::node_pcdata).set_value(std::to_string(value).c_str());
		return node;
	}
}