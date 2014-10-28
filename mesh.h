#ifndef MESH_H
#define MESH_H

#include "glm/glm.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtx/vector_angle.hpp"
#include <vector>
#include "GL/glew.h"
//#include "shader.h"
//#include "assets.h"
//#include "util.h" //CreateShaderなどの定義の重複を解消するためにコメントアウト

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

namespace pho {

	struct Face {
		GLshort a,b,c;
		glm::vec3 normal;
	};

	class Mesh {
	public:
		Mesh();
		Mesh(std::vector<glm::vec3> vertices, std::vector<GLushort> indices, std::vector<glm::vec3> colors);
		Mesh(std::vector<glm::vec3> vertixes, std::vector<GLushort> indixes, std::vector<glm::vec3> colorz, bool simple);
		GLuint getVaoId();
//        void setShader(pho::Shader* tehShader);
		void model();
		void sphere();
		void wiremodel();
		void vertex();
		void plane();
		void simpleShadow();
		void Triangle();
		void Intersection();

		void draw();
		void spheredraw();
		void Trdraw();
		void wfdraw();
		void vertexdraw();
		void planedraw();
		void planelinedraw();
		void shadowdraw();
		void Intersectiondraw(vec3 Intersection);

		void vertexColorChange(int vertexNum);
		void vertexColorRes(int vertexNum);
//		void draw(bool wireframe);

		bool loadToGPU();
		glm::vec3 getPosition();
		
		glm::vec3 farthestVertex;
		bool findIntersection(int rotateMode, glm::mat4 modelMatrix, glm::mat4 rayMatrix, glm::vec3& foundPoint);
		bool rayTriangleIntersection(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::mat4 rayMatrix, float epsilon, glm::vec3 &intersection);
		bool raySphereIntersection(const vec3& raydir, const vec3& rayorig,const vec3& pos,const float& rad, vec3& hitpoint,float& distance, vec3& normal);
		bool findSphereIntersection(glm::mat4 modelMatrix, glm::mat4 rayMatrix,glm::vec3& foundPoint,float& foundDistance,glm::vec3& foundNormal);
		float sum(const vec3& v);

		bool simple; //for rays, simple lines

		unsigned int numFaces, numVertices;
//		unsigned int selectedtriangle;
        GLuint vaoId;
		GLuint wfvaoId;
		GLuint ibId;
		GLuint vertexVboId;
		GLuint colorVboId;
		GLuint wfibId; //indices for wireframe rendering
		GLuint texIndex;
		GLuint uniformBlockIndex;

		GLuint modelVAO;
		GLuint rayVAO;
		GLuint positionVBO;
		GLuint indexVBO;
		GLuint colorVBO;

		//sphere(circle)
		GLuint spheremodelVAO;
		GLuint spherepositionVBO;
		GLuint sphereindexVBO;

		//選択されたtriangle
		GLuint TrmodelVAO;
		GLuint TrpositionVBO;
		GLuint TrindexVBO;

		//wireframe
		GLuint wfmodelVAO;
		GLuint wfindexVBO;

		//vertex of model
		GLuint vertmodelVAO;
		GLuint vertpositionVBO;
		GLuint vertindexVBO;
		GLuint vertcolorVBO;

		GLuint PmodelVAO;
		GLuint PpositionVBO;
		GLuint PindexVBO;

		GLuint PlinemodelVAO;
		GLuint PlinepositionVBO;
		GLuint PlineindexVBO;

		GLuint ssmodelVAO;
		GLuint sspositionVBO;
		GLuint ssindexVBO;

		GLuint AxmodelVAO;
		GLuint AxpositionVBO;
		GLuint AxindexVBO;

		std::vector<pho::Face> faces;
		glm::mat4 modelMatrix;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<GLushort> indices;	 
		std::vector<GLushort> wfindices;	 
		std::vector<glm::vec3> colors;

		std::vector<glm::vec3> spherevertices;
		std::vector<GLushort> sphereindices;

		std::vector<glm::vec3> vertvertices;
		std::vector<GLushort> vertindices;	 	 
		std::vector<glm::vec3> vertcolors;

		std::vector<vec3> Trvertices;
		std::vector<GLushort> Trindices;

		std::vector<vec3> Axvertices;
		std::vector<GLushort> Axindices;

		std::vector<vec3> Pvertices;
		std::vector<GLushort> Pindices;

		std::vector<vec3> Plinevertices;
		std::vector<GLushort> Plineindices;

		std::vector<vec3> ssvertices;
		std::vector<GLushort> ssindices;

		bool selected;
		std::string name;
//		pho::Shader* shader;
		bool uploaded;
		float* texCoords;
//		pho::Material material;
		
	}; 

	
}

#endif 
