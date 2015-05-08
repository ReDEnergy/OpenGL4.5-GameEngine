//#include <pch.h>
#include "TextureManager.h"

#include <include/utils.h>

#include <GPU/Texture.h>

#include <Manager/Manager.h>
#include <Manager/DebugInfo.h>
#include <Manager/ResourceManager.h>

TextureManager::TextureManager() {
	Manager::Debug->InitManager("Shader");
}

void TextureManager::Init() {
	LoadTexture(RESOURCE_PATH::TEXTURES, "default.png");
	LoadTexture(RESOURCE_PATH::TEXTURES, "white.png");
	LoadTexture(RESOURCE_PATH::TEXTURES, "black.jpg");
	LoadTexture(RESOURCE_PATH::TEXTURES, "noise.png");
	LoadTexture(RESOURCE_PATH::TEXTURES, "random.jpg");
	LoadTexture(RESOURCE_PATH::TEXTURES, "explosion.bmp");
	LoadTexture(RESOURCE_PATH::TEXTURES, "particle.png");
}

TextureManager::~TextureManager() {
	// delete textures
	unsigned int size = (unsigned int) vTextures.size();
	for (unsigned int i=0; i <size; ++i)
		SAFE_FREE(vTextures[i]);
}

Texture* TextureManager::LoadTexture(const string &path, const char *fileName) {

	Texture *texture = GetTexture(fileName);

	if (texture) {
		return mapTextures[fileName];
	}

	texture = new Texture();
	bool rc = texture->Load2D((path + '\\' + fileName).c_str());

	if (rc == FALSE) {
		delete texture;
		return vTextures[0];
	}

	vTextures.push_back(texture);
	mapTextures[fileName] = texture;
	return texture;
}

Texture* TextureManager::GetTexture(const char* name) {
	if (mapTextures[name])
		return mapTextures[name];
	return NULL;
}

Texture* TextureManager::GetTexture(unsigned int textureID) {
	if (textureID < vTextures.size())
		return vTextures[textureID];
	return NULL;
}
