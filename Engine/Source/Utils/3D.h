#pragma once
#include <vector>
#include <include/dll_export.h>
#include <include/glm.h>

typedef unsigned int UINT32;

namespace Utils3D
{
	DLLExport void PushQuadTriangle(std::vector<unsigned short> &indices, UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3);

	DLLExport void PushQuad(std::vector<unsigned short> &indices, UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3);

	DLLExport std::vector<glm::vec3> GetPerspectiveSection(float offsetOrigin, float FoVy, float aspectRatio);

}
