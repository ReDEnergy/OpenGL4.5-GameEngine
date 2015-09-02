#pragma once
#include <vector>
#include <include/dll_export.h>
#include <include/glm.h>

using namespace std;

namespace OpenGL
{
	DLLExport void DrawCircle(float cx, float cy, float r, int num_segments);
	DLLExport void DrawLine(glm::vec3 posA, glm::vec3 posB);
	DLLExport void DrawPolygon(vector<glm::vec3> &vertices);
}