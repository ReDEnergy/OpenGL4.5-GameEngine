#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <include/dll_export.h>

using namespace std;

class Texture;

class DLLExport TextureManager
{
	public:
		Texture* LoadTexture(const string &Path, const char *fileName);
		Texture* GetTexture(const char* name);
		Texture* GetTexture(unsigned int textureID);
		void Init();

	protected:
		TextureManager();
		~TextureManager();

	private:

		unordered_map<string, Texture*> mapTextures;
		vector<Texture*> vTextures;
};