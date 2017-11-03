#include "FontManager.h"

#include <string>

#include "Manager.h"
#include <Manager/DebugInfo.h>
#include <Core/GameObject.h>

#include <include/gl.h>
#include <Config/ResourcePath.h>

#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/text-buffer.h>
#include <freetype-gl/vec234.h>
#include <freetype-gl/texture-font.h>
#include <freetype-gl/vertex-buffer.h>
#include <freetype-gl/edtaa3func.h>

#include <FreeType/ft2build.h>
#include FT_FREETYPE_H

using namespace std;

FontManager::FontManager() {
	fontAtlasID = 0;
}

FontManager::~FontManager() {

}

void FontManager::Init() {
	Manager::Debug->InitManager("Font");

	 /* Text to be printed */
	const char *cache = " !\"#$%&'()*+,-./0123456789:;<=>?"
                           "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                           "`abcdefghijklmnopqrstuvwxyz{|}~";

	/* Texture atlas to store individual glyphs */
	auto fatlas = texture_atlas_new( 512, 512, 1 );

	/* Build a new texture font from its description and size */
	auto font = texture_font_new_from_file(fatlas, 48, (RESOURCE_PATH::FONTS + "Roboto/Roboto-Regular.ttf").c_str());
	fontInfo = (void*)font;
	
	/* Cache some glyphs to speed things up */
	texture_font_load_glyphs( font, cache);
	// texture_font_delete( font );

	unsigned char *map;
    map = MakeDistanceMap(fatlas->data, static_cast<uint32_t>(fatlas->width), static_cast<uint32_t>(fatlas->height));
    memcpy(fatlas->data, map, fatlas->width * fatlas->height * sizeof(unsigned char) );
    free(map);

	texture_atlas_upload(fatlas);
	fontAtlasID = fatlas->id;
}

unsigned int FontManager::GetFontAtlasID() const
{
	return fontAtlasID;
}

void* FontManager::GetTextureFont() const
{
	return fontInfo;
}

unsigned char * FontManager::MakeDistanceMap( unsigned char *img, unsigned int width, unsigned int height )
{
	short * xdist = (short *)  malloc( width * height * sizeof(short) );
	short * ydist = (short *)  malloc( width * height * sizeof(short) );
	double * gx   = (double *) calloc( width * height, sizeof(double) );
	double * gy      = (double *) calloc( width * height, sizeof(double) );
	double * data    = (double *) calloc( width * height, sizeof(double) );
	double * outside = (double *) calloc( width * height, sizeof(double) );
	double * inside  = (double *) calloc( width * height, sizeof(double) );
	unsigned int i;
	
	// Convert img into double (data)
	double img_min = 255, img_max = -255;
	for( i=0; i<width*height; ++i)
	{
		double v = img[i];
		data[i] = v;
		if (v > img_max) img_max = v;
		if (v < img_min) img_min = v;
	}
	// Rescale image levels between 0 and 1
	for( i=0; i<width*height; ++i)
	{
		data[i] = (img[i]-img_min)/img_max;
	}
	
	// Compute outside = edtaa3(bitmap); % Transform background (0's)
	computegradient( data, width, height, gx, gy);
	edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
	for( i=0; i<width*height; ++i)
		if( outside[i] < 0 )
			outside[i] = 0.0;
	
	// Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
	memset(gx, 0, sizeof(double)*width*height );
	memset(gy, 0, sizeof(double)*width*height );
	for( i=0; i<width*height; ++i)
		data[i] = 1 - data[i];
	computegradient( data, width, height, gx, gy);
	edtaa3(data, gx, gy, width, height, xdist, ydist, inside);
	for( i=0; i<width*height; ++i)
		if( inside[i] < 0 )
			inside[i] = 0.0;
	
	// distmap = outside - inside; % Bipolar distance field
	unsigned char *out = (unsigned char *) malloc( width * height * sizeof(unsigned char) );
	for( i=0; i<width*height; ++i)
	{
		outside[i] -= inside[i];
		outside[i] = 128+outside[i]*16;
		if( outside[i] < 0 ) outside[i] = 0;
		if( outside[i] > 255 ) outside[i] = 255;
		out[i] = 255 - (unsigned char) outside[i];
		//out[i] = (unsigned char) outside[i];
	}
	
	free( xdist );
	free( ydist );
	free( gx );
	free( gy );
	free( data );
	free( outside );
	free( inside );
	return out;
}
