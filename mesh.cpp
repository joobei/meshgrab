#pragma warning(disable: 4819)
#include "mesh.h"
#include <iostream>
#include <stdio.h>
//#include <vector>

// Vertex Attribute Locations
const GLuint vertexLoc=0, indexLoc = 4, normalLoc=1, texCoordLoc=2, colorLoc=3;

int selectedtriangle,selectedtriangle2,Tselectedtriangle,FixedTriangle;
glm::vec3 normal; //選択している面の垂線

pho::Mesh::Mesh() {
	modelMatrix = glm::mat4();
	selected = false;
}
/*
//loads data and uploads to GPU
pho::Mesh::Mesh(std::vector<glm::vec3> vertixes, std::vector<GLushort> indixes, std::vector<glm::vec3> colorz):
vertices(vertixes),
	indices(indixes),
	colors(colorz)
{
	simple = false;
	farthestVertex = vertices[indices[0]];
	GLushort farthestIndex=0;

	for (std::vector<GLushort>::size_type i=0; i != indices.size(); i+=3) {
		
		Face temp;
		temp.a = indixes[i];
		temp.b = indixes[i+1];
		temp.c = indixes[i+2];
		faces.push_back(temp);
		
		//for every face create indices to draw lines between every vertex
		wfindices.push_back(indixes[i]);
		wfindices.push_back(indixes[i+1]);
		wfindices.push_back(indixes[i+1]);
		wfindices.push_back(indixes[i+2]);
		wfindices.push_back(indixes[i+2]);
		wfindices.push_back(indixes[i]);
	}

	//find farthest vertex and save for sphere radius
	for (std::vector<glm::vec3>::size_type i=0; i!= vertices.size(); i++) {

		if(glm::distance(vertices[i],glm::vec3(0,0,0)) > glm::distance(vertices[farthestIndex],glm::vec3(0,0,0))) {
			farthestVertex = vertices[farthestIndex];
			farthestIndex = i;
		}
	}


	modelMatrix = glm::mat4();
	CALL_GL(glGenVertexArrays(1,&vaoId));
	CALL_GL(glGenBuffers(1,&ibId));
	CALL_GL(glGenBuffers(1,&vertexVboId));
	CALL_GL(glGenBuffers(1,&colorVboId));

	CALL_GL(glBindVertexArray(vaoId));

	

	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibId));
	CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLushort),indices.data(),GL_STATIC_DRAW));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,vertexVboId));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),vertices.data(),GL_STATIC_DRAW));
	CALL_GL(glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(vertexLoc));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,colorVboId));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER,colors.size()*2*sizeof(GLfloat),colors.data(),GL_STATIC_DRAW));
	CALL_GL(glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(colorLoc));

	CALL_GL(glBindVertexArray(0));


	CALL_GL(glGenVertexArrays(1,&wfvaoId));  //vao for wireframe
	CALL_GL(glGenBuffers(1,&wfibId));

	CALL_GL(glBindVertexArray(wfvaoId));

	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,wfibId));
	CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,wfindices.size()*sizeof(GLushort),wfindices.data(),GL_STATIC_DRAW));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,vertexVboId));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),vertices.data(),GL_STATIC_DRAW));
	CALL_GL(glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(vertexLoc));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,colorVboId));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER,colors.size()*2*sizeof(GLfloat),colors.data(),GL_STATIC_DRAW));
	CALL_GL(glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(colorLoc));

	CALL_GL(glBindVertexArray(0));
}

//loads a simple mesh (like ray and plane)
pho::Mesh::Mesh(std::vector<glm::vec3> vertixes, std::vector<GLushort> indixes, std::vector<glm::vec3> colorz, bool simplex):
vertices(vertixes),
	indices(indixes),
	colors(colorz),
	simple(simplex)
{
	wfindices.push_back(indixes[0]);
	wfindices.push_back(indixes[1]);

	modelMatrix = glm::mat4();
	CALL_GL(glGenVertexArrays(1,&vaoId));
	CALL_GL(glGenBuffers(1,&ibId));
	CALL_GL(glGenBuffers(1,&vertexVboId));
	CALL_GL(glGenBuffers(1,&colorVboId));

	CALL_GL(glBindVertexArray(vaoId));
	
	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,wfibId));
	CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,wfindices.size()*sizeof(GLushort),wfindices.data(),GL_STATIC_DRAW));

	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibId));
	CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLushort),indices.data(),GL_STATIC_DRAW));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,vertexVboId));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),vertices.data(),GL_STATIC_DRAW));
	CALL_GL(glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(vertexLoc));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,colorVboId));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER,colors.size()*2*sizeof(GLfloat),colors.data(),GL_STATIC_DRAW));
	CALL_GL(glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(colorLoc));

	CALL_GL(glBindVertexArray(0));

	//for the wireframe
	CALL_GL(glGenVertexArrays(1,&wfvaoId));
	CALL_GL(glGenBuffers(1,&wfibId));

	CALL_GL(glBindVertexArray(wfvaoId));
	
	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,wfibId));
	CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,wfindices.size()*sizeof(GLushort),wfindices.data(),GL_STATIC_DRAW));

	
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,vertexVboId));
	CALL_GL(glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(vertexLoc));

	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,colorVboId));
	CALL_GL(glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0));
	CALL_GL(glEnableVertexAttribArray(colorLoc));

	CALL_GL(glBindVertexArray(0));


}


void pho::Mesh::setShader(pho::Shader* tehShader) {
	shader = tehShader;
}

bool pho::Mesh::loadToGPU() {

	GLuint buffer;  //buffer used to upload stuff to GPU memory

	//Generate Vertex Array for mesh
	CALL_GL(glGenVertexArrays(1,&(vaoId)));
	CALL_GL(glBindVertexArray(vaoId));

	//faces
	CALL_GL(glGenBuffers(1,&buffer));
	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer));
	//CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numFaces * 3, faces, GL_STATIC_DRAW));

	//vertices
	CALL_GL(glGenBuffers(1,&buffer));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numVertices, vertices.data(), GL_STATIC_DRAW));
	CALL_GL(glEnableVertexAttribArray(pho::vertexLoc));
	CALL_GL(glVertexAttribPointer(pho::vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0));

	//normals
	CALL_GL(glGenBuffers(1, &buffer));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*numVertices, normals.data(), GL_STATIC_DRAW));
	CALL_GL(glEnableVertexAttribArray(pho::normalLoc));
	CALL_GL(glVertexAttribPointer(pho::normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0));

	//texture coordinates
	CALL_GL(glGenBuffers(1, &buffer));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	CALL_GL(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*numVertices, texCoords, GL_STATIC_DRAW));
	CALL_GL(glEnableVertexAttribArray(pho::texCoordLoc));
	CALL_GL(glVertexAttribPointer(pho::texCoordLoc, 2, GL_FLOAT, 0, 0, 0));

	// unbind buffers
	CALL_GL(glBindVertexArray(0));
	CALL_GL(glBindBuffer(GL_ARRAY_BUFFER,0));
	CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));

	return true;
}

GLuint pho::Mesh::getVaoId() {
	return vaoId;
}

glm::vec3 pho::Mesh::getPosition() {
	return glm::vec3(modelMatrix[3]);
}

void pho::Mesh::draw() {
	
	CALL_GL(glBindVertexArray(vaoId));
	CALL_GL(glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,NULL));
}

//draw wireframe (to be improved)
void pho::Mesh::draw(bool wireframe) {

	CALL_GL(glBindVertexArray(wfvaoId));
	CALL_GL(glLineWidth(2.5));
	CALL_GL(glDrawElements(GL_LINES,wfindices.size(),GL_UNSIGNED_SHORT,NULL));
}
*/

void pho::Mesh::draw() {
	
	glBindVertexArray(modelVAO);
	//glDrawRangeElements(GL_TRIANGLES,0,35,36,GL_UNSIGNED_SHORT,NULL); //DRAW!
	glDrawRangeElements(GL_TRIANGLES,0,11,12,GL_UNSIGNED_SHORT,NULL); //DRAW!
}


void pho::Mesh::spheredraw() {
	glBindVertexArray(spheremodelVAO);
	glLineWidth(2.5);
	glDrawElements(GL_LINES,sphereindices.size(),GL_UNSIGNED_SHORT,NULL);
}


void pho::Mesh::Trdraw() { //parts of draw

	std::vector<vec3> newTrvertices;

	newTrvertices.push_back(vertices[indices[Tselectedtriangle]]);
	newTrvertices.push_back(vertices[indices[Tselectedtriangle+1]]);
	newTrvertices.push_back(vertices[indices[Tselectedtriangle+2]]);

	glGenBuffers(1,&TrpositionVBO);
	glBindVertexArray(TrmodelVAO);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,TrpositionVBO);
    glBufferData(GL_ARRAY_BUFFER,newTrvertices.size()*3*sizeof(GLfloat),newTrvertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	glDrawRangeElements(GL_TRIANGLES,0,2,3,GL_UNSIGNED_SHORT,NULL); //DRAW!
//	std::cout << selectedtriangle << '\n';
}


//draw wireframe (to be improved)
void pho::Mesh::wfdraw(){

	glBindVertexArray(wfmodelVAO);
	//glLineWidth(2.5);
	glLineWidth(5.0);
	glDrawElements(GL_LINES,wfindices.size(),GL_UNSIGNED_SHORT,NULL);
}


void pho::Mesh::vertexdraw(){

	glBindVertexArray(vertmodelVAO);
	glPointSize(20.0);
	//glPointSize(10.0);
	glDrawElements(GL_POINTS,vertindices.size(),GL_UNSIGNED_SHORT,NULL);
}


void pho::Mesh::planedraw() {
	
	glBindVertexArray(PmodelVAO);
	glDrawRangeElements(GL_TRIANGLES,0,5,6,GL_UNSIGNED_SHORT,NULL); //DRAW!
}


void pho::Mesh::planelinedraw() {
	glBindVertexArray(PlinemodelVAO);
	glLineWidth(2.5);
	glDrawElements(GL_LINES,Plineindices.size(),GL_UNSIGNED_SHORT,NULL);
}
	

void pho::Mesh::shadowdraw() {
	glBindVertexArray(ssmodelVAO);
	glDrawElements(GL_TRIANGLES,ssindices.size(),GL_UNSIGNED_SHORT,NULL);
}


void pho::Mesh::Intersectiondraw(vec3 Intersection) { //Intersection draw

	std::vector<vec3> newAxvertices;
//#define cubeSize 0.05
#define cubeSize 0.1
/*
	newAxvertices.push_back(vec3(Intersection[0]-0.05,Intersection[1]-0.05,Intersection[2]));
	newAxvertices.push_back(vec3(Intersection[0]+0.05,Intersection[1]-0.05,Intersection[2]));
	newAxvertices.push_back(vec3(Intersection[0]+0.05,Intersection[1]+0.05,Intersection[2]));
	newAxvertices.push_back(vec3(Intersection[0]-0.05,Intersection[1]+0.05,Intersection[2]));

	newAxvertices.push_back(vec3(Intersection[0],Intersection[1]-0.05,Intersection[2]-0.05));
	newAxvertices.push_back(vec3(Intersection[0],Intersection[1]+0.05,Intersection[2]-0.05));
	newAxvertices.push_back(vec3(Intersection[0],Intersection[1]+0.05,Intersection[2]+0.05));
	newAxvertices.push_back(vec3(Intersection[0],Intersection[1]-0.05,Intersection[2]+0.05));

	newAxvertices.push_back(vec3(Intersection[0]-0.05,Intersection[1],Intersection[2]-0.05));
	newAxvertices.push_back(vec3(Intersection[0]+0.05,Intersection[1],Intersection[2]-0.05));
	newAxvertices.push_back(vec3(Intersection[0]+0.05,Intersection[1],Intersection[2]+0.05));
	newAxvertices.push_back(vec3(Intersection[0]-0.05,Intersection[1],Intersection[2]+0.05));
*/
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]-cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]-cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]+cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]+cubeSize,Intersection[2]-cubeSize));

	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]-cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]-cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]+cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]+cubeSize,Intersection[2]+cubeSize));

	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]-cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]+cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]+cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]-cubeSize,Intersection[2]+cubeSize));

	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]-cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]+cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]+cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]-cubeSize,Intersection[2]+cubeSize));

	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]-cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]-cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]-cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]-cubeSize,Intersection[2]+cubeSize));

	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]+cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]+cubeSize,Intersection[2]-cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]+cubeSize,Intersection[1]+cubeSize,Intersection[2]+cubeSize));
	newAxvertices.push_back(vec3(Intersection[0]-cubeSize,Intersection[1]+cubeSize,Intersection[2]+cubeSize));

	/*
	newAxvertices.push_back(vec3(-1, -1, Intersection[2]));
    newAxvertices.push_back(vec3(1, -1, Intersection[2]));
    newAxvertices.push_back(vec3(1, 1, Intersection[2]));
    newAxvertices.push_back(vec3(-1, 1, Intersection[2]));
	*/
	glGenBuffers(1,&AxpositionVBO);
	glBindVertexArray(AxmodelVAO);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,AxpositionVBO);
    glBufferData(GL_ARRAY_BUFFER,newAxvertices.size()*3*sizeof(GLfloat),newAxvertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	glDrawRangeElements(GL_TRIANGLES,0,35,36,GL_UNSIGNED_SHORT,NULL); //DRAW!
//	std::cout << selectedtriangle << '\n';
}


void pho::Mesh::model(){

	/*//small size
	vertices.push_back(vec3(-0.5, -0.5, -0.5));
	colors.push_back(vec3(0.0,0.5,1.0));
	vertices.push_back(vec3(0.5, -0.5, 0.5));
	colors.push_back(vec3(0.0,0.5,1.0));
	vertices.push_back(vec3(-0.5, 0.5, 0.5));
	colors.push_back(vec3(0.0,0.5,1.0));

	vertices.push_back(vec3(-0.5, -0.5, -0.5));
	colors.push_back(vec3(0.0,1.0,1.0));
	vertices.push_back(vec3(-0.5, 0.5, 0.5));
	colors.push_back(vec3(0.0,1.0,1.0));
	vertices.push_back(vec3(0.5, 0.5, -0.5));
	colors.push_back(vec3(0.0,1.0,1.0));

	vertices.push_back(vec3(-0.5, -0.5, -0.5));
	colors.push_back(vec3(0.5,0.0,1.0));
	vertices.push_back(vec3(0.5, -0.5, 0.5));
	colors.push_back(vec3(0.5,0.0,1.0));
	vertices.push_back(vec3(0.5, 0.5, -0.5));
	colors.push_back(vec3(0.5,0.0,1.0));

	vertices.push_back(vec3(-0.5, 0.5, 0.5));
	colors.push_back(vec3(1.0,0.0,1.0));
	vertices.push_back(vec3(0.5, -0.5, 0.5));
	colors.push_back(vec3(1.0,0.0,1.0));
	vertices.push_back(vec3(0.5, 0.5, -0.5));
	colors.push_back(vec3(1.0,0.0,1.0));
	*/

	//normal size
	vertices.push_back(vec3(-1, -1, -1));
	colors.push_back(vec3(0.0,0.5,1.0));
	vertices.push_back(vec3(1, -1, 1));
	colors.push_back(vec3(0.0,0.5,1.0));
	vertices.push_back(vec3(-1, 1, 1));
	colors.push_back(vec3(0.0,0.5,1.0));

	vertices.push_back(vec3(-1, -1, -1));
	colors.push_back(vec3(0.0,1.0,1.0));
	vertices.push_back(vec3(-1, 1, 1));
	colors.push_back(vec3(0.0,1.0,1.0));
	vertices.push_back(vec3(1, 1, -1));
	colors.push_back(vec3(0.0,1.0,1.0));

	vertices.push_back(vec3(-1, -1, -1));
	colors.push_back(vec3(0.5,0.0,1.0));
	vertices.push_back(vec3(1, -1, 1));
	colors.push_back(vec3(0.5,0.0,1.0));
	vertices.push_back(vec3(1, 1, -1));
	colors.push_back(vec3(0.5,0.0,1.0));

	vertices.push_back(vec3(-1, 1, 1));
	colors.push_back(vec3(1.0,0.0,1.0));
	vertices.push_back(vec3(1, -1, 1));
	colors.push_back(vec3(1.0,0.0,1.0));
	vertices.push_back(vec3(1, 1, -1));
	colors.push_back(vec3(1.0,0.0,1.0));

	/*//initial color
	colors.push_back(vec3(0.0,0.5,1.0));
	colors.push_back(vec3(0.0,1.0,1.0));
	colors.push_back(vec3(0.5,0.0,1.0));
	colors.push_back(vec3(1.0,0.0,1.0));
	*/

	indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2); 

    indices.push_back(3);
    indices.push_back(4);
    indices.push_back(5);

	indices.push_back(6);
    indices.push_back(7);
    indices.push_back(8);

    indices.push_back(9);
    indices.push_back(10);
    indices.push_back(11);


	//find farthest vertex and save for sphere radius
	farthestVertex = vertices[indices[0]];
	GLushort farthestIndex=0;

	for (std::vector<glm::vec3>::size_type i=0; i!= vertices.size(); i++) {

		if(glm::distance(vertices[i],glm::vec3(0,0,0)) > glm::distance(vertices[farthestIndex],glm::vec3(0,0,0))) {
			farthestVertex = vertices[farthestIndex];
			farthestIndex = i;
		}
	}
	//std::cout << farthestIndex << farthestVertex[0] << farthestVertex[1] << farthestVertex[2] << '\n';


	glGenVertexArrays(1,&modelVAO);
    glGenBuffers(1,&positionVBO);
    glGenBuffers(1,&indexVBO);
    glGenBuffers(1,&colorVBO);
	
	//bind our model VAO
    glBindVertexArray(modelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLushort),indices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,positionVBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);
	

	//bind the color VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER,colorVBO);
    glBufferData(GL_ARRAY_BUFFER,colors.size()*3*sizeof(GLfloat),colors.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(colorLoc);
    
	glBindVertexArray(0);

}


void pho::Mesh::sphere(){

	float radius = glm::length(farthestVertex);

	//spherevertices.push_back(vec3(0,0,0));
	for(float i = 0; i < 6.38 ; i+=0.1)  //generate vertices at positions on the circumference from 0 to 2*pi 
	{
		spherevertices.push_back(glm::vec3(radius*cos(i),radius*sin(i),0));
	}
	for(int i = 0; i < 62.8 ; i++)
	{
		sphereindices.push_back(i);
		sphereindices.push_back(i+1);
	}

	sphereindices.push_back(62);
	sphereindices.push_back(0);
	

	glGenVertexArrays(1,&spheremodelVAO);
    glGenBuffers(1,&sphereindexVBO);
	glGenBuffers(1,&spherepositionVBO);

	//bind our model VAO
    glBindVertexArray(spheremodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,sphereindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sphereindices.size()*sizeof(GLushort),sphereindices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,spherepositionVBO);
    glBufferData(GL_ARRAY_BUFFER,spherevertices.size()*3*sizeof(GLfloat),spherevertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	glBindVertexArray(0);
}



void pho::Mesh::wiremodel(){

//	std::vector<vec3> wfindices;

	for (std::vector<GLushort>::size_type i=0; i != 12; i+=3) {

		//for every face create indices to draw lines between every vertex
		wfindices.push_back(indices[i]);
		wfindices.push_back(indices[i+1]);
		wfindices.push_back(indices[i+1]);
		wfindices.push_back(indices[i+2]);
		wfindices.push_back(indices[i+2]);
		wfindices.push_back(indices[i]);
	}

	glGenVertexArrays(1,&wfmodelVAO);
    glGenBuffers(1,&wfindexVBO);
	
	//bind our model VAO
    glBindVertexArray(wfmodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,wfindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,wfindices.size()*sizeof(GLushort),wfindices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,positionVBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);
	/*
	//bind the color VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER,colorVBO);
    glBufferData(GL_ARRAY_BUFFER,colors.size()*3*sizeof(GLfloat),colors.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(colorLoc);
	*/
	glBindVertexArray(0);
}


void pho::Mesh::vertex(){

	/*//small size
	vertvertices.push_back(vec3(-0.5, -0.5, -0.5));
	vertcolors.push_back(vec3(1.0,0.0,0.0));
	vertvertices.push_back(vec3(0.5, -0.5, 0.5));
	vertcolors.push_back(vec3(0.0,1.0,1.0));
	vertvertices.push_back(vec3(-0.5, 0.5, 0.5));
	vertcolors.push_back(vec3(0.0,0.0,1.0));
	vertvertices.push_back(vec3(0.5, 0.5, -0.5));
	vertcolors.push_back(vec3(1.0,1.0,0.0));
	*/

	//normal size
	vertvertices.push_back(vec3(-1, -1, -1));
	vertcolors.push_back(vec3(1.0,0.0,0.0));
	vertvertices.push_back(vec3(1, -1, 1));
	vertcolors.push_back(vec3(0.0,1.0,1.0));
	vertvertices.push_back(vec3(-1, 1, 1));
	vertcolors.push_back(vec3(0.0,0.0,1.0));
	vertvertices.push_back(vec3(1, 1, -1));
	vertcolors.push_back(vec3(1.0,1.0,0.0));

	vertindices.push_back(0);
    vertindices.push_back(1);
    vertindices.push_back(2); 
    vertindices.push_back(3);

	glGenVertexArrays(1,&vertmodelVAO);
    glGenBuffers(1,&vertpositionVBO);
    glGenBuffers(1,&vertindexVBO);
    glGenBuffers(1,&vertcolorVBO);
	
	//bind our model VAO
    glBindVertexArray(vertmodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vertindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,vertindices.size()*sizeof(GLushort),vertindices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,vertpositionVBO);
    glBufferData(GL_ARRAY_BUFFER,vertvertices.size()*3*sizeof(GLfloat),vertvertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);
	

	//bind the color VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER,vertcolorVBO);
    glBufferData(GL_ARRAY_BUFFER,vertcolors.size()*3*sizeof(GLfloat),vertcolors.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(colorLoc);
    
	glBindVertexArray(0);

}


void pho::Mesh::vertexColorChange(int vertexNum){

	vertcolors[vertindices[vertexNum]] = vec3(0.0,1.0,0.0); //緑に変更

	//bind the color VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER,vertcolorVBO);
    glBufferData(GL_ARRAY_BUFFER,vertcolors.size()*3*sizeof(GLfloat),vertcolors.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(colorLoc);
}


void pho::Mesh::vertexColorRes(int vertexNum){

	//vertexの色を元に戻す
	switch(vertexNum){
	case 0:
		vertcolors[vertindices[0]] = vec3(1.0,0.0,0.0);
		break;
	case 1:
		vertcolors[vertindices[1]] = vec3(0.0,1.0,1.0);
		break;
	case 2:
		vertcolors[vertindices[2]] = vec3(0.0,0.0,1.0);
		break;
	case 3:
		vertcolors[vertindices[3]] = vec3(1.0,1.0,0.0);
		break;
	default:
		break;
	}

	//bind the color VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER,vertcolorVBO);
    glBufferData(GL_ARRAY_BUFFER,vertcolors.size()*3*sizeof(GLfloat),vertcolors.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(colorLoc);
}


void pho::Mesh::plane(){

	#define PLANE_WIDE 10
	#define PLANE_DEPTH 20
	
	//plane
	Pvertices.push_back(vec3(-PLANE_WIDE, 0, PLANE_DEPTH));
	Pvertices.push_back(vec3(PLANE_WIDE, 0, PLANE_DEPTH));
	Pvertices.push_back(vec3(PLANE_WIDE, 0, -PLANE_DEPTH));
	Pvertices.push_back(vec3(-PLANE_WIDE, 0, -PLANE_DEPTH));

	Pindices.push_back(0);
    Pindices.push_back(1);
    Pindices.push_back(2);

	Pindices.push_back(0);
    Pindices.push_back(2);
    Pindices.push_back(3);

	glGenVertexArrays(1,&PmodelVAO);
    glGenBuffers(1,&PpositionVBO);
    glGenBuffers(1,&PindexVBO);

	//bind our model VAO
    glBindVertexArray(PmodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,PindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,Pindices.size()*sizeof(GLushort),Pindices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,PpositionVBO);
    glBufferData(GL_ARRAY_BUFFER,Pvertices.size()*3*sizeof(GLfloat),Pvertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	glBindVertexArray(0);



	//line
	for (int i=0;i <= PLANE_WIDE*2; i++) {
		Plinevertices.push_back(vec3(-PLANE_WIDE+i, 0, PLANE_DEPTH));
		Plinevertices.push_back(vec3(-PLANE_WIDE+i, 0, -PLANE_DEPTH));
	}
	for (int i=0;i <= PLANE_DEPTH*2; i++) {
		Plinevertices.push_back(vec3(-PLANE_WIDE, 0, -PLANE_DEPTH+i));
		Plinevertices.push_back(vec3(PLANE_WIDE, 0, -PLANE_DEPTH+i));
	}
	for (int i=0;i < PLANE_WIDE*4+PLANE_DEPTH*4+4; i++) {
		Plineindices.push_back(i);
	}
	
	glGenVertexArrays(1,&PlinemodelVAO);
    glGenBuffers(1,&PlinepositionVBO);
    glGenBuffers(1,&PlineindexVBO);

	//bind our model VAO
    glBindVertexArray(PlinemodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,PlineindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,Plineindices.size()*sizeof(GLushort),Plineindices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,PlinepositionVBO);
    glBufferData(GL_ARRAY_BUFFER,Plinevertices.size()*3*sizeof(GLfloat),Plinevertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	glBindVertexArray(0);

}


void pho::Mesh::simpleShadow(){

	float radius = 2.0f;

	ssvertices.push_back(vec3(0,0,0));
	for(float i = 0; i < 6.38 ; i+=0.1)  //generate vertices at positions on the circumference from 0 to 2*pi 
	{
		ssvertices.push_back(glm::vec3(radius*cos(i),0,radius*sin(i)));
	}
	for(int i = 1; i < 63.8 ; i++)
	{
		ssindices.push_back(0);
		ssindices.push_back(i);
		ssindices.push_back(i+1);
	}
	

	glGenVertexArrays(1,&ssmodelVAO);
    glGenBuffers(1,&ssindexVBO);
	glGenBuffers(1,&sspositionVBO);

	//bind our model VAO
    glBindVertexArray(ssmodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ssindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,ssindices.size()*sizeof(GLushort),ssindices.data(),GL_STATIC_DRAW);

	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,sspositionVBO);
    glBufferData(GL_ARRAY_BUFFER,ssvertices.size()*3*sizeof(GLfloat),ssvertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	glBindVertexArray(0);
}


void pho::Mesh::Triangle(){

	Trindices.push_back(0);
    Trindices.push_back(1);
    Trindices.push_back(2); 

	glGenVertexArrays(1,&TrmodelVAO);
    glGenBuffers(1,&TrindexVBO);
	
	//bind our model VAO
    glBindVertexArray(TrmodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,TrindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,Trindices.size()*sizeof(GLushort),Trindices.data(),GL_STATIC_DRAW);

	glBindVertexArray(0);
}


void pho::Mesh::Intersection(){

	Axindices.push_back(0);
	Axindices.push_back(1);
	Axindices.push_back(2);
	Axindices.push_back(0);
	Axindices.push_back(2);
	Axindices.push_back(3);

	Axindices.push_back(4);
    Axindices.push_back(5);
    Axindices.push_back(6);
    Axindices.push_back(4);
    Axindices.push_back(6);
    Axindices.push_back(7);

	Axindices.push_back(8);
    Axindices.push_back(9);
    Axindices.push_back(10);
    Axindices.push_back(8);
    Axindices.push_back(10);
    Axindices.push_back(11);

	Axindices.push_back(12);
    Axindices.push_back(13);
    Axindices.push_back(14);
    Axindices.push_back(12);
    Axindices.push_back(14);
    Axindices.push_back(15);

	Axindices.push_back(16);
    Axindices.push_back(17);
    Axindices.push_back(18);
    Axindices.push_back(16);
    Axindices.push_back(18);
    Axindices.push_back(19);

	Axindices.push_back(20);
    Axindices.push_back(21);
    Axindices.push_back(22);
    Axindices.push_back(20);
    Axindices.push_back(22);
    Axindices.push_back(23);

	glGenVertexArrays(1,&AxmodelVAO);
    glGenBuffers(1,&AxindexVBO);
	
	//bind our model VAO
    glBindVertexArray(AxmodelVAO);

	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,AxindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,Axindices.size()*sizeof(GLushort),Axindices.data(),GL_STATIC_DRAW);

	glBindVertexArray(0);
}


//*/



//for a given ray, find out if that ray hits the mesh then return the closest hit point
bool pho::Mesh::findIntersection(int rotateMode, glm::mat4 modelMatrix, glm::mat4 rayMatrix, glm::vec3& foundPoint) {
	glm::vec3 rayOrigin;
	glm::vec3 rayDirection;

//	std::vector<vec3> Intersection; //triangleとの交点の退避場所
	vec3 Intersection; //triangleとの交点の退避場所
	int IntersectNum = 0;

	double epsilon = 0.000001;

	rayOrigin = glm::vec3(rayMatrix[3]);  //pick up position of ray from the matrix
	rayDirection = glm::mat3(rayMatrix)*glm::vec3(0,0,-1);  //direction of ray
	rayDirection = glm::normalize(rayDirection);

	glm::vec4 v0,v1,v2; //three vertices of the triangle to be tested

	//for (std::vector<Face>::size_type i=0;i < 36; i+=3) { //cube
	for (std::vector<Face>::size_type i=0;i < 12; i+=3) { //tetrahedron
		v0 = glm::vec4(vertices[indices[i]],1.0f); //vertices are kept in another vector object
		v1 = glm::vec4(vertices[indices[i+1]],1.0f); //I fetch them using indices
		v2 = glm::vec4(vertices[indices[i+2]],1.0f);

		v0 = modelMatrix*v0;  //Transform the vertex from model space ---> world space
		v1 = modelMatrix*v1;
		v2 = modelMatrix*v2;
		
		//currently disabled because glm says barycentric but they are not barycentric
		/*if there's a hit
		if (glm::intersectRayTriangle(rayOrigin,rayDirection,glm::vec3(v0),glm::vec3(v1),glm::vec3(v2),intersection)) {
			static int count  = 0;

			//convert intersection point from barycentric to cartesian
			//currently does not work because of glm::intersectRayTriangle bug
			foundPoint = intersection;
		
			std::cout << count << " - Found x: \t" <<foundPoint.x << " \t" << foundPoint.y << " \t" << foundPoint.z << '\n'; 
			count++;
			return true;
		}*/

		if (rayTriangleIntersection(glm::vec3(v0),glm::vec3(v1),glm::vec3(v2),rayMatrix,epsilon,foundPoint)) {
			if(IntersectNum == 0){ //初めにrayTriangleIntersectionがtrueになった点をIntersectionに退避する
				Intersection = foundPoint;
				selectedtriangle = i; //debug用
			} else {selectedtriangle2 = i;} //debug用
			IntersectNum++;
		}

	}
//	if (IntersectNum > 2) std::cout << IntersectNum << '\n';
	if(IntersectNum >= 1){
//		if (IntersectNum > 1 && glm::distance(vec3(0,0,0),Intersection[0]) <= glm::distance(vec3(0,0,0),Intersection[1])){
//		if ((IntersectNum > 1) && (glm::distance(vec3(0,0,0),Intersection) <= glm::distance(vec3(0,0,0),foundPoint))){ //indexの番号が若い方が原点に近いとき
		if ((IntersectNum > 1) && (glm::distance(vec3(rayMatrix[3]),Intersection) <= glm::distance(vec3(rayMatrix[3]),foundPoint))){ //indexの番号が若い方が原点に近いとき
			foundPoint = Intersection;
//			std::cout << selectedtriangle << '\n';
			Tselectedtriangle = selectedtriangle;
		} else if (IntersectNum > 1) {
//			std::cout << selectedtriangle2 << '\n';
			Tselectedtriangle = selectedtriangle2;
		}

		//rotateModeがONのときの処理
		if (rotateMode >= 1){
			if (rotateMode == 1){
				FixedTriangle = Tselectedtriangle; //初めに選択しているtriangleを固定(triangleのindexを退避)

			}else if (rotateMode == 2){
				//rayと垂線の角度を求める
				v0 = modelMatrix*glm::vec4(vertices[indices[FixedTriangle]],1.0f); //Transform the vertex from model space ---> world space
				v1 = modelMatrix*glm::vec4(vertices[indices[FixedTriangle+1]],1.0f);
				v2 = modelMatrix*glm::vec4(vertices[indices[FixedTriangle+2]],1.0f);
				
				normal = glm::cross(vec3(v0-v1),vec3(v2-v1));
				normal = glm::normalize(normal);
				//std::cout << "normal\t" << normal[0] << "\t" << normal[1] << "\t" << normal[2] << std::endl;
				float vangle = glm::angle(normal,rayDirection); //角度
//				std::cout << "vangle\t" << vangle << std::endl;

				//if (vangle >= 50 && vangle <= 140){
				//if (vangle >= 70 && vangle <= 110){
				if (vangle >= 80 && vangle <= 100){
//					std::cout << "over angle" << std::endl;
					return false;
				}

			}else if (Tselectedtriangle != FixedTriangle){
				std::cout << "defferent triangle" << std::endl;
				return false;
			}
		}

		return true;
	}
	//std::cout << "not IntersectNum" << std::endl;
	return false;

}
///*
bool pho::Mesh::findSphereIntersection(glm::mat4 modelMatrix, glm::mat4 rayMatrix,glm::vec3& foundPoint,float& foundDistance,glm::vec3& foundNormal) {
	glm::vec3 rayOrigin;
	glm::vec3 rayDirection;
	glm::vec3 sphereOrigin;
	float radius;

	rayOrigin = glm::vec3(rayMatrix[3]);  //pick up position of ray from the modelmatrix
	rayDirection = glm::mat3(rayMatrix)*glm::vec3(0,0,-1);  //direction of ray
	rayDirection = glm::normalize(rayDirection);
	sphereOrigin = glm::vec3(modelMatrix[3]);
	radius = glm::length(farthestVertex); //since the farthest vertex is from the origin 0,0,0 then lehgth() should just give us the sphere radius

	if (raySphereIntersection(rayDirection,rayOrigin,sphereOrigin,radius,foundPoint,foundDistance,foundNormal))
	{
		return true;	
	} else return false;
}
//*/
bool pho::Mesh::rayTriangleIntersection(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::mat4 rayMatrix, float epsilon, glm::vec3 &intersection) {

	glm::vec3 lineDirection = glm::mat3(rayMatrix)*glm::vec3(0,0,-1);  //direction of ray
	glm::vec3 lineOrigin = glm::vec3(rayMatrix[3][0],rayMatrix[3][1],rayMatrix[3][2]);

	glm::vec3 e1,e2,p,s,q;

	float t,u,v,tmp;

	e1 = v1-v0;
	e2 = v2-v0;
	p=glm::cross(lineDirection,e2);
	tmp = glm::dot(p,e1);

	if((tmp > -epsilon) && (tmp < epsilon)) {
		return false;
	}

	tmp = 1.0f/tmp;
	s = lineOrigin - v0;

	u=tmp*glm::dot(s,p);
	if (u < 00 || u > 1.0) {
		return false;
	}

	q = glm::cross(s,e1);
	v = tmp * glm::dot(lineDirection,q);

	if(v<0.0||v>1.0) {
		return false;
	}

	if (u+v > 1.0) {
		return false;
	}

	t = tmp * glm::dot(e2,q);

	//info here (probably barycentric coordinates)

	//intersection point
	intersection = lineOrigin + t*lineDirection;
	return true;

}
///*
inline float pho::Mesh::sum(const vec3& v)
{
return v[0] + v[1] + v[2];
}

bool pho::Mesh::raySphereIntersection(const vec3& raydir, const vec3& rayorig,const vec3& pos,const float& rad, vec3& hitpoint,float& distance, vec3& normal)
{
	float a = sum(raydir*raydir);
	float b = sum(raydir * (2.0f * ( rayorig - pos)));
	float c = sum(pos*pos) + sum(rayorig*rayorig) -2.0f*sum(rayorig*pos) - rad*rad;
	float D = b*b + (-4.0f)*a*c;

	// If ray can not intersect then stop
	if (D < 0)
		return false;
	D=sqrtf(D);

	// Ray can intersect the sphere, solve the closer hitpoint
	float t = (-0.5f)*(b+D)/a;
	if (t > 0.0f)
	{
		distance=sqrtf(a)*t;
		hitpoint=rayorig + t*raydir;
		normal=(hitpoint - pos) / rad;
	}
	else
		return false;
	return true;
}
//*/