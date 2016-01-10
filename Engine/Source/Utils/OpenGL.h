#pragma once
#include <vector>
#include <include/dll_export.h>
#include <include/glm.h>
#include <include/utils.h>

using namespace std;

namespace OpenGL
{
	// Draws a circle in 3D space on the XOZ plane
	DLLExport void DrawCircle(const glm::vec3 &center, float radius, int num_segments);

	DLLExport void DrawCone(const glm::vec3 &center, float radius, float height, int num_segments);

	DLLExport void DrawLine(const glm::vec3 & pos);
	DLLExport void DrawLine(const glm::vec3 &posA, const glm::vec3 &posB);

	DLLExport void DrawPolygon(vector<glm::vec3> &vertices);

	DLLExport void DispatchCompute(uint sizeX, uint sizeY, uint sizeZ, uint workGroupSize, bool synchronize = true);
}