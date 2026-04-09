#include "pch.h"
#include <fstream>      // For input streams from files
#include <string>       // For the string type
#include <sstream>      // For streams from strings
#include <cmath>
#include "Mesh.h"
using namespace std;


CMesh::CMesh(void)
	: m_program(0)
	, m_vao(0)
	, m_vertexVBO(0)
	, m_normalVBO(0)
	, m_texVBO(0)
{
}


CMesh::~CMesh(void)
{
}


void CMesh::InitGL()
{
    for(PTV v=m_triangles.begin();  v!=m_triangles.end();  v++)
    {
		normalArray.push_back(m_normals[v->n]);
		vertexArray.push_back(m_vertices[v->v]);
		texArray.push_back(m_tvertices[v->t]);
	}

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexArray.size()*sizeof(glm::vec3), 
		&vertexArray[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &m_normalVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
	glBufferData(GL_ARRAY_BUFFER, normalArray.size()*sizeof(glm::vec3),
		&normalArray[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glGenBuffers(1, &m_texVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_texVBO);
	glBufferData(GL_ARRAY_BUFFER, texArray.size()*sizeof(glm::vec2),
		&texArray[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void CMesh::RenderGL()
{
	glUseProgram(m_program);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertexArray.size());
}

void CMesh::CleanGL()
{
	if (m_vertexVBO)
	{
		glDeleteBuffers(1, &m_vertexVBO);
		m_vertexVBO = 0;
	}
	if (m_normalVBO)
	{
		glDeleteBuffers(1, &m_normalVBO);
		m_normalVBO = 0;
	}
	if (m_texVBO)
	{
		glDeleteBuffers(1, &m_texVBO);
		m_texVBO = 0;
	}
	if (m_vao)
	{
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	vertexArray.clear();
	normalArray.clear();
	texArray.clear();
}

void CMesh::AddTriangleVertex(int v, int n, int t)
{
    TV tv;
    tv.v = v;
    tv.n = n;
    tv.t = t;
    m_triangles.push_back(tv);
}

//
// Name :         CMesh::AddFlatQuad()
// Description :  Add a quadrangle to the triangle mesh.
//
void CMesh::AddFlatQuad(int a, int b, int c, int d, int n)
{
	
	 // First triangle
    AddTriangleVertex(a, n, -1);
    AddTriangleVertex(b, n, -1);
    AddTriangleVertex(c, n, -1);

    // Second triangle
    AddTriangleVertex(a, n, -1);
    AddTriangleVertex(c, n, -1);
    AddTriangleVertex(d, n, -1);
}


void CMesh::AddQuad(int a, int b, int c, int d)
{
	 // First triangle
    AddTriangleVertex(a, a, -1);
    AddTriangleVertex(b, b, -1);
    AddTriangleVertex(c, c, -1);

    // Second triangle
    AddTriangleVertex(a, a, -1);
    AddTriangleVertex(c, c, -1);
    AddTriangleVertex(d, d, -1);
}


//
// Name :         CMesh::ComputeSmoothNormals()
// Description :  Compute a normal for each vertex that is the
//                average of the surface normals incident on the 
//                vertex.
//
void CMesh::ComputeSmoothNormals(void)
{
	m_normals.resize(m_vertices.size());
    for(unsigned int i=0;  i<m_vertices.size();  i++)
        m_normals[i] = glm::vec3(0, 0, 0);

	 for(PTV v=m_triangles.begin();  v!=m_triangles.end();  )
    {
        // Collect up the vertices of a triangle...
        int a = v->v;
        v++;
        int b = v->v;
        v++;
        int c = v->v;
        v++;

        // Surface normal
        glm::vec3 normal = glm::cross(m_vertices[b] - m_vertices[a], 
                                 m_vertices[c] - m_vertices[a]);
        normal *=1/normal.length();

        // Add to the incident vertices normals
        m_normals[a] += normal;
        m_normals[b] += normal;
        m_normals[c] += normal;

		 // Normalize the normals
    for(unsigned int i=0;  i<m_vertices.size();  i++)
        m_normals[i] *= 1/m_normals[i].length();
}
}

void CMesh::LoadOBJ(const char* filename)
{
	 ifstream str(filename);
    if(!str)
    {
        AfxMessageBox(L"File not found");
        return;
    }

    string line;
    while(getline(str, line))
    {
        istringstream lstr(line);

        string code;
        lstr >> code;
        if(code == "v") 
        {
            double x, y, z;
            lstr >> x >> y >> z;
            AddVertex(glm::vec3(x, y, z));
        }
        else if(code == "vn")
        {
			double a, b, c;
            lstr >> a >> b >> c;
            AddNormal(glm::vec3(a, b, c));	
        }
        else if(code == "vt")
        {
			double s, t;
            lstr >> s >> t;
            AddTexCoord(glm::vec2(s, t));
        }
        else if(code == "f")
        {
			for(int i=0;  i<3;  i++)
            {
                char slash;
                int v, t, n;
                lstr >> v >> slash >> t >> slash >> n;
                AddTriangleVertex(v-1, n-1, t-1);
            }
        }

    }
}

void CMesh::CreateCube()
{
		AddVertex(glm::vec3(-0.5, -0.5, 0.5));
		AddVertex(glm::vec3(-0.5, 0.5, 0.5));
		AddVertex(glm::vec3(0.5, 0.5, 0.5));
		AddVertex(glm::vec3(0.5, -0.5, 0.5));
		AddVertex(glm::vec3(-0.5, -0.5, -0.5));
		AddVertex(glm::vec3(-0.5, 0.5, -0.5));
		AddVertex(glm::vec3(0.5, 0.5, -0.5));
		AddVertex(glm::vec3(0.5, -0.5, -0.5));

		AddNormal(glm::vec3(1, 0, 0));
		AddNormal(glm::vec3(-1, 0, 0));
		AddNormal(glm::vec3(0, 1, 0));
		AddNormal(glm::vec3(0, -1, 0));
		AddNormal(glm::vec3(0, 0, 1));
		AddNormal(glm::vec3(0, 0, -1));

		AddTexCoord(glm::vec2(0, 0));
		AddTexCoord(glm::vec2(1, 0));
		AddTexCoord(glm::vec2(0, 1));
		AddTexCoord(glm::vec2(1, 1));

		AddTriangleVertex(1, 5, 0);
		AddTriangleVertex(0, 5, 1);
		AddTriangleVertex(3, 5, 2);
		AddTriangleVertex(1, 5, 0);
		AddTriangleVertex(3, 5, 2);
		AddTriangleVertex(2, 5, 3);
		AddTriangleVertex(2, 0, 0);
		AddTriangleVertex(3, 0, 1);
		AddTriangleVertex(7, 0, 2);
		AddTriangleVertex(2, 0, 0);
		AddTriangleVertex(7, 0, 2);
		AddTriangleVertex(6, 0, 3);
		AddTriangleVertex(3, 3, 0);
		AddTriangleVertex(0, 3, 1);
		AddTriangleVertex(4, 3, 2);
		AddTriangleVertex(3, 3, 0);
		AddTriangleVertex(4, 3, 2);
		AddTriangleVertex(7, 3, 3);
		AddTriangleVertex(6, 2, 0);
		AddTriangleVertex(5, 2, 1);
		AddTriangleVertex(1, 2, 2);
		AddTriangleVertex(6, 2, 0);
		AddTriangleVertex(1, 2, 2);
		AddTriangleVertex(2, 2, 3);
		AddTriangleVertex(4, 5, 0);
		AddTriangleVertex(5, 5, 1);
		AddTriangleVertex(6, 5, 2);
		AddTriangleVertex(4, 5, 0);
		AddTriangleVertex(6, 5, 2);
		AddTriangleVertex(7, 5, 3);
		AddTriangleVertex(5, 1, 0);
		AddTriangleVertex(4, 1, 1);
		AddTriangleVertex(0, 1, 2);
		AddTriangleVertex(5, 1, 0);
		AddTriangleVertex(0, 1, 2);
		AddTriangleVertex(1, 1, 3);
}

void CMesh::CreateDecagonalPrism(float radius, float height)
{
	m_vertices.clear();
	m_normals.clear();
	m_tvertices.clear();
	m_triangles.clear();
	vertexArray.clear();
	normalArray.clear();
	texArray.clear();

	const int sides = 10;
	const float halfH = 0.5f * height;
	const float pi = glm::pi<float>();

	auto addVNT = [&](const glm::vec3& v, const glm::vec3& n, const glm::vec2& t)
	{
		const int idx = (int)m_vertices.size();
		m_vertices.push_back(v);
		m_normals.push_back(n);
		m_tvertices.push_back(t);
		AddTriangleVertex(idx, idx, idx);
	};

	// Side faces (two triangles per side)
	for (int i = 0; i < sides; i++)
	{
		const float a0 = (2.f * pi * (float)i) / (float)sides;
		const float a1 = (2.f * pi * (float)(i + 1)) / (float)sides;

		const glm::vec3 p0(radius * cosf(a0), -halfH, radius * sinf(a0)); // bottom i
		const glm::vec3 p1(radius * cosf(a1), -halfH, radius * sinf(a1)); // bottom i+1
		const glm::vec3 p2(radius * cosf(a1), +halfH, radius * sinf(a1)); // top i+1
		const glm::vec3 p3(radius * cosf(a0), +halfH, radius * sinf(a0)); // top i

		glm::vec3 n = glm::normalize(glm::cross(p1 - p0, p3 - p0));

		const float u0 = (float)i / (float)sides;
		const float u1 = (float)(i + 1) / (float)sides;

		// Triangle 1: p0 p1 p2
		addVNT(p0, n, glm::vec2(u0, 0.f));
		addVNT(p1, n, glm::vec2(u1, 0.f));
		addVNT(p2, n, glm::vec2(u1, 1.f));

		// Triangle 2: p0 p2 p3
		addVNT(p0, n, glm::vec2(u0, 0.f));
		addVNT(p2, n, glm::vec2(u1, 1.f));
		addVNT(p3, n, glm::vec2(u0, 1.f));
	}

	// Top cap
	{
		const glm::vec3 n(0.f, 1.f, 0.f);
		const glm::vec3 c(0.f, +halfH, 0.f);
		const glm::vec2 tc(0.5f, 0.5f);

		for (int i = 0; i < sides; i++)
		{
			const float a0 = (2.f * pi * (float)i) / (float)sides;
			const float a1 = (2.f * pi * (float)(i + 1)) / (float)sides;
			const glm::vec3 p0(radius * cosf(a0), +halfH, radius * sinf(a0));
			const glm::vec3 p1(radius * cosf(a1), +halfH, radius * sinf(a1));

			const glm::vec2 t0(0.5f + 0.5f * (p0.x / radius), 0.5f + 0.5f * (p0.z / radius));
			const glm::vec2 t1(0.5f + 0.5f * (p1.x / radius), 0.5f + 0.5f * (p1.z / radius));

			// Winding for +Y outward: center, p0, p1
			addVNT(c, n, tc);
			addVNT(p0, n, t0);
			addVNT(p1, n, t1);
		}
	}

	// Bottom cap
	{
		const glm::vec3 n(0.f, -1.f, 0.f);
		const glm::vec3 c(0.f, -halfH, 0.f);
		const glm::vec2 tc(0.5f, 0.5f);

		for (int i = 0; i < sides; i++)
		{
			const float a0 = (2.f * pi * (float)i) / (float)sides;
			const float a1 = (2.f * pi * (float)(i + 1)) / (float)sides;
			const glm::vec3 p0(radius * cosf(a0), -halfH, radius * sinf(a0));
			const glm::vec3 p1(radius * cosf(a1), -halfH, radius * sinf(a1));

			const glm::vec2 t0(0.5f + 0.5f * (p0.x / radius), 0.5f + 0.5f * (p0.z / radius));
			const glm::vec2 t1(0.5f + 0.5f * (p1.x / radius), 0.5f + 0.5f * (p1.z / radius));

			// Winding for -Y outward: center, p1, p0
			addVNT(c, n, tc);
			addVNT(p1, n, t1);
			addVNT(p0, n, t0);
		}
	}
}
