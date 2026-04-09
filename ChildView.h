#pragma once

#include "ShaderWnd\ShaderWnd.h"

#include "Mesh.h"
#include "Sphere.h"
#include "graphics\GrTexture.h"
#include "graphics\GrCubeTexture.h"

class CChildView : public CShaderWnd
{
public:
	CChildView();
	virtual ~CChildView();

	virtual void RenderGL() override;
	virtual void InitGL() override;
	virtual void CleanGL() override;

protected:
	DECLARE_MESSAGE_MAP()

private:
	CGrTexture m_bunnyTex;
	CGrTexture m_sphereTex;
	CGrTexture m_heightTex;
	CGrCubeTexture m_cubeTex;

	CGrTexture m_fishTex;
	CGrTexture m_prismTex;

	CMesh m_bunny;
	CSphere m_sphere;
	CMesh m_skybox;

	CMesh m_cat;
	CMesh m_fish;
	CMesh m_prism;
};
