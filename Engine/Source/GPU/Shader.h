#pragma once
#include <string>
#include <vector>
#include <list>
#include <functional>

#include <include/dll_export.h>
#include <include/gl.h>

#define MAX_2D_TEXTURES 16
#define MAX_BONES		100
#define INVALID_LOC -1

using namespace std;

class DLLExport Shader
{
	public:
		Shader(const char &name);
		~Shader();

		unsigned int Reload();
		void ClearShaders();
		void AddShader(const string &shaderFile, GLenum shaderType);
		unsigned int CreateAndLink();

		void BindTexturesUnits();
		GLint GetUniformLocation(const char * uniformName) const;

		void OnLoad(function<void()> onLoad);
		void Use() const;

	private:
		void GetUniforms();
		static unsigned int CreateShader(const string &shaderFile, GLenum shaderType);
		static unsigned int CreateProgram(const vector<unsigned int> &shaderObjects);

	public:
		GLuint program;

		// Textures
		GLint loc_textures[MAX_2D_TEXTURES];
		GLint loc_cube_textures[MAX_2D_TEXTURES];
		GLint loc_channel_mask;

		// Material
		GLint loc_material;
		GLint loc_transparency;

		// MVP
		GLint loc_model_matrix;
		GLint loc_view_matrix;
		GLint loc_projection_matrix;

		// Shadow
		GLint loc_light_pos;
		GLint loc_light_color;
		GLint loc_light_radius;
		GLint loc_light_direction;
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
		GLint loc_eye_forward;
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
		GLint active_shadows;
		GLint active_selection;

		// Skinning
		GLint loc_bones[MAX_BONES];
		GLint loc_animated;

		// Text
		GLint text_color;

		// Debugging
		GLint loc_debug_view;
		GLint loc_debug_id;
		GLint loc_debug_color;

	private:

		struct ShaderFile {
			string file;
			GLenum type;
		};

		string shaderName;
		vector<ShaderFile> shaderFiles;
		list<function<void()>> loadObservers;

		// TODO - add support for user defined uniforms
		// unordered_map<string, GLint> uniforms;
};