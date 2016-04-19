#pragma once

class FontManager
{
	protected:
		FontManager();
		~FontManager();

	private:
		unsigned char* MakeDistanceMap(unsigned char *img, unsigned int width, unsigned int height);

	public:
		void Init();
		unsigned int GetFontAtlasID() const;
		void* GetTextureFont() const;

	private:
		void *fontInfo;
		unsigned int fontAtlasID;
};