//#include <pch.h>
#include "3D.h"

namespace Utils3D {

	void PushQuad2Triangle(vector<unsigned short> &indices,
							UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3) {
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
		UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3) {
			indices.push_back(v0);
			indices.push_back(v1);
			indices.push_back(v2);
			indices.push_back(v3);
	}
}

