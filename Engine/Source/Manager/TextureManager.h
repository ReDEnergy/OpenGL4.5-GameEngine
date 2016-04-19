#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include <include/dll_export.h>

class Texture;

class DLLExport TextureManager
{
	public:
		Texture* LoadTexture(const std::string &Path, const char *fileName);
		Texture* GetTexture(const char* name);
		Texture* GetTexture(unsigned int textureID);
		void Init();

	protected:
		TextureManager();
		~TextureManager();

	private:

		std::unordered_map<std::string, Texture*> mapTextures;
		std::vector<Texture*> vTextures;
};