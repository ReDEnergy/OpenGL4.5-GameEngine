#pragma once
#ifdef ENGINE_DLL_EXPORTS
#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/text-buffer.h>
#include <freetype-gl/texture-font.h>
#endif

using namespace std;

class FontManager
{
	protected:
		FontManager();
		~FontManager();

	private:
		unsigned char* MakeDistanceMap(unsigned char *img, unsigned int width, unsigned int height);

	public:
		void Init();
		void CreateText(const wchar_t* text);
		unsigned int GetId() const;

	public:
		#ifdef ENGINE_DLL_EXPORTS
		texture_atlas_t *atlas;
		texture_font_t *font;
		#endif
};