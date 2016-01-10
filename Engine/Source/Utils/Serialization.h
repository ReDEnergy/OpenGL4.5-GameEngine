#pragma once
#include <vector>
#include <include/dll_export.h>
#include <include/glm.h>
#include <include/pugixml.h>

#include <Component/Transform/Transform.h>

using namespace std;

namespace Serialization
{
	DLLExport void ReadTransform(pugi::xml_node node, Transform &T);
}