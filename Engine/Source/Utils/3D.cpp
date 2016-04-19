#include "3D.h"

#include <include/math.h>
#include <include/utils.h>
#include <include/glm_utils.h>

using namespace std;

namespace Utils3D
{
	void PushQuadTriangle(vector<unsigned short> &indices,
							UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3)
	{
		// first triangle
		indices.push_back(v0);
		indices.push_back(v1);
		indices.push_back(v2);
		// second triangle
		indices.push_back(v0);
		indices.push_back(v2);
		indices.push_back(v3);
	}

	DLLExport void PushQuad(vector<unsigned short> &indices,
							UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3)
	{
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);
	}

	DLLExport vector<glm::vec3> GetPerspectiveSection(float offsetOrigin, float FoVy, float aspectRatio)
	{
		// section dimensions
		float height = (float)tan(RADIANS(FoVy / 2)) * offsetOrigin;
		float width = height * aspectRatio;

		// center point
		glm::vec3 center = glm::vec3_backward * offsetOrigin;
		glm::vec3 deltaHeight = glm::vec3_up * height;
		glm::vec3 deltaWidth = glm::vec3_right * width;

		vector<glm::vec3> points(4);

		points[0] = center + deltaHeight - deltaWidth; // top left point
		points[1] = center + deltaHeight + deltaWidth; // top right point
		points[2] = center - deltaHeight + deltaWidth; // bottom right point
		points[3] = center - deltaHeight - deltaWidth; // bottom left point

		return points;
	}
}

