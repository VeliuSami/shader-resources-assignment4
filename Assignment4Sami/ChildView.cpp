#include "pch.h"
#include "ChildView.h"

using namespace glm;

BEGIN_MESSAGE_MAP(CChildView, CShaderWnd)
END_MESSAGE_MAP()

void CChildView::InitGL()
{
	glClearColor(0.2f, 0.6f, 0.4f, 1.f);
	glEnable(GL_DEPTH_TEST);

	// Bunny (diffuse texture + Blinn-Phong)
	m_bunnyTex.LoadFile(L"models/bunny-atlas.jpg");
	m_bunny.LoadOBJ("models\\bunny.obj");
	m_bunny.m_program = LoadShaders("ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl");
	m_bunny.InitGL();

	// Parallax bump-mapped sphere
	m_heightTex.LoadFile(L"textures/height.bmp");
	m_sphereTex.LoadFile(L"textures/bumpmap.jpg");
	m_sphere.SetRadius(3);
	m_sphere.m_program = LoadShaders("ShaderWnd/vertexSphere.glsl", "ShaderWnd/parallaxSphere.glsl");
	m_sphere.InitGL();

	// Sky box
	m_cubeTex.LoadFiles(L"textures/right.jpg", L"textures/left.jpg", L"textures/top.jpg",
		L"textures/bottom.jpg", L"textures/front.jpg", L"textures/back.jpg");
	m_skybox.CreateCube();
	m_skybox.m_program = LoadShaders("ShaderWnd/vertexSky.glsl", "ShaderWnd/fragmentSky.glsl");
	m_skybox.InitGL();

	// Cat — environment map (metallic)
	m_cat.LoadOBJ("models\\cat.obj");
	m_cat.m_program = LoadShaders("ShaderWnd/vertexSphere2.glsl", "ShaderWnd/fragmentSphere2.glsl");
	m_cat.InitGL();

	// Fish — diffuse texture
	m_fishTex.LoadFile(L"models/BLUEGILL.bmp");
	m_fish.LoadOBJ("models\\fish4.obj");
	m_fish.m_program = LoadShaders("ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl");
	m_fish.InitGL();

	// Regular decagonal prism — diffuse texture
	m_prismTex.LoadFile(L"textures/marble02.bmp");
	m_prism.CreateDecagonalPrism(2.f, 4.f);
	m_prism.m_program = LoadShaders("ShaderWnd/vertex.glsl", "ShaderWnd/fragment.glsl");
	m_prism.InitGL();
}

void CChildView::RenderGL()
{
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Shared light/material for diffuse shader
	vec4 light_position(-5.f, 5.f, -5.f, 0.f);
	vec4 light_ambient(0.2f, 0.2f, 0.2f, 1.f);
	vec4 light_diffuse(1.f, 1.f, 1.f, 1.f);
	vec4 light_specular(1.f, 1.f, 1.f, 1.f);

	vec4 material_ambient(1.f, 1.f, 1.f, 1.f);
	vec4 material_diffuse(1.f, 1.f, 1.f, 1.f);
	vec4 material_specular(1.f, 1.f, 1.f, 1.f);
	float material_shininess = 100.0f;

	vec4 ambient_product = light_ambient * material_ambient;
	vec4 diffuse_product = light_diffuse * material_diffuse;
	vec4 specular_product = light_specular * material_specular;

	// --- Bunny ---
	m_program = m_bunny.m_program;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "diffuse_mat"), 0);

	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");
	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(m_mPVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(m_mVM));

	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(m_program, "DiffuseProduct"), 1, value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(m_program, "SpecularProduct"), 1, value_ptr(specular_product));
	glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, value_ptr(light_position));
	glUniform1f(glGetUniformLocation(m_program, "Shininess"), material_shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_bunnyTex.TexName());
	m_bunny.RenderGL();

	// --- Parallax sphere ---
	m_program = m_sphere.m_program;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "bump_map"), 0);
	glUniform1i(glGetUniformLocation(m_program, "height_map"), 1);

	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");

	mat4 M = translate(mat4(1.f), vec3(-10.f, 0.f, 0.f))
		* rotate(mat4(1.f), 90.f, vec3(0.f, 1.f, 0.f));
	mat4 VM = m_mVM * M;
	mat4 PVM = m_mPVM * M;

	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(VM));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_sphereTex.TexName());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_heightTex.TexName());
	m_sphere.RenderGL();

	// --- Cat (environment map) ---
	m_program = m_cat.m_program;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "env_map"), 0);

	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");

	M = translate(mat4(1.f), vec3(10.f, 0.f, 0.f))
		* scale(mat4(1.f), vec3(2.2f, 2.2f, 2.2f));
	VM = m_mVM * M;
	PVM = m_mPVM * M;

	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(VM));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex.TexName());
	m_cat.RenderGL();

	// --- Fish (diffuse) ---
	m_program = m_fish.m_program;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "diffuse_mat"), 0);

	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");

	// Place fish clearly in front of (and offset from) the cat to avoid intersection.
	M = translate(mat4(1.f), vec3(6.0f, -1.1f, 11.0f))
		* rotate(mat4(1.f), -55.f, vec3(0.f, 1.f, 0.f))
		* rotate(mat4(1.f), 10.f, vec3(1.f, 0.f, 0.f))
		* scale(mat4(1.f), vec3(0.34f, 0.34f, 0.34f));
	VM = m_mVM * M;
	PVM = m_mPVM * M;

	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(VM));

	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(m_program, "DiffuseProduct"), 1, value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(m_program, "SpecularProduct"), 1, value_ptr(specular_product));
	glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, value_ptr(light_position));
	glUniform1f(glGetUniformLocation(m_program, "Shininess"), material_shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fishTex.TexName());
	m_fish.RenderGL();

	// --- Decagonal prism (diffuse marble) ---
	m_program = m_prism.m_program;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "diffuse_mat"), 0);

	m_nPVM = glGetUniformLocation(m_program, "mPVM");
	m_nVM = glGetUniformLocation(m_program, "mVM");

	M = translate(mat4(1.f), vec3(-11.f, 0.f, -7.f))
		* rotate(mat4(1.f), 40.f, vec3(0.f, 1.f, 0.f));
	VM = m_mVM * M;
	PVM = m_mPVM * M;

	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(PVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(VM));

	glUniform4fv(glGetUniformLocation(m_program, "AmbientProduct"), 1, value_ptr(ambient_product));
	glUniform4fv(glGetUniformLocation(m_program, "DiffuseProduct"), 1, value_ptr(diffuse_product));
	glUniform4fv(glGetUniformLocation(m_program, "SpecularProduct"), 1, value_ptr(specular_product));
	glUniform4fv(glGetUniformLocation(m_program, "LightPosition"), 1, value_ptr(light_position));
	glUniform1f(glGetUniformLocation(m_program, "Shininess"), material_shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_prismTex.TexName());
	m_prism.RenderGL();

	// --- Sky box ---
	m_program = m_skybox.m_program;
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "skybox"), 0);

	m_nPVM = glGetUniformLocation(m_program, "mP");
	m_nVM = glGetUniformLocation(m_program, "mV");
	glUniformMatrix4fv(m_nPVM, 1, GL_FALSE, value_ptr(m_mProjection));
	mat4 view = mat4(mat3(m_mVM));
	glUniformMatrix4fv(m_nVM, 1, GL_FALSE, value_ptr(view));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTex.TexName());
	glDepthFunc(GL_LEQUAL);
	m_skybox.RenderGL();
	glDepthFunc(GL_LESS);
}

void CChildView::CleanGL()
{
	m_bunny.CleanGL();
	m_sphere.CleanGL();
	m_skybox.CleanGL();
	m_cat.CleanGL();
	m_fish.CleanGL();
	m_prism.CleanGL();

	m_bunnyTex.Clear();
	m_sphereTex.Clear();
	m_heightTex.Clear();
	m_cubeTex.Clear();
	m_fishTex.Clear();
	m_prismTex.Clear();
}

