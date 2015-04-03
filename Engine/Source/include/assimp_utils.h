#pragma once
#include <include/gl.h>
#include <include/glm.h>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure

using namespace std;

namespace assimp {

	inline void CopyColor(const aiColor4D &color, glm::vec4 dest)
	{
		dest.r = color.r;
		dest.g = color.g;
		dest.b = color.b;
		dest.a = color.a;
	}

	inline void CopyMatix(const aiMatrix4x4 &mat, glm::mat4 &dest)
	{
		dest[0][0] = mat.a1; dest[0][1] = mat.a2; dest[0][2] = mat.a3; dest[0][3] = mat.a4;
		dest[1][0] = mat.b1; dest[1][1] = mat.b2; dest[1][2] = mat.b3; dest[1][3] = mat.b4;
		dest[2][0] = mat.c1; dest[2][1] = mat.c2; dest[2][2] = mat.c3; dest[2][3] = mat.c4;
		dest[3][0] = mat.d1; dest[3][1] = mat.d2; dest[3][2] = mat.d3; dest[3][3] = mat.d4;
	}


}
