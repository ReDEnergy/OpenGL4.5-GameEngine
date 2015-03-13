#pragma once
#include <vector>
#include <include/dll_export.h>

using namespace std;

typedef unsigned int UINT32;

namespace Utils3D {
	DLLExport void PushQuad(vector<unsigned short> &indices,
				  UINT32 v0, UINT32 v1, UINT32 v2, UINT32 v3);
}