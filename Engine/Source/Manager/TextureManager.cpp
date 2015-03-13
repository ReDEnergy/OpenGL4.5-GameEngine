//#include <pch.h>
#include "TextureManager.h"

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>
#include <GPU/Texture.h>
#include <include/util.h>

static const string T_PATH("Resources\\Models\\maps\\");

TextureManager::TextureManager() {
	Manager::Debug->InitManager("Shader");
}

void TextureManager::Init() {
	LoadTexture("default.png");
	LoadTexture("white.png");
	LoadTexture("black.jpg");
	LoadTexture("noise.png");
	LoadTexture("random.jpg");
	LoadTexture("explosion.bmp");
	LoadTexture("particle.png");
}

TextureManager::~TextureManager() {
	// delete textures
	unsigned int size = (unsigned int) vTextures.size();
	for (unsigned int i=0; i <size; ++i)
		SAFE_FREE(vTextures[i]);
}

Texture* TextureManager::LoadTexture(const char* name) {

	Texture *texture = GetTexture(name);

	if (texture) {
		#ifdef DEBUG_INFO
			fprintf(stderr, "Texture: %s already loaded\n", name);
		#endif
		return mapTextures[name];
	}

	texture = new Texture();
	bool rc = texture->Load2D((T_PATH + name).c_str());

	if (rc == FALSE) {
		delete texture;
		return vTextures[0];
	}

	vTextures.push_back(texture);
	mapTextures[name] = texture;
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
