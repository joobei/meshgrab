/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "GL/glew.h"				//for GL extensions checking
#define GLFW_NO_GLU
#include "GL/glfw.h"				//for the context
#include "GL/glut.h"
#include "glm.hpp"                 //for matrices/math
#include "gtc/type_ptr.hpp"         //glm
#include "gtc/matrix_transform.hpp"  //glm
//#include "util.h"   //shader loading functions
#include "gtx/quaternion.hpp"
#include "boost/bind.hpp"
#include <functional>
/*
#include <boost/math/special_functions/round.hpp>
#include <boost/static_assert.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer.hpp> //for stopwatch and time output
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
*/
/*
#include <deque>
#include <iostream>
#include <boost/array.hpp>
#include "spuc/generic/running_average.h"
*/
#include "photonio.h"
#include "eventQueue.h"
#include "asio.h"

#include "stdlib.h"
#include "direct.h"
#include <mmsystem.h>

#pragma comment( lib, "glew32.lib" )
#pragma comment( lib, "GLFW.lib" ) 
/*
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")
*/

//const int WINDOW_SIZE_X = 800;
//const int WINDOW_SIZE_Y = 600;
//const int WINDOW_SIZE_X = 1440;
//const int WINDOW_SIZE_Y = 1080;

//const int WINDOW_SIZE_X = 1200;
//const int WINDOW_SIZE_Y = 900;

const int WINDOW_SIZE_X = 1280;
const int WINDOW_SIZE_Y = 800;

using namespace glm;

#define FLICKJUDGETIME 0.1f
#define FLICKJUDGEDISTANCE 0.8f
//#define FLICKJUDGEDISTANCE 0.2f

#define LOGTIME { Logger::Writetime(); }
#define LOGSTRING( message ) { Logger::WriteString( message ); }
#define LOG( message ) { Logger::Write( message ); }
#define LOGINT( message ) { Logger::Writeint( message ); }
#define LOGLINE { Logger::Writeln(); }

boost::timer t; // �^�C�}�[�̊J�n

glm::mat4 modelMatrix;  //to move our object around
glm::mat4 viewMatrix;   //to move our camera around
glm::mat4 projectionMatrix; //for perspective projection
glm::mat4 pvm;
glm::mat4 raymodelMatrix;  //to move our object around
glm::mat4 rayviewMatrix;   //to move our camera around
glm::mat4 rayprojectionMatrix; //for perspective projection
glm::mat4 raypvm;

glm::mat4 IntersectionModelMatrix;

glm::mat4 tmpraymodelMatrix; //raymodelMatrix���[���I��rotate��������(findIntersection�Ŏg�p)
/*
//using namespace glm;
mat4 viewTranslate;
mat4 viewRotateX;
mat4 rayviewTranslate;
mat4 rayviewRotateX;
*/
glm::mat4 previousModelMatrix;
glm::mat4 previousRaymodelMatrix;
glm::mat4 previousRaymodelMatrix2;

glm::mat4 initOrientation; //�I�����̃��V�[�o�̎p��(Scaled HOMER mode)
//glm::mat4 initPosition; //�I�����̃��V�[�o�̈ʒu(Scaled HOMER mode)
glm::vec3 initPosition; //�I�����̃��V�[�o�̈ʒu(Scaled HOMER mode)
glm::vec3 prevPosition;
glm::mat4 prevModelMatrix;
glm::mat4 initModelMatrix; //������modelMatrix(Scaled HOMER mode)

vec4 firstRayPosition; //�ړ��O��ray�̐�[�̍��W
vec4 endRayPosition; //�ړ����ray�̐�[�̍��W

vec4 flickRot;
vec3 flickMove;
vec4 limitRot; //��]�̌��E���Ƀt���b�N���s���ۂɎg�p

int translateMode = 0;
int RotateTranslateMode = 0;
int rotateMode = 0;
int RotTransFlickMode = 0;
int translateFlickMode = 0;
int rotateFlickMode = 0;
int ScaledHomerMode = 0;
int rayVerticesCount = 0;
int tflag = 0;
int tcount = 0;
//int FlickEnable = 0;
int FlickEnable = 1;

//Scaled HOMER�����s�\�ȏ�Ԃɂ���t���O
//int ScaledHomerEnable = 0; 
//int ScaledHomerEnable = 1;

int flickStop = 0; //�t���b�N�������I������ۂɎg�p
int RotateLimitFlag = 0; //��]�̌��E���Ɏg�p����t���O
int NextTaskFlg = 0; //���̃^�X�N�Ɉړ�����ۂ̃t���O

int targetFlg = 0;
int targetNumber = 1;
int LogStartFlg = 0; //log�̋L�^���J�n����ۂ̃t���O

//�{�^���̉�����Ă���t���[���̃J�E���g
int A_count = 0;
int B_count = 0;
int AB_count = 0;
int idle_count = 0;
int two_count = 0;

float rayDistance;
float grabbedDistance = 0;

float randRot = 0;
vec3 randAxis = vec3(1,1,1); //�����l��^���Ă���(0���ƃI�u�W�F�N�g��������)
int rotateNum = 0;
int TargetNum[24];
int PracticeNum = 0;

int restartFlg = 0;
int DockingCompFlg = 0;
//int DockingTaskFlg = 0;
//int RotationTaskFlg = 0;

int PedalPushed = 0;

GLuint rayVAO;
GLuint raypositionVBO;
GLuint rayindexVBO;
GLuint raycolorVBO;

GLuint shaderProgram;
GLuint shaderProgramPVM;  //position of the Uniform (for the matrices) after linking
GLuint shaderProgramColor;

GLuint flatShader;
GLuint flatShaderPvm;
GLuint flatShaderColor;

GLuint targetShader;
GLuint targetShaderPvm;
GLuint targetShaderColor;

GLuint picking();
GLuint get_object_id();
void generate_frame_buffer_objects();
void generate_pixel_buffer_objects();

GLuint pbo_a, pbo_b, fbo, tex;

using glm::mat4;
using glm::vec3;
using glm::vec4;


Engine::Engine():
calibrate(false),
	eventQueue(),
	appInputState(idle),
	_serialserver(serialioservice,115200,"COM27",&eventQueue,&ioMutex),
	wii(false)
{
#define SIZE 30                     //Size of the moving average filter
	accelerometerX.set_size(SIZE);  //Around 30 is good performance without gyro
	accelerometerY.set_size(SIZE);
	accelerometerZ.set_size(SIZE);
	magnetometerX.set_size(SIZE);
	magnetometerY.set_size(SIZE);
	magnetometerZ.set_size(SIZE);

	errorLog.open("error.log",std::ios_base::app);

	//Polhemus
	serialThread = new boost::thread(boost::bind(&boost::asio::io_service::run, &serialioservice));
///*
	wii=remote.Connect(wiimote::FIRST_AVAILABLE);

	if (wii) { 	remote.SetLEDs(0x01); }
	else { errorLog << "WiiRemote Could not Connect \n"; }
//*/
	appInputState = idle; 
	appMode = rayCasting;
	//appMode = rayCastingOfSphere;
	rotTechnique = screenSpace;
	taskMode = NonTask;

	prevMouseWheel = 0;
	gyroData = false;
	objectHit=false;
	sphereHit=false;

	axisChange[0][0] = 1; axisChange[0][1] =  0;  axisChange[0][2] =  0;
	axisChange[1][0] = 0; axisChange[1][1] =  0;  axisChange[1][2] =  1;
	axisChange[2][0] = 0; axisChange[2][1] =  -1;  axisChange[2][2] = 0;

	tf = new boost::posix_time::time_facet("%d-%b-%Y %H:%M:%S");
	deltat = -1.0f; //so as to not repeat keystrokes

	perspective = 80.0f;
}


void Engine::modelinit() {

	//read our shaders from disk and uploade them to opengl
	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER,readTextFile("shaders/shader.vert"));
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER,readTextFile("shaders/shader.frag"));
	
	//compile/link shader program from the two shaders
	shaderProgram = CreateProgram(vertexShader, fragmentShader);
	
	//Get the position of the Uniform (for the matrices) after linking
	shaderProgramPVM = glGetUniformLocation(shaderProgram,"pvm");
	shaderProgramColor = glGetUniformLocation(shaderProgram,"baseColor");

	//Prepare the matrices
	projectionMatrix = glm::perspective(45.0f, (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y,0.1f,1000.0f); //create perspective matrix
//	viewMatrix = glm::translate(glm::mat4(),vec3(0,0,-2)); //a matrix that translates the camera backwards (i.e. world forward)

	//viewMatrix = translate(mat4(),vec3(0,0,-4));
	viewMatrix = translate(mat4(),vec3(0,0,0));
	modelMatrix = glm::translate(modelMatrix, vec3(1.3,1.3,-15)); //object�̈ʒu
//	modelMatrix = glm::rotate(modelMatrix, 50.0f, vec3(-1.0f, -1.0f, 0.0f)); //�K���ȉ�](�������)

	//initModelMatrix = modelMatrix; //scaled HOMER�p

	cursor.model();
	cursor.sphere();
	cursor.wiremodel();
	cursor.vertex();
	cursor.plane();
	cursor.simpleShadow();
	cursor.Triangle();
	cursor.Intersection();

}


void Engine::rayinit() {

	//Prepare the matrices
	rayprojectionMatrix = glm::perspective(45.0f, (float)WINDOW_SIZE_X/(float)WINDOW_SIZE_Y,0.1f,1000.0f); //create perspective matrix
	//rayviewMatrix = translate(mat4(),vec3(0,0,-4));
	rayviewMatrix = translate(mat4(),vec3(0,0,0));
	raymodelMatrix = translate(mat4(),vec3(0,0,-9));
	//raymodelMatrix = translate(mat4(),vec3(0,0,0));
	
	//hold vertex data to upload to VBO's
	//std::vector<GLushort> indices;
	std::vector<vec3> vertices;
	std::vector<vec3> colors;

	/*
	//���_�𒆐S�ɉ�]
	vertices.push_back(vec3(0, 0, -4)); //�ړ�����_
	colors.push_back(vec3(0.0,1.0,0.0));
    vertices.push_back(vec3(0, 0, 0));
	colors.push_back(vec3(1.0,0.0,0.0));

	indices.push_back(0);
    indices.push_back(1);
	*/

	float radius = 0.01f;
	for(float i = 0; i < 6.38 ; i+=0.1)  //generate vertices at positions on the circumference from 0 to 2*pi 
	{
		vertices.push_back(glm::vec3(3*radius*cos(i),3*radius*sin(i),0));	
		colors.push_back(vec3(0.5,1.0,0.0));
		rayVerticesCount++;
		//vertices.push_back(glm::vec3(radius*cos(i),radius*sin(i),-8));
		vertices.push_back(glm::vec3(radius*cos(i),radius*sin(i),-16));
		colors.push_back(vec3(0.0,0.0,0.0));
		rayVerticesCount++;
	}


	//upload to GPU
	glGenVertexArrays(1,&rayVAO);
    glGenBuffers(1,&raypositionVBO);
    glGenBuffers(1,&rayindexVBO);
    glGenBuffers(1,&raycolorVBO);
	
	//bind our model VAO
    glBindVertexArray(rayVAO);
	/*
	//bind it's index VBO and upload data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,rayindexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLushort),indices.data(),GL_STATIC_DRAW);
	*/
	//bind the position VBO and upload data
	glBindBuffer(GL_ARRAY_BUFFER,raypositionVBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),vertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(vertexLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(vertexLoc);

	//bind the color VBO and upload data
    glBindBuffer(GL_ARRAY_BUFFER,raycolorVBO);
    glBufferData(GL_ARRAY_BUFFER,colors.size()*3*sizeof(GLfloat),colors.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc,3,GL_FLOAT,GL_FALSE,0,0); 
    glEnableVertexAttribArray(colorLoc);
    
	glBindVertexArray(0);
}


void Engine::framebufferinit() {

	/* bind the frame buffer */ 
	glBindFramebuffer(GL_FRAMEBUFFER, fbo); 
	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER,readTextFile("shaders/offscreen.vert"));
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER,readTextFile("shaders/offscreen.frag"));
	
	//compile/link shader program from the two shaders
	flatShader = CreateProgram(vertexShader, fragmentShader);
	
	//Get the position of the Uniform (for the matrices) after linking
	flatShaderPvm = glGetUniformLocation(flatShader,"pvm");
	flatShaderColor = glGetUniformLocation(flatShader,"baseColor");
}


void Engine::targetinit(){

	//read our shaders from disk and uploade them to opengl
	GLuint vertexShader = CreateShader(GL_VERTEX_SHADER,readTextFile("shaders/targetshader.vert"));
	GLuint fragmentShader = CreateShader(GL_FRAGMENT_SHADER,readTextFile("shaders/targetshader.frag"));
	
	//compile/link shader program from the two shaders
	targetShader = CreateProgram(vertexShader, fragmentShader);
	
	//Get the position of the Uniform (for the matrices) after linking
	targetShaderPvm = glGetUniformLocation(targetShader,"pvm");
	targetShaderColor = glGetUniformLocation(targetShader,"baseColor");

}


void Engine::checkEvents() {

#define FACTOR 0.5f
#define FLICKCOUNT 100
#define FLICKDECAY 0.95f
//#define FACTOR 0.1f
//#define FLICKCOUNT 500
//#define FLICKDECAY 0.995f

	if (LogStartFlg == 1){
		//�^�X�N�̏��߂̂Ƃ�(0�b����J�n���邽��)
		if (restartFlg == 1){
			restartFlg = 0;
			t.restart();
		}
		LOG(t.elapsed()); //���O�̋L�^���J�n����Ă���Ƃ��Ɏ����̋L�^���s��(2�s�ڂ���)
	}


	previousModelMatrix = modelMatrix;
	previousRaymodelMatrix = raymodelMatrix;

	//�I�u�W�F�N�g���I������Ă��邩�ǂ����̔���
	switch(appMode)  {
		case rayCasting:
		case ScaledHOMER:
			if (cursor.findIntersection(0,modelMatrix,raymodelMatrix,objectIntersectionPoint)){
				objectHit=true;
			}else{
				objectHit=false;
			}
			break;
		case rayCastingOfSphere:
			if (cursor.findSphereIntersection(modelMatrix,raymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal)){
				objectHit=true;
			}else{
				objectHit=false;
			}
			break;
	}
	

	checkKeyboard();
	checkPolhemus();
	checkWiiMote();
	checkSpaceNavigator();


	switch(appMode)  {
		case rayCasting:
			if (RotateTranslateMode >= 1) cursor.findIntersection(RotateTranslateMode,modelMatrix,previousRaymodelMatrix,objectIntersectionPoint); //ray�ړ��O(������̕������m�H)
			if (translateMode >= 1) cursor.findIntersection(translateMode,modelMatrix,previousRaymodelMatrix,objectIntersectionPoint);
			if (rotateMode >= 1) cursor.findIntersection(rotateMode,modelMatrix,previousRaymodelMatrix,objectIntersectionPoint);
			break;
		case rayCastingOfSphere:
			if (RotateTranslateMode >= 1) cursor.findSphereIntersection(modelMatrix,previousRaymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal); //ray�ړ��O(������̕������m�H)
			if (translateMode >= 1) cursor.findSphereIntersection(modelMatrix,previousRaymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal);
			if (rotateMode >= 1) cursor.findSphereIntersection(modelMatrix,previousRaymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal);
			break;
	}

	//std::cout << objectIntersectionPoint[0] << '\t' << objectIntersectionPoint[1] << '\t' << objectIntersectionPoint[2] << std::endl;

	/*
	if (glfwGetKey('9') == GLFW_PRESS) {
		mat4 rot = glm::rotate(mat4(), FACTOR*2, vec3(0,0,1));
		mat4 tmpMatrix = rot*tmpMatrix;
		tmpMatrix[3][0] = modelMatrix[3][0];
		tmpMatrix[3][1] = modelMatrix[3][0];
		tmpMatrix[3][2] = modelMatrix[3][0];
		modelMatrix = tmpMatrix;
	}
	*/



	vec4 modelIntersection;

	//--��]�{���i�̏���--//
	if (RotateTranslateMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray�ړ��O(������̕������m�H)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		//modelRotationinit(modelIntersection[0]-modelMatrix[3][0], modelIntersection[1]-modelMatrix[3][1], modelIntersection[2]-modelMatrix[3][2]);
		RotateTranslateMode = 2;
	}else if (RotateTranslateMode == 2 && raymodelMatrix != previousRaymodelMatrix){ //raymodelMatrix���ω����Ă���Ƃ�

		//��_��ϊ����ėp����ꍇ
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);  //direction of ray
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f); //ray���猩����_�̍��W
		modelIntersection = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);

		//�t���b�N����(��������)
		if (FlickEnable == 1){ 
			if (tflag == 0){ //�v���O��ray�̐�[�̏������W(�ړ��O)���L��(���̃t�F�[�Y�ł͈ړ����ray)
//				t.restart();
				firstRayPosition = modelIntersection;
				tflag = 1;
			}
		}

		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelTranslateRotation(modelIntersection[0], modelIntersection[1], modelIntersection[2]); //object rotate
		//modelTranslateRotation(modelIntersection[0]-modelMatrix[3][0], modelIntersection[1]-modelMatrix[3][1], modelIntersection[2]-modelMatrix[3][2]); //object rotate

		switch(appMode)  {
			case rayCasting:
				//�I�u�W�F�N�g��ray�̊p�x�����E�ł��邩�ǂ���
				if (cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint)){
				}else{
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //ray�ƃI�u�W�F�N�g�𓯎��ɓ������Ƃ�
					//raymodelMatrix = previousRaymodelMatrix; //ray�ƃI�u�W�F�N�g���Œ肷��Ƃ�
				}
				break;
			case rayCastingOfSphere:
				//sphere��ray�̊p�x�����E�ł��邩�ǂ���
				float vangle = glm::angle(sphereIntersectionNormal,RayDir);
				//std::cout << "vangle\t" << vangle << std::endl;
				if (vangle >= 85 && vangle <= 95){
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //ray�ƃI�u�W�F�N�g�𓯎��ɓ������Ƃ�
				}
				break;
		}


	//�t���b�N����(�I�u�W�F�N�g�̃t���b�N�ɂ��ړ�)
	} else if (FlickEnable == 1 && RotTransFlickMode == 1){
		flickRot[3] = flickRot[3]*FLICKDECAY;
		flickMove = vec3(flickMove[0]*FLICKDECAY,flickMove[1]*FLICKDECAY,flickMove[2]*FLICKDECAY);
		modelMatrix = glm::rotate(modelMatrix, flickRot[3], vec3(flickRot[0], flickRot[1], flickRot[2]));
		modelMatrix[3][0] = modelMatrix[3][0] + flickMove[0];
		modelMatrix[3][1] = modelMatrix[3][1] + flickMove[1];
		modelMatrix[3][2] = modelMatrix[3][2] + flickMove[2];
		tcount++;
//		if (tcount == FLICKCOUNT){
		if (tcount == FLICKCOUNT || flickStop == 1){ //�{�^������ɂ���ăt���b�N���~
			std::cout << "flick end"  << std::endl;
			RotTransFlickMode = 0;
			tcount = 0;
			flickStop = 0; //�t���b�N�̒�~�t���O�̏�����
		}
	}



	
	//--���i�݂̂̏���--//
	if (translateMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		//rayDistance = sqrt(pow(modelIntersection[0]-raymodelMatrix[3][0],2)+pow(modelIntersection[1]-raymodelMatrix[3][1],2)+pow(modelIntersection[2]-raymodelMatrix[3][2],2));
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray�ړ��O(������̕������m�H)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		translateMode = 2;
	} else if (translateMode == 2 && raymodelMatrix != previousRaymodelMatrix){

		vec4 first = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f);
		vec4 end = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);

		//�t���b�N����(��������)
		if (FlickEnable == 1){ //�v���O��ray�̐�[�̏������W(�ړ��O)���L��(���̃t�F�[�Y�ł��ړ��O��ray)
			if (tflag == 0){
//				t.restart(); //���O�Ń^�C�}�[���g�p���Ă��邽��
				firstRayPosition = first;
				tflag = 1;
			}
		}

		//�I�u�W�F�N�g�̕��i
		modelMatrix[3][0] = modelMatrix[3][0] + end[0] - first[0];
		modelMatrix[3][1] = modelMatrix[3][1] + end[1] - first[1];
		modelMatrix[3][2] = modelMatrix[3][2] + end[2] - first[2];

		/*
		//�t���b�N����(����)
		if (FlickEnable == 1){
			if (t.elapsed() > FLICKJUDGETIME){
				endRayPosition = end;
				float rayMoveDistance = sqrt(pow(endRayPosition[0]-firstRayPosition[0],2)+pow(endRayPosition[1]-firstRayPosition[1],2)+pow(endRayPosition[2]-firstRayPosition[2],2));

				if (rayMoveDistance >= FLICKJUDGEDISTANCE){
					flickMove = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);
					translateMode = 0; //�I�u�W�F�N�g����_���痣���
					translateFlickMode = 1;
					tcount = 0; //�J�E���g�̏�����
				}
				t.restart(); // �^�C�}�[�̃��Z�b�g
				tflag = 0;
			}
		}
		*/
		flickMove = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);

		/*
		//ray�̒����̒���
		if (grabbedDistance != 0){
			modelMatrix[3][0] = modelMatrix[3][0] + RayDir[0]*grabbedDistance;
			modelMatrix[3][1] = modelMatrix[3][1] + RayDir[1]*grabbedDistance;
			modelMatrix[3][2] = modelMatrix[3][2] + RayDir[2]*grabbedDistance;
			rayDistance -= grabbedDistance;
			grabbedDistance = 0;
		}
		*/

		switch(appMode)  {
			case rayCasting:
				//�I�u�W�F�N�g��ray�̊p�x�����E�ł��邩�ǂ���
				if (cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint)){
				}else{
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //ray�ƃI�u�W�F�N�g�𓯎��ɓ������Ƃ�
					//raymodelMatrix = previousRaymodelMatrix; //ray�ƃI�u�W�F�N�g���Œ肷��Ƃ�
				}
				break;
			case rayCastingOfSphere:
				//sphere��ray�̊p�x�����E�ł��邩�ǂ���
				float vangle = glm::angle(sphereIntersectionNormal,RayDir);
				std::cout << "vangle\t" << vangle << std::endl;
				if (vangle >= 85 && vangle <= 95){
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //ray�ƃI�u�W�F�N�g�𓯎��ɓ������Ƃ�
				}
				break;
		}


//	/*
	//�t���b�N����(�I�u�W�F�N�g�̃t���b�N�ɂ��ړ�)
	} else if (FlickEnable == 1 && translateFlickMode == 1){
		flickMove = vec3(flickMove[0]*FLICKDECAY,flickMove[1]*FLICKDECAY,flickMove[2]*FLICKDECAY);
		modelMatrix[3][0] = modelMatrix[3][0] + flickMove[0];
		modelMatrix[3][1] = modelMatrix[3][1] + flickMove[1];
		modelMatrix[3][2] = modelMatrix[3][2] + flickMove[2];
		tcount++;
//		if (tcount == FLICKCOUNT){
		if (tcount == FLICKCOUNT || flickStop == 1){ //�{�^������ɂ���ăt���b�N���~
			std::cout << "translate flick end"  << std::endl;
			translateFlickMode = 0;
			tcount = 0;
			flickStop = 0;
		}
//	*/
	}



	
	//--��]�݂̂̏���--//
	if (rotateMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		//rayDistance = sqrt(pow(modelIntersection[0]-raymodelMatrix[3][0],2)+pow(modelIntersection[1]-raymodelMatrix[3][1],2)+pow(modelIntersection[2]-raymodelMatrix[3][2],2));
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray�ړ��O(������̕������m�H)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		rotateMode = 2;
	} else if (rotateMode == 2 && raymodelMatrix != previousRaymodelMatrix){
//		modelIntersection = vec4(objectIntersectionPoint,1.0f);
//		modelIntersection = inverse(modelMatrix)*modelIntersection;
		

		//��_��ϊ����ėp����ꍇ
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);  //direction of ray
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f); //ray���猩����_�̍��W
		modelIntersection = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);


		vec4 first = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		//�t���b�N����(��������)
		if (FlickEnable == 1){ //�v���O��ray�̐�[�̏������W(�ړ��O)���L��(���̃t�F�[�Y�ł��ړ��O��ray)
			if (tflag == 0){
//				t.restart();
				firstRayPosition = first;
				tflag = 1;
			}
		}

		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotation(modelIntersection[0], modelIntersection[1], modelIntersection[2]);

		/*
		//�I�u�W�F�N�g��ray�̊p�x�����E�ł��邩�ǂ���
		if (!(cursor.findIntersection(rotateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint))){
			rotateMode = 0; //rotateMode�̏I��

			//���E���̃t���b�N����
			if (!(limitRot[0] == 0 && limitRot[1] == 0 && limitRot[2] == 0)) { //�����s��łȂ��Ƃ�(�I�����ꂽ���_�ŉ�]���s�\�łȂ��Ƃ�)
				flickRot = limitRot;
				flickRot[3] = flickRot[3]/4;
				tcount = 0; //�J�E���g�̏�����
				RotateLimitFlag = 1;
				//FlickEnable = 1; rotateFlickMode = 1; //�ꎞ�I�Ƀt���b�N��������
			} else std::cout << "rot impossible"  << std::endl;
		}
		*/

	//�t���b�N����(�I�u�W�F�N�g�̃t���b�N�ɂ��ړ�)
	} else if (FlickEnable == 1 && rotateFlickMode == 1){
		flickRot[3] = flickRot[3]*FLICKDECAY;
		modelMatrix = glm::rotate(modelMatrix, flickRot[3], vec3(flickRot[0], flickRot[1], flickRot[2]));
		tcount++;
//		if (tcount == FLICKCOUNT){
		if (tcount == FLICKCOUNT || flickStop == 1){ //�{�^������ɂ���ăt���b�N���~
			std::cout << "rotate flick end"  << std::endl;
			rotateFlickMode = 0;
			tcount = 0;
			flickStop = 0;
		}

	//���E���̃t���b�N����
	} else if (RotateLimitFlag == 1){
		flickRot[3] = flickRot[3]*FLICKDECAY;
		modelMatrix = glm::rotate(modelMatrix, flickRot[3], vec3(flickRot[0], flickRot[1], flickRot[2]));
		tcount++;
		std::cout << tcount << std::endl;
		if (tcount == FLICKCOUNT || flickStop == 1){ //�{�^������ɂ���ăt���b�N���~
			tcount = 0;
			flickStop = 0;
			RotateLimitFlag = 0;
		}
	}


	//ray�̒����̒���(���i)
	vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);
	RayDir = glm::normalize(RayDir);
	if (grabbedDistance != 0 && translateMode >= 1){
		modelMatrix[3][0] = modelMatrix[3][0] + RayDir[0]*grabbedDistance;
		modelMatrix[3][1] = modelMatrix[3][1] + RayDir[1]*grabbedDistance;
		modelMatrix[3][2] = modelMatrix[3][2] + RayDir[2]*grabbedDistance;
		rayDistance -= grabbedDistance;
		if (grabbedDistance == -0.1f) LOGSTRING("Up");
		if (grabbedDistance == 0.1f) LOGSTRING("Down");
		grabbedDistance = 0;
	}else LOGSTRING("None");


	/*
	//�I�u�W�F�N�g��Z����]
	if (grabbedDistance != 0 && translateMode >= 1){
		mat4 rot = glm::rotate(mat4(), FACTOR*2, vec3(0,0,1));
		mat4 tmpMatrix = rot*tmpMatrix;
		tmpMatrix[3][0] = modelMatrix[3][0];
		tmpMatrix[3][1] = modelMatrix[3][0];
		tmpMatrix[3][2] = modelMatrix[3][0];
		modelMatrix = tmpMatrix;
		if (grabbedDistance == -0.1f) LOGSTRING("Up");
		if (grabbedDistance == 0.1f) LOGSTRING("Down");
		grabbedDistance = 0;
	}else LOGSTRING("None");
	*/


	//--Scaled HOMER�̏���--//
	if (ScaledHomerMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray�ړ��O(������̕������m�H)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		//ScaledHomerMode = 2;
	} else if (ScaledHomerMode == 2 && raymodelMatrix != previousRaymodelMatrix){

		/*
		vec4 first = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f);
		vec4 end = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);

		//�I�u�W�F�N�g�̕��i
		modelMatrix[3][0] = modelMatrix[3][0] + end[0] - first[0];
		modelMatrix[3][1] = modelMatrix[3][1] + end[1] - first[1];
		modelMatrix[3][2] = modelMatrix[3][2] + end[2] - first[2];

		flickMove = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);




		//�I�u�W�F�N�g��ray�̊p�x�����E�ł��邩�ǂ���
		if (!(cursor.findIntersection(translateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint))){
			translateMode = 0; //translateMode�̏I��
		}
		*/
	}




	//ray���~�܂����Ƃ�(keyborad�p)
	if (raymodelMatrix == previousRaymodelMatrix){
		tflag = 0;
	}
}


void Engine::checkKeyboard() {

	//ray,model�Ȃǂ̏�����
	if (glfwGetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {

		switch(taskMode){
		case DockingTask:
			modelMatrix = glm::translate(mat4(),vec3(1.3,1.3,-15));
			break;
		case RotationTask:
			modelMatrix = glm::translate(mat4(), vec3(0,0,-5));
			break;
		default:
			modelMatrix = glm::translate(mat4(),vec3(1.3,1.3,-15));
			break;
		}
		
		raymodelMatrix = translate(mat4(),vec3(0,0,-4)); //keyboard�p
		translateMode = 0;
		//FlickEnable = 0;
		RotateTranslateMode = 0;
		RotTransFlickMode = 0;
	}

	//log�̋L�^�̊J�n(���K�p�^�X�N)
	if (LogStartFlg == 0 && glfwGetKey('9') == GLFW_PRESS) {
		LogStartFlg = 1;
		taskMode = PracticeTask;
		//taskshuffle(); //�^�X�N�̃V���b�t��

		//������
		modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
		appInputState = idle;
		RotateTranslateMode = 0;
		translateMode = 0;
		rotateMode = 0;
		RotTransFlickMode = 0;
		translateFlickMode = 0;
		rotateFlickMode = 0;
		idle_count = 1;

		targetNumber = 0;
		rotateNum = 0;
		PracticeNum = 1;

		//Logger::Fileclose();
		//Logger::Initialize("dast_log.txt"); //���O�t�@�C���̍쐬
		std::cout << "practice task start" << '\n';

		t.restart();
		LOG(t.elapsed()); //�����̋L�^(1�s��)
	}

	//log�̋L�^�̊J�n(�h�b�L���O�^�X�N)
	if (LogStartFlg == 0 && glfwGetKey('7') == GLFW_PRESS) {
		LogStartFlg = 1;
		taskMode = DockingTask;
		taskshuffle(); //�^�X�N�̃V���b�t��

		//������
		modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
		appInputState = idle;
		RotateTranslateMode = 0;
		translateMode = 0;
		rotateMode = 0;
		RotTransFlickMode = 0;
		translateFlickMode = 0;
		rotateFlickMode = 0;
		idle_count = 1;

		targetNumber = 0; ////����targetNumber++ �����s����邽��
		rotateNum = 0;
		PracticeNum = 0;

		Logger::Fileclose();
		Logger::Initialize("docking_task_log.txt"); //���O�t�@�C���̍쐬
		std::cout << "docking task start" << '\n';

		t.restart();
		LOG(t.elapsed()); //�����̋L�^(1�s��)
	}

	//log�̋L�^�̊J�n(��]�^�X�N)
	if (LogStartFlg == 0 && glfwGetKey('8') == GLFW_PRESS) {
		LogStartFlg = 1;
		taskMode = RotationTask;

		//������
		modelMatrix = glm::translate(mat4(), vec3(0,0,-5));
		appInputState = idle;
		RotateTranslateMode = 0;
		translateMode = 0;
		rotateMode = 0;
		RotTransFlickMode = 0;
		translateFlickMode = 0;
		rotateFlickMode = 0;
		idle_count = 1;

		targetNumber = 0;
		rotateNum = 1;
		PracticeNum = 0;

		//�����_���ȉ�]��^����
		randRot = rand()%180;
		randAxis[0] = rand()%101-50;
		randAxis[1] = rand()%101-50;
		randAxis[2] = rand()%101-50;
		randAxis = normalize(randAxis);

		//�����s��ȏꍇ�̏���
		while (randAxis[0] == 0 && randAxis[1] == 0 && randAxis[2] == 0) { 
			randAxis[0] = rand()%101-50;
			randAxis[1] = rand()%101-50;
			randAxis[2] = rand()%101-50;
			randAxis = normalize(randAxis);
		}

		Logger::Initialize("rotation_task_log.txt"); //���O�t�@�C���̍쐬
		std::cout << "rotation task start" << '\n';
		t.restart();
		LOG(t.elapsed()); //�����̋L�^(1�s��)
	}


	/*
	//�^�X�N�I��/���̃^�[�Q�b�g�ɐi��(keyboard�p)
	if (LogStartFlg == 1 && targetFlg == 0 && glfwGetKey(GLFW_KEY_ENTER) == GLFW_PRESS) {
		//targetNumber++;
		targetFlg = 1;
		NextTaskFlg = 1;
		//std::cout << targetFlg << "\t" << targetNumber << '\n';
	}else if (glfwGetKey(GLFW_KEY_ENTER) != GLFW_PRESS) { //�L�[����w�����ꂽ��
		targetFlg = 0;
	}
	*/


	//rayCasting mode
	if (appMode != rayCasting && glfwGetKey('4') == GLFW_PRESS) { //��x�����ʂ�悤�ɂ���
		appMode = rayCasting;
		std::cout << "rayCasting mode" << '\n';
	}
	//rayCastingOfSphere mode
	if (appMode != rayCastingOfSphere && glfwGetKey('5') == GLFW_PRESS) {
		appMode = rayCastingOfSphere;
		std::cout << "rayCastingOfSphere mode" << '\n';
	}
	//ScaledHOMER mode
	if (appMode != ScaledHOMER && glfwGetKey('6') == GLFW_PRESS) {
		appMode = ScaledHOMER;
		std::cout << "ScaledHOMER mode" << '\n';
	}


	//��]�{���i
	if (RotateTranslateMode == 0 && glfwGetKey('1') == GLFW_PRESS) { //��x�����ʂ�悤�ɂ���
		RotateTranslateMode = 1;
		translateMode = 0;
		rotateMode = 0;
		tflag = 0;
	}
	if (RotateTranslateMode >= 1 && glfwGetKey('0') == GLFW_PRESS) {
		RotateTranslateMode = 0;
	}

	//���i�̂�
	if (translateMode == 0 && glfwGetKey('T') == GLFW_PRESS) {
		translateMode = 1;
		RotateTranslateMode = 0;
		rotateMode = 0;
		tflag = 0;
	}
	if (translateMode >= 1 && glfwGetKey('Y') == GLFW_PRESS) {
		translateMode = 0;
	}
	
	//��]�̂�
	if (rotateMode == 0 && glfwGetKey('R') == GLFW_PRESS) {
		rotateMode = 1;
		RotateTranslateMode = 0;
		translateMode = 0;
		tflag = 0;
	}
	if (rotateMode >= 1 && glfwGetKey('E') == GLFW_PRESS) {
		rotateMode = 0;
	}

	//�t���b�N���g�p���邩�ǂ���
	if (FlickEnable == 0 && glfwGetKey('F') == GLFW_PRESS) {
		FlickEnable = 1;
		//std::cout << "FlickEnable" << '\n';
	}
	if (FlickEnable == 1 && glfwGetKey('G') == GLFW_PRESS) {
		FlickEnable = 0;
		//std::cout << "FlickDisable" << '\n';
	}

	//�t���b�N�̒�~����
	if ((RotTransFlickMode == 1 || translateFlickMode == 1 || rotateFlickMode == 1) && FlickEnable == 1 && glfwGetKey('Q') == GLFW_PRESS) {
		flickStop = 1;
	}
	
	//Scaled HOMER
	if (ScaledHomerMode == 0 && glfwGetKey('3') == GLFW_PRESS) {
		ScaledHomerMode = 1;
	}
	if (ScaledHomerMode >= 1 && glfwGetKey('2') == GLFW_PRESS) {
		ScaledHomerMode = 0;
	}
	/*
	if (ScaledHomerEnable == 0 && glfwGetKey('3') == GLFW_PRESS) {
		ScaledHomerEnable = 1;
	}
	if (ScaledHomerEnable == 1 && glfwGetKey('2') == GLFW_PRESS) {
		ScaledHomerEnable = 0;
	}
	*/

	if (glfwGetKey(GLFW_KEY_DOWN)) {
		raymodelMatrix = glm::rotate(raymodelMatrix, FACTOR, vec3(-1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey(GLFW_KEY_UP)) {
		raymodelMatrix = glm::rotate(raymodelMatrix, FACTOR, vec3(1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey(GLFW_KEY_LEFT)) {
		raymodelMatrix = rotate(raymodelMatrix, FACTOR, vec3(0.0f, 1.0f, 0.0f));
	}
	if (glfwGetKey(GLFW_KEY_RIGHT)) {
		raymodelMatrix = rotate(raymodelMatrix, FACTOR, vec3(0.0f, -1.0f, 0.0f));
	}
	if (glfwGetKey('J') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, FACTOR*2, vec3(1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey('K') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, FACTOR*2, vec3(-1.0f, 0.0f, 0.0f));
	}
	if (glfwGetKey('H') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, FACTOR*2, vec3(0.0f, 1.0f, 0.0f));
	}
	if (glfwGetKey('L') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, FACTOR*2, vec3(0.0f, -1.0f, 0.0f));
	}
	if (glfwGetKey('N') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, FACTOR*2, vec3(0.0f, 0.0f, 1.0f));
	}
	if (glfwGetKey('M') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, FACTOR*2, vec3(0.0f, 0.0f, -1.0f));
	}
	if (glfwGetKey('O') == GLFW_PRESS) {
		modelMatrix = glm::rotate(modelMatrix, 0.5f, vec3(0.0f, 0.0f, 0.0f));
	}

	//ray translate
	if (glfwGetKey('A') == GLFW_PRESS) {
		raymodelMatrix = glm::translate(raymodelMatrix, vec3(-FACTOR*0.1,0,0));
	}
	if (glfwGetKey('D') == GLFW_PRESS) {
		raymodelMatrix = glm::translate(raymodelMatrix, vec3(FACTOR*0.1,0,0));
	}
	if (glfwGetKey('S') == GLFW_PRESS) {
		raymodelMatrix = glm::translate(raymodelMatrix, vec3(0,-FACTOR*0.1,0));
	}
	if (glfwGetKey('W') == GLFW_PRESS) {
		raymodelMatrix = glm::translate(raymodelMatrix, vec3(0,FACTOR*0.1,0));
	}
	if (glfwGetKey('X') == GLFW_PRESS) {
		raymodelMatrix = glm::translate(raymodelMatrix, vec3(0,0,-FACTOR*0.1));
	}
	if (glfwGetKey('Z') == GLFW_PRESS) {
		raymodelMatrix = glm::translate(raymodelMatrix, vec3(0,0,FACTOR*0.1));
	}
}


void Engine::checkPolhemus() {

	boost::mutex::scoped_lock lock(ioMutex);
	//SERIAL Queue
	while(!eventQueue.isSerialEmpty()) {
		boost::array<float,7> temp = eventQueue.serialPop();
		glm::vec3 position;
		glm::vec3 Rposition;
		glm::quat orientation;
		glm::quat Rorientation;
		glm::mat4 transform;
		glm::mat4 Rtransform;


		switch(appMode)  {
			case ScaledHOMER:
				//Scaled HOMER �g�p��
				position = glm::vec3(-temp[0]/100, -temp[1]/100, -temp[2]/100);
				position += vec3(0.0,0.5,1.9);
				break;
			default:
				//��Ď�@�g�p��
	//			position = glm::vec3(temp[0],temp[1],temp[2]+10);
				position = glm::vec3(-temp[0]/100, -temp[1]/100, -temp[2]/10);
				//position += vec3(0.0,-0.3,6);
				//position += vec3(0.0,0.5,1.9);
				position += vec3(0.0,0.5,6.0);
				//position = glm::vec3(temp[0]/10, temp[1]/10, temp[2]/10);
				//position += vec3(0.0,-0.3,1.8);
				break;
		}


		//std::cout << temp[0] << "\t" << temp[1] << "\t" << temp[2] << std::endl;
		
		orientation.w = temp[3];
		orientation.x = temp[4];
		orientation.y = temp[5];
		orientation.z = temp[6];

		glm::mat4 rot = glm::toMat4(orientation);
		glm::mat4 trans = glm::translate(glm::mat4(),position);	

		//transform = translate(mat4(),vec3(0,0,-4));
		transform = glm::toMat4(glm::angleAxis(90.0f,glm::vec3(0,-1,0)))*transform;
//		transform = glm::toMat4(glm::angleAxis(180.0f,glm::vec3(0,1,0)))*transform;
		transform = glm::toMat4(glm::angleAxis(90.0f,glm::vec3(0,0,1)))*transform;

//		raymodelMatrix = rot*transform; //��]�����ł���΂��̍s�̂�


		//Scaled HOMER mode �ł̏���
		if (ScaledHomerMode == 1) {
			orientation.w = -temp[3];
			initOrientation = glm::toMat4(orientation); //�I�����̃��V�[�o�̎p�����L��(���̎p������Ƃ���)
			//initPosition = inverse(trans); //�I�����̃��V�[�o�̈ʒu���L��(���̈ʒu����Ƃ���)

			//���i���s��łȂ��x�N�g���ŕ\�����ꍇ
			initPosition = position; //�I�����̃��V�[�o�̈ʒu���L��(���̈ʒu����Ƃ���)
			prevPosition = position;
			initModelMatrix = modelMatrix;
			prevModelMatrix = modelMatrix;
			ScaledHomerMode = 2;
		}
		if (ScaledHomerMode == 2 && raymodelMatrix != previousRaymodelMatrix) {
			//modelMatrix = rot*inverse(initOrientation)*transform; //��]�̓���(�����̎p�����)
			//modelMatrix = rot*initOrientation*transform; //��]�̓���(�����̎p�����)
			modelMatrix = rot*initOrientation*initModelMatrix;

			//��]�^�X�N�ł���Ε��i���s��Ȃ�
			if (taskMode == RotationTask || PracticeNum >= 3){
				modelMatrix[3][0] = prevModelMatrix[3][0];
				modelMatrix[3][1] = prevModelMatrix[3][1];
				modelMatrix[3][2] = prevModelMatrix[3][2];

			//��]�^�X�N�łȂ���Ε��i(Scaled HOMER)���s��
			}else{
				vec3 V_hand = (-position) - (-prevPosition); //���V�[�o�̑��x(1�t���[��������̃��V�[�o�̕ω���)
				//std::cout << V_hand[0] << "\t" << V_hand[1] << "\t" << V_hand[2] << std::endl;

				float SC = 0.007;
				float V_hand_d = 1.2 < sqrtf(pow(V_hand[0],2)+pow(V_hand[1],2)+pow(V_hand[2],2)) / SC ? 1.2 : sqrtf(pow(V_hand[0],2)+pow(V_hand[1],2)+pow(V_hand[2],2)) / SC; //���x�̃X�P�[�����O
				modelMatrix[3][0] = 50*V_hand_d*V_hand[0]+prevModelMatrix[3][0];
				modelMatrix[3][1] = 50*V_hand_d*V_hand[1]+prevModelMatrix[3][1];
				modelMatrix[3][2] = 50*V_hand_d*V_hand[2]+prevModelMatrix[3][2];

				prevPosition = position;
				prevModelMatrix = modelMatrix;
				LOG(sqrtf(pow(V_hand[0],2)+pow(V_hand[1],2)+pow(V_hand[2],2)));
			}

		}

		switch(appMode)  {
			case ScaledHOMER:
				transform = rot*transform;
				transform = inverse(trans)*transform;
				raymodelMatrix = transform;
				break;
			default:
				transform = rot*transform;
				transform = inverse(trans)*transform;
				raymodelMatrix = transform;
				break;
		}
		/*
		raymodelMatrix[3][0] = position[0];
		raymodelMatrix[3][1] = position[1];
		raymodelMatrix[3][2] = position[2];
		*/
	}
	lock.unlock();
}


void Engine::checkWiiMote() {
	//if the connection to the wii-mote was successful
	if (wii) {
		remote.RefreshState();

		switch(appMode)  {
		case rayCasting:
		case rayCastingOfSphere:
			if (appInputState == idle && !remote.Button.A() && remote.Button.B() && objectHit && !(taskMode == RotationTask || PracticeNum >= 3)) { 
				appInputState = trans;
				RotateTranslateMode = 0;
				translateMode = 1;
				rotateMode = 0;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				tflag = 0;
				std::cout << "translate mode" << '\n';
				//�f�o�C�X����̓��̓��O(push B)
				LOGSTRING("B");

				//�{�^���̃J�E���g���̃��O
				B_count = 1;
				LOGINT(B_count);
				B_count++;
				break;
			}
			if (appInputState == idle && remote.Button.A() && !remote.Button.B() && objectHit) {
				appInputState = rot;
				RotateTranslateMode = 0;
				translateMode = 0;
				rotateMode = 1;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				tflag = 0;
				std::cout << "rotate mode" << '\n';
				//�f�o�C�X����̓��̓��O(push A)
				LOGSTRING("A");

				A_count = 1;
				LOGINT(A_count);
				A_count++;
				break;
			}
			if ((appInputState == idle || appInputState == rot || appInputState == trans) && remote.Button.A() && remote.Button.B() && objectHit && !(taskMode == RotationTask || PracticeNum >= 3)) {
				//if (appInputState == rot) �f�o�C�X����̓��̓��O(pop A)
				//else if (appInputState == trans) �f�o�C�X����̓��̓��O(pop B)
				appInputState = rot_and_trans;
				RotateTranslateMode = 1;
				translateMode = 0;
				rotateMode = 0;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				tflag = 0;
				std::cout << "rotate and translate mode" << '\n';
				LOGSTRING("AB");

				AB_count = 1;
				LOGINT(AB_count);
				AB_count++;
				break;
			}

			/*
			//Scaled HOMER Enable
			if (appInputState == idle && remote.Button.One()&& objectHit) { 
				ScaledHomerEnable = 1;
				std::cout << "Scaled HOMER enable" << '\n';
				LOGSTRING("1");
				break;
			}
			*/

			/*
			//Scaled HOMER mode
			if (appInputState == idle && remote.Button.Two()&& objectHit) { 
				appInputState = scaled_homer;
				ScaledHomerMode = 1;
				std::cout << "Scaled HOMER mode" << '\n';
				//�f�o�C�X����̓��̓��O(push B)
				LOGSTRING("2");

				two_count = 1;
				LOGINT(two_count);
				two_count++;
				break;
			}
			*/

			if (appInputState == trans && remote.Button.Down() && rayDistance > 1 && !(taskMode == RotationTask || PracticeNum >= 3)) {
				grabbedDistance=0.1;
			}
			if (appInputState == trans && remote.Button.Up() && !(taskMode == RotationTask || PracticeNum >= 3)) {
				grabbedDistance=-0.1;
			}
			if (appInputState == trans && !remote.Button.B()) {
				appInputState = idle;
				translateMode = 0;

//				/*//���i�ł̓t���b�N���g�p���Ȃ�
				//�t���b�N����
				if (FlickEnable == 1){
					translateFlickMode = 1;
//					t.restart(); // �^�C�}�[�̃��Z�b�g
					tflag = 0;
					tcount = 0; //�J�E���g�̏�����
				}
//				*/

				std::cout << "idle" << '\n';
				//�f�o�C�X����̓��̓��O
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}
			if (appInputState == rot && !remote.Button.A()) {
				appInputState = idle;
				rotateMode = 0;

				//�t���b�N����
				if (FlickEnable == 1){
					rotateFlickMode = 1;
//					t.restart(); // �^�C�}�[�̃��Z�b�g
					tflag = 0;
					tcount = 0; //�J�E���g�̏�����
				}

				std::cout << "idle" << '\n';
				//�f�o�C�X����̓��̓��O
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}
			if (appInputState == rot_and_trans && (!remote.Button.A() || !remote.Button.B())) {
				appInputState = idle;
				RotateTranslateMode = 0;

				//�t���b�N����
				if (FlickEnable == 1){
					RotTransFlickMode = 1;
//					t.restart(); // �^�C�}�[�̃��Z�b�g
					tflag = 0;
					tcount = 0; //�J�E���g�̏�����
				}

				std::cout << "idle" << '\n';
				//�f�o�C�X����̓��̓��O
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}
			if (appInputState == scaled_homer && !remote.Button.Two()) {
				appInputState = idle;
				ScaledHomerMode = 0;
				std::cout << "idle" << '\n';
				//�f�o�C�X����̓��̓��O
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}


			//���O�p
			if (appInputState == trans && remote.Button.B()) {
				LOGSTRING("B");
				LOGINT(B_count);
				B_count++;
			}
			if (appInputState == rot && remote.Button.A()) {
				LOGSTRING("A");
				LOGINT(A_count);
				A_count++;
			}
			if (appInputState == rot_and_trans && remote.Button.A() && remote.Button.B()) {
				LOGSTRING("AB");
				LOGINT(AB_count);
				AB_count++;
			}
			if (appInputState == scaled_homer && remote.Button.Two()) {
				LOGSTRING("2");
				LOGINT(two_count);
				two_count++;
			}
			//if (appInputState == idle && (!remote.Button.A() && !remote.Button.B())) {
			if (appInputState == idle) {
				LOGSTRING("idle");
				LOGINT(idle_count);
				idle_count++;
			}
			break;



		case ScaledHOMER:
			//Scaled HOMER mode
			if (appInputState == idle && remote.Button.A() && !remote.Button.B() && objectHit) {
				appInputState = scaled_homer;
				ScaledHomerMode = 1;
				std::cout << "ScaledHOMER" << '\n';
				//�f�o�C�X����̓��̓��O(push B)
				LOGSTRING("A");

				A_count = 1;
				LOGINT(A_count);
				A_count++;
				break;
			}
			if (appInputState == scaled_homer && !remote.Button.A()) {
				appInputState = idle;
				ScaledHomerMode = 0;
				std::cout << "idle" << '\n';
				//�f�o�C�X����̓��̓��O
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}

			//���O�p
			if (appInputState == rot && remote.Button.A()) {
				LOGSTRING("A");
				LOGINT(A_count);
				A_count++;
			}
			if (appInputState == idle) {
				LOGSTRING("idle");
				LOGINT(idle_count);
				idle_count++;
			}
			break;

		default:
			//�f�o�C�X����̓��̓��O(��)
			break;
		}
	}
}


void Engine::checkSpaceNavigator() { 

#define SNSCALE 0.2f
#define RTSCALE 5.0f

	float position[6];
	std::fill_n(position,6,0.0f);



	if (t.elapsed() >= 1.0 && PedalPushed == 1) PedalPushed = 0; //�y�_�����������1.0�b�o�߂����Ƃ�

	if (glfwGetJoystickPos( GLFW_JOYSTICK_1, position,6) == 6 ) {

		//std::cout << position[2] << std::endl;

		//�^�X�N�I��/���̃^�[�Q�b�g�ɐi��
		if (LogStartFlg == 1 && targetFlg == 0 && position[2] == 1 && PedalPushed == 0) {
			targetFlg = 1;
			NextTaskFlg = 1;
			PedalPushed = 1;
		}else if (position[2] != 1) { //�L�[����w�����ꂽ��
			targetFlg = 0;
		}

	/*
	modelMatrix = glm::translate(vec3(position[0]*SNSCALE,0,0))*modelMatrix;
	modelMatrix = glm::translate(vec3(0,position[2]*SNSCALE,0))*modelMatrix;
	modelMatrix = glm::translate(vec3(0,0,position[1]*SNSCALE))*modelMatrix;
	modelMatrix = glm::rotate(RTSCALE*position[3],glm::vec3(0,1,0));
	modelMatrix = glm::rotate(RTSCALE*position[4],glm::vec3(0,0,1));
	modelMatrix = glm::rotate(RTSCALE*position[5],glm::vec3(1,0,0));
	*/
	}

	/*if (glfwGetJoystickPos( GLFW_JOYSTICK_1, position,6) == 6 ) {
	
	viewMatrix = glm::translate(vec3(position[0]*SNSCALE,0,0))*viewMatrix;
	viewMatrix = glm::translate(vec3(0,position[2]*SNSCALE,0))*viewMatrix;
	viewMatrix = glm::translate(vec3(0,0,position[1]*SNSCALE))*viewMatrix;
	viewMatrix = glm::rotate(RTSCALE*position[3],glm::vec3(0,1,0))*viewMatrix;
	viewMatrix = glm::rotate(RTSCALE*position[5],glm::vec3(1,0,0))*viewMatrix;
	viewMatrix = glm::rotate(RTSCALE*position[4],glm::vec3(0,0,1))*viewMatrix;
	}*/
}


void Engine::display() {

	#define RADIUS 3.5f

	glEnable(GL_DEPTH_TEST); //depthBuffer��L��
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	if (restoreRay) {
		glBindBuffer(GL_ARRAY_BUFFER,raypositionVBO);
		//float d = -8.0f;
		float d = -16.0f;
		//glBufferSubData(GL_ARRAY_BUFFER,2*sizeof(float),sizeof(d),&d);  //edit the 6th float, i.e. the Z
		for (int i = 5; i < 3*rayVerticesCount; i+=6) {
			glBufferSubData(GL_ARRAY_BUFFER,i*sizeof(float),sizeof(d),&d);  //edit every 6th float, i.e. the Z
		}
		restoreRay = false;  //we already restored, no need to do it every frame
	}

	glUseProgram(shaderProgram);  //select shader
	pvm = projectionMatrix*viewMatrix*modelMatrix; //calculate pvm
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm)); //upload it	
	cursor.draw();

	if (appMode == rayCastingOfSphere){
		//�I�u�W�F�N�g�̎����sphere��`��
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 1.0f);
		mat4 SphereModelMatrix;
		SphereModelMatrix = glm::translate(mat4(), vec3(modelMatrix[3]));

		pvm = projectionMatrix*viewMatrix*SphereModelMatrix; //calculate pvm
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.spheredraw();
	}


	//���ʂ̕`��
	glUseProgram(flatShader);
	glUniform4f(flatShaderColor, 0.8f, 0.8f ,0.8f, 1.0f);
	mat4 PlaneModelMatrix;
	PlaneModelMatrix = glm::translate(mat4(), vec3(0.1,-4.0,-15));

	pvm = projectionMatrix*viewMatrix*PlaneModelMatrix; //calculate pvm
	glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
	cursor.planedraw();

	glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 1.0f);
	cursor.planelinedraw();

	//�e�̕`��
	mat4 ShadowModelMatrix;
	ShadowModelMatrix = glm::translate(mat4(), vec3(modelMatrix[3][0],-3.9,modelMatrix[3][2]));
	float scale = 1 / (modelMatrix[3][1] + 4.9);
	if (scale > 1) scale = 1;
	if (scale < 0) scale = 0; //�I�u�W�F�N�g�����ʂƏd�Ȃ����Ƃ�
	pvm = projectionMatrix*viewMatrix*glm::scale(ShadowModelMatrix,glm::vec3(scale,scale,scale));
	glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
	glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 0.5f);
	cursor.shadowdraw();

	//target of experiment task
	//glUseProgram(targetShader);  //select shader
	glUseProgram(flatShader);
	glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 1.0f);
	mat4 TargetModelMatrix;
	vec3 targetvec;
	float targetrot; //�^�[�Q�b�g�̉�]��
	TargetModelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15)); //�^�[�Q�b�g��z�u���鋅�̒��S�ƂȂ�ʒu



	switch(taskMode){
	case DockingTask:

		//�^�X�N�̐؂�ւ�
		switch(TargetNum[targetNumber]){
		case 1:
			targetvec = vec3(1,0,0);
			targetrot = 20.0f;
			break;
		case 2:
			targetvec = vec3(-1,0,0);
			targetrot = 30.0f;
			break;
		case 3:
			targetvec = vec3(0,1,0);
			targetrot = 40.0f;
			break;
		case 4:
			targetvec = vec3(0,-1,0);
			targetrot = 50.0f;
			break;
		case 5:
			targetvec = normalize(vec3(1,1,1));
			targetrot = 60.0f;
			break;
		case 6:
			targetvec = normalize(vec3(-1,1,1));
			targetrot = 70.0f;
			break;
		case 7:
			targetvec = normalize(vec3(1,-1,1));
			targetrot = 80.0f;
			break;
		case 8:
			targetvec = normalize(vec3(1,1,-1));
			targetrot = 90.0f;
			break;
		case 9:
			targetvec = normalize(vec3(-1,-1,1));
			targetrot = 100.0f;
			break;
		case 10:
			targetvec = normalize(vec3(1,-1,-1));
			targetrot = 110.0f;
			break;
		case 11:
			targetvec = normalize(vec3(-1,1,-1));
			targetrot = 120.0f;
			break;
		case 12:
			targetvec = normalize(vec3(-1,-1,-1));
			targetrot = 130.0f;
			break;
		default:
			break;
		}
		TargetModelMatrix = glm::translate(TargetModelMatrix, RADIUS*targetvec);
		TargetModelMatrix = glm::rotate(TargetModelMatrix, targetrot, vec3(-1.0f, -1.0f, 0.0f));
		break;


	case RotationTask:

		TargetModelMatrix = glm::translate(mat4(), vec3(0,0,-5));
		TargetModelMatrix = glm::rotate(TargetModelMatrix, randRot, randAxis); //�����_���ȉ�]
		break;


	//���K�p�^�X�N�̏���(�^�X�N�̏��Ԃ͌Œ�)
	case PracticeTask:
		
		//�^�X�N�̐؂�ւ�
		switch(PracticeNum){
		case 1:
			targetvec = normalize(vec3(1.0f,1.0f,0.0f));
			targetrot = 40.0f;
			TargetModelMatrix = glm::translate(TargetModelMatrix, RADIUS*targetvec);
			TargetModelMatrix = glm::rotate(TargetModelMatrix, targetrot, vec3(-1.0f, -1.0f, 0.0f));
			break;
		case 2:
			targetvec = normalize(vec3(-1,-1,0));
			targetrot = 80.0f;
			TargetModelMatrix = glm::translate(TargetModelMatrix, RADIUS*targetvec);
			TargetModelMatrix = glm::rotate(TargetModelMatrix, targetrot, vec3(-1.0f, -1.0f, 0.0f));
			break;
		case 3:
			TargetModelMatrix = glm::translate(mat4(), vec3(0,0,-5));
			TargetModelMatrix = glm::rotate(TargetModelMatrix, 60.0f, vec3(-1.0f, -1.0f, -1.0f)); //�����_���ȉ�]
			break;
		case 4:
			TargetModelMatrix = glm::translate(mat4(), vec3(0,0,-5));
			TargetModelMatrix = glm::rotate(TargetModelMatrix, 90.0f, vec3(1.0f, 1.0f, 1.0f));
			break;
		}
		break;


	default:
		break;
	}


	pvm = projectionMatrix*viewMatrix*TargetModelMatrix; //calculate pvm
	//glUniformMatrix4fv(targetShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm)); //upload it
	glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
	glUniform4f(flatShaderColor, 0.5f, 0.5f ,0.5f, 1.0f);
////	cursor.wfdraw();

	/*
	//�^�[�Q�b�g�̎p�����ʗp��vertex��`��
	glUseProgram(shaderProgram);
	pvm = projectionMatrix*viewMatrix*TargetModelMatrix;
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm));
	cursor.vertexdraw();
	*/

	//�^�[�Q�b�g�̉e��`��
	glUseProgram(flatShader);
	ShadowModelMatrix = glm::translate(mat4(), vec3(TargetModelMatrix[3][0],-3.9,TargetModelMatrix[3][2]));
	scale = 1 / (TargetModelMatrix[3][1] + 4.9);
	if (scale > 1) scale = 1;
	if (scale < 0) scale = 0; //�I�u�W�F�N�g�����ʂƏd�Ȃ����Ƃ�
	pvm = projectionMatrix*viewMatrix*glm::scale(ShadowModelMatrix,glm::vec3(scale,scale,scale));
	glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
	glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 0.5f);
////	cursor.shadowdraw();


	/*
	float w = sqrtf(modelMatrix[0][0]+modelMatrix[1][1]+modelMatrix[2][2]+1.0f) * 0.5f;
	float teata = acos(w)*2;
	std::cout << teata << '\n';
	*/

	//�^�[�Q�b�g�Ƃ̊p�x�덷�̌v�Z
	mat4 rotMatrix = inverse(modelMatrix)*TargetModelMatrix;
	float w = sqrtf(rotMatrix[0][0]+rotMatrix[1][1]+rotMatrix[2][2]+1.0f) * 0.5f;
	float rot = degrees(acos(w)*2); //�^�[�Q�b�g�Ƃ̊p�x�덷

	//�^�[�Q�b�g�Ƃ̈ʒu�덷�̌v�Z
	float length = sqrtf( pow(TargetModelMatrix[3][0]-modelMatrix[3][0], 2) 
						+ pow(TargetModelMatrix[3][1]-modelMatrix[3][1], 2)
						+ pow(TargetModelMatrix[3][2]-modelMatrix[3][2], 2));

//	std::cout << rot << '\t' << length << '\n';
	LOG(rot);
	LOG(length);


	//�^�[�Q�b�g�ƃJ�[�\����vertex���Ƃ̋����̌v�Z
	vec4 vertVec1 = TargetModelMatrix*vec4(-1, -1, -1, 1.0f) - modelMatrix*vec4(-1, -1, -1, 1.0f);
	vec4 vertVec2 = TargetModelMatrix*vec4(1, -1, 1, 1.0f) - modelMatrix*vec4(1, -1, 1, 1.0f);
	vec4 vertVec3 = TargetModelMatrix*vec4(-1, 1, 1, 1.0f) - modelMatrix*vec4(-1, 1, 1, 1.0f);
	vec4 vertVec4 = TargetModelMatrix*vec4(1, 1, -1, 1.0f) - modelMatrix*vec4(1, 1, -1, 1.0f);

	float vertexD1 = sqrtf( pow(vertVec1[0], 2) + pow(vertVec1[1], 2) + pow(vertVec1[2], 2));
	float vertexD2 = sqrtf( pow(vertVec2[0], 2) + pow(vertVec2[1], 2) + pow(vertVec2[2], 2));
	float vertexD3 = sqrtf( pow(vertVec3[0], 2) + pow(vertVec3[1], 2) + pow(vertVec3[2], 2));
	float vertexD4 = sqrtf( pow(vertVec4[0], 2) + pow(vertVec4[1], 2) + pow(vertVec4[2], 2));

	//std::cout << vertexD1 << '\t' << vertexD2 << '\t' << vertexD3 << '\t' << vertexD4 << '\n';

	float vertLength;
	switch(taskMode){
	case DockingTask:
		vertLength = 1.0f;
		break;
	case RotationTask:
		vertLength = 0.5f;
		break;
	default:
		vertLength = 0.5f;
		break;
	}

	//vertex�̐F�̕ύX
	if (vertexD1 < vertLength) cursor.vertexColorChange(0);
	else cursor.vertexColorRes(0);
	if (vertexD2 < vertLength) cursor.vertexColorChange(1);
	else cursor.vertexColorRes(1);
	if (vertexD3 < vertLength) cursor.vertexColorChange(2);
	else cursor.vertexColorRes(2);
	if (vertexD4 < vertLength) cursor.vertexColorChange(3);
	else cursor.vertexColorRes(3);

	//���̃^�X�N�Ɉړ��ł��邱�Ƃ�������
	if (vertexD1 < vertLength && vertexD2 < vertLength && vertexD3 < vertLength && vertexD4 < vertLength && DockingCompFlg == 0 && LogStartFlg == 1){
		//std::cout << "OK" << '\n';
		DockingCompFlg = 1;
		mciSendString(TEXT("play myFile from 0"), NULL, 0, 0);
	}

	//�I�u�W�F�N�g(�J�[�\��)�̎p�����ʗp
	glUseProgram(shaderProgram);
	pvm = projectionMatrix*viewMatrix*modelMatrix;
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm));
////	cursor.vertexdraw();

	//�^�[�Q�b�g�̎p�����ʗp��vertex��`��
	glUseProgram(shaderProgram);
	pvm = projectionMatrix*viewMatrix*TargetModelMatrix;
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm));
////	cursor.vertexdraw();


	//�I�u�W�F�N�g�̕`��
	/*//����
	glUseProgram(targetShader);  //select shader
	pvm = projectionMatrix*viewMatrix*modelMatrix; //calculate pvm
	glUniformMatrix4fv(targetShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm)); //upload it	
	cursor.draw();
	*/


	if ((appMode == rayCastingOfSphere && cursor.findSphereIntersection(modelMatrix,raymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal)) ||
		(appMode == rayCasting         && cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint))										 ||
		(appMode == ScaledHOMER        && cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint) && ScaledHomerMode == 0)){

		/*
		//�I�u�W�F�N�g�̋����\���̕`��
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 1.0f ,0.0f, 0.2f); //��
		if (RotateTranslateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 0.0f ,0.0f, 0.2f); //��
		if (translateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 1.0f ,0.0f, 0.2f); //��
		if (rotateMode >= 1) glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.2f); //��
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.03f,1.03f,1.03f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.draw();

		//triangle�̕`��
		glUniform4f(flatShaderColor, 0.0f, 1.0f ,0.0f, 0.3f); //��
		if (RotateTranslateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 0.0f ,0.0f, 0.3f); //��
		if (translateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 1.0f ,0.0f, 0.3f); //��
		if (rotateMode >= 1) glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.3f); //��
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.04f,1.04f,1.04f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.Trdraw(); 
		*/
		
		//Ray length calculation
		rayLength = -glm::distance(vec3(raymodelMatrix[3]),objectIntersectionPoint);
		glBindBuffer(GL_ARRAY_BUFFER,raypositionVBO);
		for (int i = 5; i < 3*rayVerticesCount; i+=6) {
			glBufferSubData(GL_ARRAY_BUFFER,i*sizeof(float),sizeof(rayLength),&rayLength);  //edit every 6th float, i.e. the Z
		}
		restoreRay = true; //mark ray to be restored to full length


		//Intersection draw
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 1.0f); 
		vec4 Intersection = vec4(objectIntersectionPoint,1.0f);
		Intersection = inverse(modelMatrix)*Intersection;
		pvm = projectionMatrix*viewMatrix*modelMatrix;
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm)); //upload it
		cursor.Intersectiondraw(vec3(Intersection));

	/*
	//�I�u�W�F�N�g�͑I������Ă��Ȃ����ArotateMode�̂Ƃ�(rotateMode�ł�ray���I�u�W�F�N�g���痣��邱�Ƃ����肤��)
	}else if (rotateMode >= 1) {
		//rotation�p�̋����\��
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.1f); //��
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.1f,1.1f,1.1f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.draw();

		glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.2f); //��
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.11f,1.11f,1.11f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.Trdraw(); //triangle�̕`��
	*/
	}


	//ray�̕`��(Scaled HOMER mode �ł͕`�悵�Ȃ�)
	if (ScaledHomerMode == 0) {
		glUseProgram(shaderProgram);  //select shader
		raypvm = rayprojectionMatrix*rayviewMatrix*raymodelMatrix; //calculate pvm
		glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(raypvm)); //upload it
		glBindVertexArray(rayVAO); //select ray
		glDrawArrays(GL_TRIANGLE_STRIP,0,rayVerticesCount);
	}

	//�I�u�W�F�N�g�Aray�̃��O
	LOG(modelMatrix[0][0]);
	LOG(modelMatrix[0][1]);
	LOG(modelMatrix[0][2]);
	LOG(modelMatrix[0][3]);
	LOG(modelMatrix[1][0]);
	LOG(modelMatrix[1][1]);
	LOG(modelMatrix[1][2]);
	LOG(modelMatrix[1][3]);
	LOG(modelMatrix[2][0]);
	LOG(modelMatrix[2][1]);
	LOG(modelMatrix[2][2]);
	LOG(modelMatrix[2][3]);
	LOG(modelMatrix[3][0]);
	LOG(modelMatrix[3][1]);
	LOG(modelMatrix[3][2]);
	LOG(modelMatrix[3][3]);

	LOG(raymodelMatrix[0][0]);
	LOG(raymodelMatrix[0][1]);
	LOG(raymodelMatrix[0][2]);
	LOG(raymodelMatrix[0][3]);
	LOG(raymodelMatrix[1][0]);
	LOG(raymodelMatrix[1][1]);
	LOG(raymodelMatrix[1][2]);
	LOG(raymodelMatrix[1][3]);
	LOG(raymodelMatrix[2][0]);
	LOG(raymodelMatrix[2][1]);
	LOG(raymodelMatrix[2][2]);
	LOG(raymodelMatrix[2][3]);
	LOG(raymodelMatrix[3][0]);
	LOG(raymodelMatrix[3][1]);
	LOG(raymodelMatrix[3][2]);
	LOG(raymodelMatrix[3][3]);

	//�^�X�N�I�����̏���(�h�b�L���O�^�X�N)
	if (NextTaskFlg == 1){
	//if (rot < 10 && length < 0.5){

		switch(taskMode){
			case PracticeTask:
				std::cout << PracticeNum << '\n'; //�I�������^�X�N
				//������
				if (PracticeNum >= 2) modelMatrix = glm::translate(mat4(), vec3(0,0,-5));
				else modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
				appInputState = idle;
				RotateTranslateMode = 0;
				translateMode = 0;
				rotateMode = 0;
				ScaledHomerMode = 0;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				idle_count = 1;

				if (PracticeNum == 4){
					LogStartFlg = 0;
					mciSendString(TEXT("play myFile2 from 0"), NULL, 0, 0);
					std::cout << "practice task end" << '\n';

				}else PracticeNum++;

				NextTaskFlg = 0;
				DockingCompFlg = 0;
				restartFlg = 1;
				break;


			case DockingTask:	

				LOGLINE;
				LOGLINE;
				Logger::Fileclose();

				if (targetNumber == 1) Logger::resultInitialize("docking_task_result.txt"); //�^�X�N�̌��ʗp�t�@�C�����쐬
				else Logger::ResultFileopen("docking_task_result.txt");

				LOGINT(targetNumber);
				LOGINT(TargetNum[targetNumber]);
				//LOGSTRING("task time : ");
				LOG(t.elapsed());

				//LOGSTRING("vertex distance : ");
				LOG(vertexD1);
				LOG(vertexD2);
				LOG(vertexD3);
				LOG(vertexD4);

				//LOGSTRING("position error : ");
				LOG(length);

				//LOGSTRING("angle error : ");
				LOG(rot);

				LOGLINE;

				Logger::ResultFileclose();
				Logger::Fileopen("docking_task_log.txt");

				std::cout << targetNumber+1 << '\n';
				//������
				modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
				appInputState = idle;
				RotateTranslateMode = 0;
				translateMode = 0;
				rotateMode = 0;
				ScaledHomerMode = 0;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				idle_count = 1;

				//rotateNum = 0;

				//�^�X�N�ꏄ�̏I������
				//if (targetNumber == 12){
				if (targetNumber == 23){
					Logger::Fileclose();

					mciSendString(TEXT("play myFile2 from 0"), NULL, 0, 0);
					_mkdir("log");

					switch(appMode){
					case rayCasting:
						_mkdir("log\\rayCasting");
						CopyFile( _T("docking_task_log.txt"), _T("log\\rayCasting\\docking_task_log.txt"), FALSE ); //���O�t�@�C���̃R�s�[
						CopyFile( _T("docking_task_result.txt"), _T("log\\rayCasting\\docking_task_result.txt"), FALSE );
						break;
					case rayCastingOfSphere:
						_mkdir("log\\rayCastingOfSphere");
						CopyFile( _T("docking_task_log.txt"), _T("log\\rayCastingOfSphere\\docking_task_log.txt"), FALSE ); //���O�t�@�C���̃R�s�[
						CopyFile( _T("docking_task_result.txt"), _T("log\\rayCastingOfSphere\\docking_task_result.txt"), FALSE );
						break;
					case ScaledHOMER:
						_mkdir("log\\ScaledHOMER");
						CopyFile( _T("docking_task_log.txt"), _T("log\\ScaledHOMER\\docking_task_log.txt"), FALSE ); //���O�t�@�C���̃R�s�[
						CopyFile( _T("docking_task_result.txt"), _T("log\\ScaledHOMER\\docking_task_result.txt"), FALSE );
						break;
					}

					Logger::Initialize("dast_log.txt"); //�^�X�N�I����̃��O�͂�����Ɉڂ�
					LogStartFlg = 0;
					std::cout << "docking task end" << '\n';

				}else targetNumber++;
				NextTaskFlg = 0;
				DockingCompFlg = 0;
				//t.restart(); // �^�C�}�[�̃��Z�b�g
				restartFlg = 1;
				break;



			case RotationTask:
		
				LOGLINE;
				LOGLINE;
				Logger::Fileclose();

				if (rotateNum == 1) Logger::resultInitialize("rotation_task_result.txt"); //�^�X�N�̌��ʗp�t�@�C�����쐬
				else Logger::ResultFileopen("rotation_task_result.txt");

				LOGINT(rotateNum);
				//LOGSTRING("task time : ");
				LOG(t.elapsed());

				//LOGSTRING("vertex distance : ");
				LOG(vertexD1);
				LOG(vertexD2);
				LOG(vertexD3);
				LOG(vertexD4);

				//LOGSTRING("angle error : ");
				LOG(rot);

				//LOGSTRING("target angle : ");
				LOG(randRot);
				LOG(randAxis[0]);
				LOG(randAxis[1]);
				LOG(randAxis[2]);

				LOGLINE;

				Logger::ResultFileclose();
				Logger::Fileopen("rotation_task_log.txt");

				std::cout << rotateNum << '\n';
				//������
				//modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
				appInputState = idle;
				RotateTranslateMode = 0;
				translateMode = 0;
				rotateMode = 0;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				idle_count = 1;

				//�^�X�N�ꏄ�̏I������
				if (rotateNum == 20){
					Logger::Fileclose();

					mciSendString(TEXT("play myFile2 from 0"), NULL, 0, 0);
					_mkdir("log");

					switch(appMode){
					case rayCasting:
						_mkdir("log\\rayCasting");
						CopyFile( _T("rotation_task_log.txt"), _T("log\\rayCasting\\rotation_task_log.txt"), FALSE ); //���O�t�@�C���̃R�s�[
						CopyFile( _T("rotation_task_result.txt"), _T("log\\rayCasting\\rotation_task_result.txt"), FALSE );
						break;
					case rayCastingOfSphere:
						_mkdir("log\\rayCastingOfSphere");
						CopyFile( _T("rotation_task_log.txt"), _T("log\\rayCastingOfSphere\\rotation_task_log.txt"), FALSE ); //���O�t�@�C���̃R�s�[
						CopyFile( _T("rotation_task_result.txt"), _T("log\\rayCastingOfSphere\\rotation_task_result.txt"), FALSE );
						break;
					case ScaledHOMER:
						_mkdir("log\\ScaledHOMER");
						CopyFile( _T("rotation_task_log.txt"), _T("log\\ScaledHOMER\\rotation_task_log.txt"), FALSE ); //���O�t�@�C���̃R�s�[
						CopyFile( _T("rotation_task_result.txt"), _T("log\\ScaledHOMER\\rotation_task_result.txt"), FALSE );
						break;
					}

					Logger::Initialize("dast_log.txt"); //�^�X�N�I����̃��O�͂�����Ɉڂ�
					LogStartFlg = 0;
					std::cout << "rotation task end" << '\n';

				}else rotateNum++;

				//�����_���ȉ�]��^����
				randRot = rand()%180;
				randAxis[0] = rand()%101-50;
				randAxis[1] = rand()%101-50;
				randAxis[2] = rand()%101-50;
				randAxis = normalize(randAxis);

				//�����s��ȏꍇ�̏���
				while (randAxis[0] == 0 && randAxis[1] == 0 && randAxis[2] == 0) { 
					randAxis[0] = rand()%101-50;
					randAxis[1] = rand()%101-50;
					randAxis[2] = rand()%101-50;
					randAxis = normalize(randAxis);
				}

				NextTaskFlg = 0;
				DockingCompFlg = 0;
				//t.restart(); // �^�C�}�[�̃��Z�b�g
				restartFlg = 1;
				break;


			case NonTask:
				break;
		}

	}else LOGLINE;

	glfwSwapBuffers(); //swap buffers for display
}


void Engine::taskshuffle(){

	#define TARGETNUMBER 12
	int i,a,b;
	srand((unsigned)time(NULL)); //�����̏�����
	for(i=0; i<TARGETNUMBER; i++) TargetNum[i] = i+1;
	
	for(i=0; i<TARGETNUMBER; i++){
		a = rand()%TARGETNUMBER;
		b = rand()%TARGETNUMBER;
		swap(TargetNum[a],TargetNum[b]);
	}

	//�h�b�L���O�^�X�N�̓񏇖ڂ̍쐬
	int tmpNum[TARGETNUMBER];
	for(i=0; i<TARGETNUMBER; i++) tmpNum[i] = i+1;
	
	for(i=0; i<TARGETNUMBER; i++){
		a = rand()%TARGETNUMBER;
		b = rand()%TARGETNUMBER;
		swap(tmpNum[a],tmpNum[b]);
	}
	
	for(i=0; i<TARGETNUMBER; i++) TargetNum[i+TARGETNUMBER] = tmpNum[i];

	/*
	//�f�o�b�O�p(1�`12�̐����d�������\������Ă��邩�ǂ���)
	for(i=0; i<TARGETNUMBER*2; i++){
		std::cout << i << '\t' << TargetNum[i] << '\n';
	}
	*/
}


void Engine::modelRotationinit(float x, float y, float z) {
	arcBallPreviousPoint[0] = x*1.0f;
	arcBallPreviousPoint[1] = y*1.0f;
	arcBallPreviousPoint[2] = z*1.0f;
	//tempOrigin = glm::vec3(modelMatrix[3][0],modelMatrix[3][1],modelMatrix[3][2]);
}


void Engine::modelTranslateRotation(float xpos, float ypos, float zpos) {
	vec3 move,rayDirection;
	glm::vec4 rot,axis,transIntersection,first,end;
	rot = pho::util::getRotation(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],xpos*1.0f,ypos*1.0f,zpos*1.0f,modelMatrix[3][0],modelMatrix[3][1],modelMatrix[3][2],false);

	if (!(rot[0] == 0 && rot[1] == 0 && rot[2] == 0)) { //�����s��łȂ��Ƃ�
		end = modelMatrix*vec4(xpos,ypos,zpos,1.0f);
		modelMatrix = glm::rotate(modelMatrix, rot[3], vec3(rot[0], rot[1], rot[2]));
		first = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		move = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);

		modelMatrix[3][0] = modelMatrix[3][0] + move[0];
		modelMatrix[3][1] = modelMatrix[3][1] + move[1];
		modelMatrix[3][2] = modelMatrix[3][2] + move[2];
		/*
		arcBallPreviousPoint[0] = x*1.0f;
		arcBallPreviousPoint[1] = y*1.0f;
		arcBallPreviousPoint[2] = z*1.0f;
		*/
		/*
		//�t���b�N����(����)
		if (FlickEnable == 1){
			if (t.elapsed() > FLICKJUDGETIME){
				endRayPosition = end;
				float rayMoveDistance = sqrt(pow(endRayPosition[0]-firstRayPosition[0],2)+pow(endRayPosition[1]-firstRayPosition[1],2)+pow(endRayPosition[2]-firstRayPosition[2],2));

				if (rayMoveDistance >= FLICKJUDGEDISTANCE){
					flickRot = rot;
					flickMove = move;
					RotateTranslateMode = 0; //�I�u�W�F�N�g����_���痣���
					RotTransFlickMode = 1;
					tcount = 0; //�J�E���g�̏�����
				}

				std::cout << rayMoveDistance << "\t" << t.elapsed() << " sec" << std::endl;
				t.restart(); // �^�C�}�[�̃��Z�b�g
				tflag = 0;
				tcount = 0;
			} else {
				tcount++;
			}
		}
		*/
		flickRot = rot;
		flickMove = move;


	}
}


void Engine::modelRotation(float xpos, float ypos, float zpos) {
	vec4 rot;
	rot = pho::util::getRotation(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],xpos*1.0f,ypos*1.0f,zpos*1.0f,tempOrigin[0],tempOrigin[1],tempOrigin[2],false);
	if (!(rot[0] == 0 && rot[1] == 0 && rot[2] == 0)) { //�����s��łȂ��Ƃ�
		modelMatrix = glm::rotate(modelMatrix, rot[3], vec3(rot[0], rot[1], rot[2]));

		/*
		vec4 end = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		//�t���b�N����(����)
		if (FlickEnable == 1){
			if (t.elapsed() > FLICKJUDGETIME){
				endRayPosition = end;
				float rayMoveDistance = sqrt(pow(endRayPosition[0]-firstRayPosition[0],2)+pow(endRayPosition[1]-firstRayPosition[1],2)+pow(endRayPosition[2]-firstRayPosition[2],2));

				if (rayMoveDistance >= FLICKJUDGEDISTANCE){
					flickRot = rot;
					rotateMode = 0; //�I�u�W�F�N�g����_���痣���
					rotateFlickMode = 1;
					tcount = 0; //�J�E���g�̏�����
				}
				t.restart(); // �^�C�}�[�̃��Z�b�g
				tflag = 0;
			}
		}
		*/
		flickRot = rot;

		//�I�u�W�F�N�g�̖ʂ�ray�͘A��������ׂ��Ȃ̂ŁA���̏����͕K�v�Ȃ�
		//arcBallPreviousPoint[0] = xpos;
		//arcBallPreviousPoint[1] = ypos;
		//arcBallPreviousPoint[2] = zpos;
	}
	limitRot = rot; //��]�̌��E���Ɏg�p
}


///////////////////////////////////////////////
// ��]�s�񁨃N�H�[�^�j�I���ϊ�
//
// qx, qy, qz, qw : �N�H�[�^�j�I�������i�o�́j
// m11-m33 : ��]�s�񐬕�

bool Engine::transformRotMatToQuaternion(float &qx, float &qy, float &qz, float &qw, mat4 matrix){

	float m11 = matrix[0][0];
	float m12 = matrix[0][1];
	float m13 = matrix[0][2];
	float m21 = matrix[1][0];
	float m22 = matrix[1][1];
	float m23 = matrix[1][2];
	float m31 = matrix[2][0];
	float m32 = matrix[2][1];
	float m33 = matrix[2][2];

    // �ő听��������
    float elem[ 4 ]; // 0:x, 1:y, 2:z, 3:w
    elem[ 0 ] = m11 - m22 - m33 + 1.0f;
    elem[ 1 ] = -m11 + m22 - m33 + 1.0f;
    elem[ 2 ] = -m11 - m22 + m33 + 1.0f;
    elem[ 3 ] = m11 + m22 + m33 + 1.0f;

    unsigned biggestIndex = 0;
    for ( int i = 1; i < 4; i++ ) {
        if ( elem[i] > elem[biggestIndex] )
            biggestIndex = i;
    }

    if ( elem[biggestIndex] < 0.0f )
        return false; // �����̍s��ɊԈႢ����I

    // �ő�v�f�̒l���Z�o
    float *q[4] = {&qx, &qy, &qz, &qw};
    float v = sqrtf( elem[biggestIndex] ) * 0.5f;
    *q[biggestIndex] = v;
    float mult = 0.25f / v;

    switch ( biggestIndex ) {
    case 0: // x
        *q[1] = (m12 + m21) * mult;
        *q[2] = (m31 + m13) * mult;
        *q[3] = (m23 - m32) * mult;
        break;
    case 1: // y
        *q[0] = (m12 + m21) * mult;
        *q[2] = (m23 + m32) * mult;
        *q[3] = (m31 - m13) * mult;
        break;
    case 2: // z
        *q[0] = (m31 + m13) * mult;
        *q[1] = (m23 + m32) * mult;
        *q[3] = (m12 - m21) * mult;
    break;
    case 3: // w
        *q[0] = (m23 - m32) * mult;
        *q[1] = (m31 - m13) * mult;
        *q[2] = (m12 - m21) * mult;
        break;
    }

    return true;
}


void Engine::go() {
	Logger::Initialize("practice_log.txt"); //���O�t�@�C���̍쐬
	modelinit();
	rayinit();
	framebufferinit();
	targetinit();
	taskshuffle(); //�^�X�N�̏��Ԃ������_���ɂ���
	t.restart(); // �^�C�}�[�̏�����
	//rendering loop
	while(true) {
		//LOG(t.elapsed()); //�����̋L�^
		checkEvents();
		display();
		if(glfwGetKey(GLFW_KEY_ESC)) {
			shutdown();
			break;
		}
	}
}


void Engine::shutdown() {

	_serialserver.shutDown();
	serialioservice.stop();
	serialThread->join();
	mciSendString(TEXT("close myFile"), NULL, 0, 0);
	mciSendString(TEXT("close myFile2"), NULL, 0, 0);

	//ioservice.stop();

	//errorLog.close();
}


int main()
{
	//init GLFW
    if (!glfwInit()==GL_TRUE) {  std::cout << "glfw initialization failed";  return 1;  }

	//attributes for the context
    glfwOpenWindowHint(GLFW_VERSION_MAJOR,3);
    glfwOpenWindowHint(GLFW_VERSION_MINOR,3);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES,8);
	
	//create the context
	if (!glfwOpenWindow(WINDOW_SIZE_X,WINDOW_SIZE_Y,0,0,0,0,0,0,GLFW_WINDOW))
//	if (!glfwOpenWindow(WINDOW_SIZE_X,WINDOW_SIZE_Y,0,0,0,0,0,0,GLFW_FULLSCREEN))
    { 
		std::cout << "GLFW Init WIndow Failed, OpenGL 3.3 not supported?" << std::endl;
    }
	//glfwSetWindowPos(0,350); //window Position
	//glfwSetWindowPos(1910,-30); //recentry window Position
	glfwSetWindowPos(1275,-30); //now window Position
	//glfwSetWindowPos(0,0); //window Position
    glfwSetWindowTitle("Mesh-Grab");

	mciSendString(TEXT("open sound1.wav type waveaudio alias myFile"), NULL, 0, 0); 
	mciSendString(TEXT("open sound2.wav type waveaudio alias myFile2"), NULL, 0, 0); 

	glewInit();
	pho::Engine *engine = new Engine();
	engine->go();
	glfwTerminate();
	return 0;
}