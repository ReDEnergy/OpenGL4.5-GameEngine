//#include <pch.h>
#include "Shader.h"


Shader::Shader() {
	program = 0;
	shaderFiles.reserve(5);
}

Shader::~Shader()
{
	glDeleteProgram(program);
}

void Shader::BindTexturesUnits() {
	glUseProgram(program);
	for (int i = 0; i < MAX_2D_TEXTURES; i++) {
		if (loc_textures[i] >= 0)
			glUniform1i(loc_textures[i], i);
		if (loc_cube_textures[i] >= 0)
			glUniform1i(loc_cube_textures[i], i);
	}
}

void Shader::GetUniforms() {

	// MVP
	loc_model_matrix	= glGetUniformLocation(program, "Model");
	loc_view_matrix		= glGetUniformLocation(program, "View");
	loc_projection_matrix = glGetUniformLocation(program, "Projection");

	// Lighting and Shadow
	loc_light_pos = glGetUniformLocation(program, "light_position");
	loc_light_color = glGetUniformLocation(program, "light_color");
	loc_light_radius = glGetUniformLocation(program, "light_radius");
	loc_light_view_matrix = glGetUniformLocation(program, "LightView");
	loc_light_projection_matrix = glGetUniformLocation(program, "LightProjection");

	CSM_LightView  = glGetUniformLocation(program, "CSM_LightView");
	CSM_LightProjection = glGetUniformLocation(program, "CSM_LightProjection");
	CSM_SplitDistance = glGetUniformLocation(program, "CSM_split_distance");
	CSM_cascadeID = glGetUniformLocation(program, "CSM_cascadeID");

	loc_shadowID = glGetUniformLocation(program, "shadowID");
	loc_shadow_texel_size = glGetUniformLocation(program, "shadow_texel_size");

	// Camera
	loc_eye_pos = glGetUniformLocation(program, "eye_position");
	loc_z_far = glGetUniformLocation(program, "zFar");
	loc_z_near = glGetUniformLocation(program, "zNear");

	// General
	loc_resolution = glGetUniformLocation(program, "resolution");

	// SSAO
	loc_kernel_size = glGetUniformLocation(program, "kernel_size");	
	loc_kernel = glGetUniformLocation(program, "kernel");	
	loc_u_rad = glGetUniformLocation(program, "u_rad");	

	// TESS
	loc_lod_factor = glGetUniformLocation(program, "lod_factor");
	loc_tess_inner_factor = glGetUniformLocation(program, "tess_inner_factor");
	loc_tess_outer_factor = glGetUniformLocation(program, "tess_outer_factor");
	loc_displacement_factor = glGetUniformLocation(program, "displacement_factor");

	// Composition settings
	active_ssao = glGetUniformLocation(program, "active_ssao");	

	// Material Block
	loc_material = glGetUniformBlockIndex(program, "Material");	

	// Textures
	char buffer[64];
	for (int i = 0; i < MAX_2D_TEXTURES; i++) {
		sprintf_s(buffer, "u_texture_%d", i);
		loc_textures[i]	 = glGetUniformLocation(program, buffer);

		sprintf_s(buffer, "u_texture_cube_%d", i);
		loc_cube_textures[i] = glGetUniformLocation(program, buffer);
	}
	loc_channel_mask = glGetUniformLocation(program, "channel_mask");


	// Text
	text_color = glGetUniformLocation(program, "text_color");	
	
	// Debugging
	loc_debug_id = glGetUniformLocation(program, "debug_id");	
	loc_debug_view = glGetUniformLocation(program, "debug_view");	
	loc_debug_color = glGetUniformLocation(program, "debug_color");	

	BindTexturesUnits();

	CheckOpenGLError();
}


void Shader::SetShaderFiles(vector <string> shaderFiles)
{
	this->shaderFiles = shaderFiles;
	Reload();
}

void Shader::Reload() {
	if (program)
		glDeleteProgram(program);

	vector<unsigned int> shaders;

	switch (shaderFiles.size())
	{
		case 1:
			shaders.push_back( Shader::CreateShader(shaderFiles[0], GL_COMPUTE_SHADER));
			break;
		case 2:
			shaders.push_back( Shader::CreateShader(shaderFiles[0], GL_VERTEX_SHADER));
			shaders.push_back( Shader::CreateShader(shaderFiles[1], GL_FRAGMENT_SHADER));
			break;
		case 3:
			shaders.push_back( Shader::CreateShader(shaderFiles[0], GL_VERTEX_SHADER));
			shaders.push_back( Shader::CreateShader(shaderFiles[1], GL_GEOMETRY_SHADER));
			shaders.push_back( Shader::CreateShader(shaderFiles[2], GL_FRAGMENT_SHADER));
			break;
		case 4:
			shaders.push_back( Shader::CreateShader(shaderFiles[0], GL_VERTEX_SHADER));
			shaders.push_back( Shader::CreateShader(shaderFiles[1], GL_TESS_CONTROL_SHADER));
			shaders.push_back( Shader::CreateShader(shaderFiles[2], GL_TESS_EVALUATION_SHADER));
			shaders.push_back( Shader::CreateShader(shaderFiles[3], GL_FRAGMENT_SHADER));
			break;
		default:
			break;
	}
	cout << "PROGRAM:" << endl;
	program = Shader::CreateProgram(shaders);
	GetUniforms();
}

unsigned int Shader::CreateShader(const string &shaderFile, GLenum shaderType) 
{
	string shader_code;
	ifstream file(shaderFile.c_str(), ios::in);

	if(!file.good()) {
		cout << "\tCould not open file: " << shaderFile << endl;
		terminate();
	}

	cout << "\tFILE = " << shaderFile;

	file.seekg(0, ios::end);
	shader_code.resize((unsigned int)file.tellg());
	file.seekg(0, ios::beg);
	file.read(&shader_code[0], shader_code.size());
	file.close();

	int info_log_length = 0;
	int compile_result = 0;
	unsigned int gl_shader_object;
			
	// create new shader object
	gl_shader_object = glCreateShader(shaderType);				
	const char *shader_code_ptr = shader_code.c_str();
	const int shader_code_size = (int) shader_code.size();

	glShaderSource(gl_shader_object, 1, &shader_code_ptr, &shader_code_size);	
	glCompileShader(gl_shader_object);
	glGetShaderiv(gl_shader_object, GL_COMPILE_STATUS, &compile_result);					
			
	// LOG COMPILE ERRORS
	if(compile_result == GL_FALSE) {

		string str_shader_type = "";

		if(shaderType == GL_VERTEX_SHADER)
			str_shader_type="vertex shader";
		if(shaderType == GL_TESS_CONTROL_SHADER)
			str_shader_type="tess control shader";
		if(shaderType == GL_TESS_EVALUATION_SHADER)
			str_shader_type="tess evaluation shader";
		if(shaderType == GL_GEOMETRY_SHADER)
			str_shader_type="geometry shader";
		if(shaderType == GL_FRAGMENT_SHADER)
			str_shader_type="fragment shader";
		if(shaderType == GL_COMPUTE_SHADER)
			str_shader_type="compute shader";

		glGetShaderiv(gl_shader_object, GL_INFO_LOG_LENGTH, &info_log_length);		
		vector<char> shader_log(info_log_length);
		glGetShaderInfoLog(gl_shader_object, info_log_length, NULL, &shader_log[0]);	

		cout << "\n-----------------------------------------------------\n";
		cout << "\nERROR: " << str_shader_type << "\n\n";
		cout << &shader_log[0] << "\n";
		cout << "-----------------------------------------------------" << endl;

		return 0;
	}

	cout << "\t ..... COMPILED " << endl;

	return gl_shader_object;
}

unsigned int Shader::CreateProgram(const vector<unsigned int> &shaderObjects)
{
	int info_log_length = 0;
	int link_result = 0;

	// build OpenGL program object and link all the OpenGL shader objects
	unsigned int gl_program_object = glCreateProgram();

	for (auto shader: shaderObjects)
		glAttachShader(gl_program_object, shader);

	glLinkProgram(gl_program_object);												
	glGetProgramiv(gl_program_object, GL_LINK_STATUS, &link_result);

	// LOG LINK ERRORS
	if(link_result == GL_FALSE) {														

		glGetProgramiv(gl_program_object, GL_INFO_LOG_LENGTH, &info_log_length);		
		vector<char> program_log(info_log_length);
		glGetProgramInfoLog(gl_program_object, info_log_length, NULL, &program_log[0]);

		cout << "Shader Loader : LINK ERROR" << endl;
		cout << &program_log[0] << endl;

		return 0;
	}
			
	// Delete the shader objects because we do not need them any more 
	for (auto shader: shaderObjects)
		glDeleteShader(shader);

	return gl_program_object;

	CheckOpenGLError();
}

void Shader::Use()
{
	glUseProgram(program);
}
