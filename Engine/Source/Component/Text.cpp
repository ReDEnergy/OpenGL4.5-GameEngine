//#include <pch.h>
#include "Text.h"

#include <freetype-gl/freetype-gl.h>
#include <freetype-gl/text-buffer.h>
#include <freetype-gl/texture-font.h>
#include <freetype-gl/vertex-buffer.h>
#include <freetype-gl/edtaa3func.h>
#include <freetype-gl/vec234.h>

#include <FreeType/ft2build.h>
#include FT_FREETYPE_H

#include <Component/Transform.h>
#include <Component/ObjectInput.h>

#include <Manager/Manager.h>
#include <Manager/FontManager.h>

#include <GPU/Shader.h>

#include <Utils/GPU.h>


Text::Text() {
	offset = new Transform();
	transform = new Transform();
	transform->scale = glm::vec3(0.005f);
	transform->Update();
	atlasTextureID = Manager::Font->atlas->id;
}

Text::~Text() {
}

void Text::SetText(const char *text) {
	/* Add text to the buffer (see demo-font.c for the add_text code) */
	this->content.assign(text);
	InitText(); 
}

void Text::SetOffset(Transform offset) {
	
}

void Text::Render(Shader *shader) const {
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlasTextureID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(buffers->VAO);
	glDrawElementsBaseVertex(GL_TRIANGLES, nr_indices, GL_UNSIGNED_SHORT, 0, 0);

	glBindVertexArray(0);
	glDisable(GL_BLEND);
}


void Text::InitText()
{
	texture_font_t* font = Manager::Font->font;

	vector<glm::vec3> positions;
	vector<glm::vec2> text_coords;
	vector<unsigned short> indices;
	unsigned int index = 0;

	vec2 pen = {0, 0};
	vec4 bbox = {{0, 0, 0, 0}};
	size_t i;
	
	for( i=0; i<strlen(this->content.c_str()); ++i )
	{
		texture_glyph_t *glyph = texture_font_get_glyph( font, this->content[i] );
		if( glyph != NULL )
		{
			float kerning = 0.0f;
			if( i > 0) {
				kerning = texture_glyph_get_kerning(glyph, this->content[i-1]);
			}
			pen.x += kerning;
			int x0  = (int)( pen.x + glyph->offset_x );
			int y0  = (int)( pen.y + glyph->offset_y );
			int x1  = (int)( x0 + glyph->width );
			int y1  = (int)( y0 - glyph->height );
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;

			indices.push_back( index );
			indices.push_back( index + 1);
			indices.push_back( index + 2);

			indices.push_back( index );
			indices.push_back( index + 2);
			indices.push_back( index + 3);
			index += 4;

			positions.push_back(glm::vec3(x0, y0, 0));
			positions.push_back(glm::vec3(x0, y1, 0));
			positions.push_back(glm::vec3(x1, y1, 0));
			positions.push_back(glm::vec3(x1, y0, 0));

			text_coords.push_back(glm::vec2(s0, t0));
			text_coords.push_back(glm::vec2(s0, t1));
			text_coords.push_back(glm::vec2(s1, t1));
			text_coords.push_back(glm::vec2(s1, t0));

			pen.x += glyph->advance_x;
			
			// Compute bounding box
			if  (x0 < bbox.x) bbox.x = (float)x0;
			if  (y1 < bbox.y) bbox.y = (float)y1;
			if ((x1 - bbox.x) > bbox.width)  bbox.width  = x1-bbox.x;
			if ((y0 - bbox.y) > bbox.height) bbox.height = y0-bbox.y;
		}
	}

	this->nr_indices = indices.size();
	buffers = UtilsGPU::UploadData(positions, text_coords, indices);
}