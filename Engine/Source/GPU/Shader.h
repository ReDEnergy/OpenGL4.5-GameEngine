#pragma once
#include <fstream>
#include <string>
#include <vector>

#include <include/dll_export.h>
#include <include/gl.h>

#define MAX_2D_TEXTURES 16
#define MAX_BONES		100
#define INVALID_LOC -1

using namespace std;

class DLLExport Shader
{
	public:
		Shader();
		~Shader();

		void Reload();
		void BindTexturesUnits();
		void SetShaderFiles(vector <string> shaderFiles);
		void Use() const;

	private:
		void GetUniforms();
		static unsigned int CreateShader(const string &shaderFile, GLenum shader_type);
		static unsigned int CreateProgram(const vector<unsigned int> &shaderObjects);

	public:
		GLuint program;

		// Textures
		GLint loc_textures[MAX_2D_TEXTURES];
		GLint loc_cube_textures[MAX_2D_TEXTURES];
		GLint loc_material;
		GLint loc_channel_mask;

		// MVP
		GLint loc_model_matrix;
		GLint loc_view_matrix;
		GLint loc_projection_matrix;

		// Shadow
		GLint loc_light_pos;
		GLint loc_light_color;
		GLint loc_light_radius;
		GLint loc_light_view_matrix;
		GLint loc_light_projection_matrix;
		GLint loc_shadow_texel_size;

		// Cascaded shadow mapping
		GLint CSM_LightView;
		GLint CSM_LightProjection;
		GLint CSM_SplitDistance;
		GLint CSM_cascadeID;
		GLint loc_shadowID;

		// Camera
		GLint loc_eye_pos;
		GLint loc_z_far;
		GLint loc_z_near;

		// General
		GLint loc_resolution;

		// SSAO
		GLint loc_kernel_size;
		GLint loc_kernel;
		GLint loc_u_rad;

		// TESS
		GLint loc_tess_inner_factor;
		GLint loc_tess_outer_factor;
		GLint loc_displacement_factor;
		GLint loc_lod_factor;

		// Rendering
		GLint active_ssao;
		GLint active_deferred;
		GLint active_shadow;

		// Skinning
		GLint loc_bones[MAX_BONES];

		// Text
		GLint text_color;

		// Debugging
		GLint loc_debug_view;
		GLint loc_debug_id;
		GLint loc_debug_color;

	private:
		vector<string> shaderFiles;
};