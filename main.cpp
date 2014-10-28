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

boost::timer t; // タイマーの開始

glm::mat4 modelMatrix;  //to move our object around
glm::mat4 viewMatrix;   //to move our camera around
glm::mat4 projectionMatrix; //for perspective projection
glm::mat4 pvm;
glm::mat4 raymodelMatrix;  //to move our object around
glm::mat4 rayviewMatrix;   //to move our camera around
glm::mat4 rayprojectionMatrix; //for perspective projection
glm::mat4 raypvm;

glm::mat4 IntersectionModelMatrix;

glm::mat4 tmpraymodelMatrix; //raymodelMatrixを擬似的にrotateしたもの(findIntersectionで使用)
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

glm::mat4 initOrientation; //選択時のレシーバの姿勢(Scaled HOMER mode)
//glm::mat4 initPosition; //選択時のレシーバの位置(Scaled HOMER mode)
glm::vec3 initPosition; //選択時のレシーバの位置(Scaled HOMER mode)
glm::vec3 prevPosition;
glm::mat4 prevModelMatrix;
glm::mat4 initModelMatrix; //初期のmodelMatrix(Scaled HOMER mode)

vec4 firstRayPosition; //移動前のrayの先端の座標
vec4 endRayPosition; //移動後のrayの先端の座標

vec4 flickRot;
vec3 flickMove;
vec4 limitRot; //回転の限界時にフリックを行う際に使用

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

//Scaled HOMERを実行可能な状態にするフラグ
//int ScaledHomerEnable = 0; 
//int ScaledHomerEnable = 1;

int flickStop = 0; //フリックを強制終了する際に使用
int RotateLimitFlag = 0; //回転の限界時に使用するフラグ
int NextTaskFlg = 0; //次のタスクに移動する際のフラグ

int targetFlg = 0;
int targetNumber = 1;
int LogStartFlg = 0; //logの記録を開始する際のフラグ

//ボタンの押されているフレームのカウント
int A_count = 0;
int B_count = 0;
int AB_count = 0;
int idle_count = 0;
int two_count = 0;

float rayDistance;
float grabbedDistance = 0;

float randRot = 0;
vec3 randAxis = vec3(1,1,1); //初期値を与えておく(0だとオブジェクトが消える)
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
	modelMatrix = glm::translate(modelMatrix, vec3(1.3,1.3,-15)); //objectの位置
//	modelMatrix = glm::rotate(modelMatrix, 50.0f, vec3(-1.0f, -1.0f, 0.0f)); //適当な回転(初期状態)

	//initModelMatrix = modelMatrix; //scaled HOMER用

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
	//原点を中心に回転
	vertices.push_back(vec3(0, 0, -4)); //移動する点
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
		//タスクの初めのとき(0秒から開始するため)
		if (restartFlg == 1){
			restartFlg = 0;
			t.restart();
		}
		LOG(t.elapsed()); //ログの記録が開始されているときに時刻の記録を行う(2行目から)
	}


	previousModelMatrix = modelMatrix;
	previousRaymodelMatrix = raymodelMatrix;

	//オブジェクトが選択されているかどうかの判定
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
			if (RotateTranslateMode >= 1) cursor.findIntersection(RotateTranslateMode,modelMatrix,previousRaymodelMatrix,objectIntersectionPoint); //ray移動前(こちらの方が正確？)
			if (translateMode >= 1) cursor.findIntersection(translateMode,modelMatrix,previousRaymodelMatrix,objectIntersectionPoint);
			if (rotateMode >= 1) cursor.findIntersection(rotateMode,modelMatrix,previousRaymodelMatrix,objectIntersectionPoint);
			break;
		case rayCastingOfSphere:
			if (RotateTranslateMode >= 1) cursor.findSphereIntersection(modelMatrix,previousRaymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal); //ray移動前(こちらの方が正確？)
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

	//--回転＋並進の処理--//
	if (RotateTranslateMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray移動前(こちらの方が正確？)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		//modelRotationinit(modelIntersection[0]-modelMatrix[3][0], modelIntersection[1]-modelMatrix[3][1], modelIntersection[2]-modelMatrix[3][2]);
		RotateTranslateMode = 2;
	}else if (RotateTranslateMode == 2 && raymodelMatrix != previousRaymodelMatrix){ //raymodelMatrixが変化しているとき

		//交点を変換して用いる場合
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);  //direction of ray
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f); //rayから見た交点の座標
		modelIntersection = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);

		//フリック処理(初期処理)
		if (FlickEnable == 1){ 
			if (tflag == 0){ //計測前にrayの先端の初期座標(移動前)を記憶(このフェーズでは移動後のray)
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
				//オブジェクトとrayの角度が限界であるかどうか
				if (cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint)){
				}else{
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //rayとオブジェクトを同時に動かすとき
					//raymodelMatrix = previousRaymodelMatrix; //rayとオブジェクトを固定するとき
				}
				break;
			case rayCastingOfSphere:
				//sphereとrayの角度が限界であるかどうか
				float vangle = glm::angle(sphereIntersectionNormal,RayDir);
				//std::cout << "vangle\t" << vangle << std::endl;
				if (vangle >= 85 && vangle <= 95){
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //rayとオブジェクトを同時に動かすとき
				}
				break;
		}


	//フリック処理(オブジェクトのフリックによる移動)
	} else if (FlickEnable == 1 && RotTransFlickMode == 1){
		flickRot[3] = flickRot[3]*FLICKDECAY;
		flickMove = vec3(flickMove[0]*FLICKDECAY,flickMove[1]*FLICKDECAY,flickMove[2]*FLICKDECAY);
		modelMatrix = glm::rotate(modelMatrix, flickRot[3], vec3(flickRot[0], flickRot[1], flickRot[2]));
		modelMatrix[3][0] = modelMatrix[3][0] + flickMove[0];
		modelMatrix[3][1] = modelMatrix[3][1] + flickMove[1];
		modelMatrix[3][2] = modelMatrix[3][2] + flickMove[2];
		tcount++;
//		if (tcount == FLICKCOUNT){
		if (tcount == FLICKCOUNT || flickStop == 1){ //ボタン操作によってフリックを停止
			std::cout << "flick end"  << std::endl;
			RotTransFlickMode = 0;
			tcount = 0;
			flickStop = 0; //フリックの停止フラグの初期化
		}
	}



	
	//--並進のみの処理--//
	if (translateMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		//rayDistance = sqrt(pow(modelIntersection[0]-raymodelMatrix[3][0],2)+pow(modelIntersection[1]-raymodelMatrix[3][1],2)+pow(modelIntersection[2]-raymodelMatrix[3][2],2));
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray移動前(こちらの方が正確？)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		translateMode = 2;
	} else if (translateMode == 2 && raymodelMatrix != previousRaymodelMatrix){

		vec4 first = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f);
		vec4 end = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);

		//フリック処理(初期処理)
		if (FlickEnable == 1){ //計測前にrayの先端の初期座標(移動前)を記憶(このフェーズでも移動前のray)
			if (tflag == 0){
//				t.restart(); //ログでタイマーを使用しているため
				firstRayPosition = first;
				tflag = 1;
			}
		}

		//オブジェクトの並進
		modelMatrix[3][0] = modelMatrix[3][0] + end[0] - first[0];
		modelMatrix[3][1] = modelMatrix[3][1] + end[1] - first[1];
		modelMatrix[3][2] = modelMatrix[3][2] + end[2] - first[2];

		/*
		//フリック処理(判定)
		if (FlickEnable == 1){
			if (t.elapsed() > FLICKJUDGETIME){
				endRayPosition = end;
				float rayMoveDistance = sqrt(pow(endRayPosition[0]-firstRayPosition[0],2)+pow(endRayPosition[1]-firstRayPosition[1],2)+pow(endRayPosition[2]-firstRayPosition[2],2));

				if (rayMoveDistance >= FLICKJUDGEDISTANCE){
					flickMove = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);
					translateMode = 0; //オブジェクトが交点から離れる
					translateFlickMode = 1;
					tcount = 0; //カウントの初期化
				}
				t.restart(); // タイマーのリセット
				tflag = 0;
			}
		}
		*/
		flickMove = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);

		/*
		//rayの長さの調節
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
				//オブジェクトとrayの角度が限界であるかどうか
				if (cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint)){
				}else{
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //rayとオブジェクトを同時に動かすとき
					//raymodelMatrix = previousRaymodelMatrix; //rayとオブジェクトを固定するとき
				}
				break;
			case rayCastingOfSphere:
				//sphereとrayの角度が限界であるかどうか
				float vangle = glm::angle(sphereIntersectionNormal,RayDir);
				std::cout << "vangle\t" << vangle << std::endl;
				if (vangle >= 85 && vangle <= 95){
					modelMatrix = previousModelMatrix;
					modelMatrix = raymodelMatrix*inverse(previousRaymodelMatrix)*modelMatrix; //rayとオブジェクトを同時に動かすとき
				}
				break;
		}


//	/*
	//フリック処理(オブジェクトのフリックによる移動)
	} else if (FlickEnable == 1 && translateFlickMode == 1){
		flickMove = vec3(flickMove[0]*FLICKDECAY,flickMove[1]*FLICKDECAY,flickMove[2]*FLICKDECAY);
		modelMatrix[3][0] = modelMatrix[3][0] + flickMove[0];
		modelMatrix[3][1] = modelMatrix[3][1] + flickMove[1];
		modelMatrix[3][2] = modelMatrix[3][2] + flickMove[2];
		tcount++;
//		if (tcount == FLICKCOUNT){
		if (tcount == FLICKCOUNT || flickStop == 1){ //ボタン操作によってフリックを停止
			std::cout << "translate flick end"  << std::endl;
			translateFlickMode = 0;
			tcount = 0;
			flickStop = 0;
		}
//	*/
	}



	
	//--回転のみの処理--//
	if (rotateMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		//rayDistance = sqrt(pow(modelIntersection[0]-raymodelMatrix[3][0],2)+pow(modelIntersection[1]-raymodelMatrix[3][1],2)+pow(modelIntersection[2]-raymodelMatrix[3][2],2));
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray移動前(こちらの方が正確？)
		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotationinit(modelIntersection[0], modelIntersection[1], modelIntersection[2]);
		rotateMode = 2;
	} else if (rotateMode == 2 && raymodelMatrix != previousRaymodelMatrix){
//		modelIntersection = vec4(objectIntersectionPoint,1.0f);
//		modelIntersection = inverse(modelMatrix)*modelIntersection;
		

		//交点を変換して用いる場合
		vec3 RayDir = glm::mat3(raymodelMatrix)*glm::vec3(0,0,-1);  //direction of ray
		RayDir = glm::normalize(RayDir);
		modelIntersection = vec4(rayDistance*RayDir,1.0f); //rayから見た交点の座標
		modelIntersection = vec4(modelIntersection[0] + raymodelMatrix[3][0], modelIntersection[1] + raymodelMatrix[3][1], modelIntersection[2] + raymodelMatrix[3][2], 1.0f);


		vec4 first = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		//フリック処理(初期処理)
		if (FlickEnable == 1){ //計測前にrayの先端の初期座標(移動前)を記憶(このフェーズでも移動前のray)
			if (tflag == 0){
//				t.restart();
				firstRayPosition = first;
				tflag = 1;
			}
		}

		modelIntersection = inverse(modelMatrix)*modelIntersection;
		modelRotation(modelIntersection[0], modelIntersection[1], modelIntersection[2]);

		/*
		//オブジェクトとrayの角度が限界であるかどうか
		if (!(cursor.findIntersection(rotateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint))){
			rotateMode = 0; //rotateModeの終了

			//限界時のフリック処理
			if (!(limitRot[0] == 0 && limitRot[1] == 0 && limitRot[2] == 0)) { //軸が不定でないとき(選択された時点で回転が不可能でないとき)
				flickRot = limitRot;
				flickRot[3] = flickRot[3]/4;
				tcount = 0; //カウントの初期化
				RotateLimitFlag = 1;
				//FlickEnable = 1; rotateFlickMode = 1; //一時的にフリックを許可する
			} else std::cout << "rot impossible"  << std::endl;
		}
		*/

	//フリック処理(オブジェクトのフリックによる移動)
	} else if (FlickEnable == 1 && rotateFlickMode == 1){
		flickRot[3] = flickRot[3]*FLICKDECAY;
		modelMatrix = glm::rotate(modelMatrix, flickRot[3], vec3(flickRot[0], flickRot[1], flickRot[2]));
		tcount++;
//		if (tcount == FLICKCOUNT){
		if (tcount == FLICKCOUNT || flickStop == 1){ //ボタン操作によってフリックを停止
			std::cout << "rotate flick end"  << std::endl;
			rotateFlickMode = 0;
			tcount = 0;
			flickStop = 0;
		}

	//限界時のフリック処理
	} else if (RotateLimitFlag == 1){
		flickRot[3] = flickRot[3]*FLICKDECAY;
		modelMatrix = glm::rotate(modelMatrix, flickRot[3], vec3(flickRot[0], flickRot[1], flickRot[2]));
		tcount++;
		std::cout << tcount << std::endl;
		if (tcount == FLICKCOUNT || flickStop == 1){ //ボタン操作によってフリックを停止
			tcount = 0;
			flickStop = 0;
			RotateLimitFlag = 0;
		}
	}


	//rayの長さの調節(並進)
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
	//オブジェクトのZ軸回転
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


	//--Scaled HOMERの処理--//
	if (ScaledHomerMode == 1){
		modelIntersection = vec4(objectIntersectionPoint,1.0f);
		rayDistance = sqrt(pow(modelIntersection[0]-previousRaymodelMatrix[3][0],2)+pow(modelIntersection[1]-previousRaymodelMatrix[3][1],2)+pow(modelIntersection[2]-previousRaymodelMatrix[3][2],2)); //ray移動前(こちらの方が正確？)
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

		//オブジェクトの並進
		modelMatrix[3][0] = modelMatrix[3][0] + end[0] - first[0];
		modelMatrix[3][1] = modelMatrix[3][1] + end[1] - first[1];
		modelMatrix[3][2] = modelMatrix[3][2] + end[2] - first[2];

		flickMove = vec3(end[0]-first[0],end[1]-first[1],end[2]-first[2]);




		//オブジェクトとrayの角度が限界であるかどうか
		if (!(cursor.findIntersection(translateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint))){
			translateMode = 0; //translateModeの終了
		}
		*/
	}




	//rayが止まったとき(keyborad用)
	if (raymodelMatrix == previousRaymodelMatrix){
		tflag = 0;
	}
}


void Engine::checkKeyboard() {

	//ray,modelなどの初期化
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
		
		raymodelMatrix = translate(mat4(),vec3(0,0,-4)); //keyboard用
		translateMode = 0;
		//FlickEnable = 0;
		RotateTranslateMode = 0;
		RotTransFlickMode = 0;
	}

	//logの記録の開始(練習用タスク)
	if (LogStartFlg == 0 && glfwGetKey('9') == GLFW_PRESS) {
		LogStartFlg = 1;
		taskMode = PracticeTask;
		//taskshuffle(); //タスクのシャッフル

		//初期化
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
		//Logger::Initialize("dast_log.txt"); //ログファイルの作成
		std::cout << "practice task start" << '\n';

		t.restart();
		LOG(t.elapsed()); //時刻の記録(1行目)
	}

	//logの記録の開始(ドッキングタスク)
	if (LogStartFlg == 0 && glfwGetKey('7') == GLFW_PRESS) {
		LogStartFlg = 1;
		taskMode = DockingTask;
		taskshuffle(); //タスクのシャッフル

		//初期化
		modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
		appInputState = idle;
		RotateTranslateMode = 0;
		translateMode = 0;
		rotateMode = 0;
		RotTransFlickMode = 0;
		translateFlickMode = 0;
		rotateFlickMode = 0;
		idle_count = 1;

		targetNumber = 0; ////次のtargetNumber++ が実行されるため
		rotateNum = 0;
		PracticeNum = 0;

		Logger::Fileclose();
		Logger::Initialize("docking_task_log.txt"); //ログファイルの作成
		std::cout << "docking task start" << '\n';

		t.restart();
		LOG(t.elapsed()); //時刻の記録(1行目)
	}

	//logの記録の開始(回転タスク)
	if (LogStartFlg == 0 && glfwGetKey('8') == GLFW_PRESS) {
		LogStartFlg = 1;
		taskMode = RotationTask;

		//初期化
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

		//ランダムな回転を与える
		randRot = rand()%180;
		randAxis[0] = rand()%101-50;
		randAxis[1] = rand()%101-50;
		randAxis[2] = rand()%101-50;
		randAxis = normalize(randAxis);

		//軸が不定な場合の処理
		while (randAxis[0] == 0 && randAxis[1] == 0 && randAxis[2] == 0) { 
			randAxis[0] = rand()%101-50;
			randAxis[1] = rand()%101-50;
			randAxis[2] = rand()%101-50;
			randAxis = normalize(randAxis);
		}

		Logger::Initialize("rotation_task_log.txt"); //ログファイルの作成
		std::cout << "rotation task start" << '\n';
		t.restart();
		LOG(t.elapsed()); //時刻の記録(1行目)
	}


	/*
	//タスク終了/次のターゲットに進む(keyboard用)
	if (LogStartFlg == 1 && targetFlg == 0 && glfwGetKey(GLFW_KEY_ENTER) == GLFW_PRESS) {
		//targetNumber++;
		targetFlg = 1;
		NextTaskFlg = 1;
		//std::cout << targetFlg << "\t" << targetNumber << '\n';
	}else if (glfwGetKey(GLFW_KEY_ENTER) != GLFW_PRESS) { //キーから指が離れた後
		targetFlg = 0;
	}
	*/


	//rayCasting mode
	if (appMode != rayCasting && glfwGetKey('4') == GLFW_PRESS) { //一度だけ通るようにする
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


	//回転＋並進
	if (RotateTranslateMode == 0 && glfwGetKey('1') == GLFW_PRESS) { //一度だけ通るようにする
		RotateTranslateMode = 1;
		translateMode = 0;
		rotateMode = 0;
		tflag = 0;
	}
	if (RotateTranslateMode >= 1 && glfwGetKey('0') == GLFW_PRESS) {
		RotateTranslateMode = 0;
	}

	//並進のみ
	if (translateMode == 0 && glfwGetKey('T') == GLFW_PRESS) {
		translateMode = 1;
		RotateTranslateMode = 0;
		rotateMode = 0;
		tflag = 0;
	}
	if (translateMode >= 1 && glfwGetKey('Y') == GLFW_PRESS) {
		translateMode = 0;
	}
	
	//回転のみ
	if (rotateMode == 0 && glfwGetKey('R') == GLFW_PRESS) {
		rotateMode = 1;
		RotateTranslateMode = 0;
		translateMode = 0;
		tflag = 0;
	}
	if (rotateMode >= 1 && glfwGetKey('E') == GLFW_PRESS) {
		rotateMode = 0;
	}

	//フリックを使用するかどうか
	if (FlickEnable == 0 && glfwGetKey('F') == GLFW_PRESS) {
		FlickEnable = 1;
		//std::cout << "FlickEnable" << '\n';
	}
	if (FlickEnable == 1 && glfwGetKey('G') == GLFW_PRESS) {
		FlickEnable = 0;
		//std::cout << "FlickDisable" << '\n';
	}

	//フリックの停止処理
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
				//Scaled HOMER 使用時
				position = glm::vec3(-temp[0]/100, -temp[1]/100, -temp[2]/100);
				position += vec3(0.0,0.5,1.9);
				break;
			default:
				//提案手法使用時
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

//		raymodelMatrix = rot*transform; //回転だけであればこの行のみ


		//Scaled HOMER mode での処理
		if (ScaledHomerMode == 1) {
			orientation.w = -temp[3];
			initOrientation = glm::toMat4(orientation); //選択時のレシーバの姿勢を記憶(この姿勢を基準とする)
			//initPosition = inverse(trans); //選択時のレシーバの位置を記憶(この位置を基準とする)

			//並進を行列でなくベクトルで表した場合
			initPosition = position; //選択時のレシーバの位置を記憶(この位置を基準とする)
			prevPosition = position;
			initModelMatrix = modelMatrix;
			prevModelMatrix = modelMatrix;
			ScaledHomerMode = 2;
		}
		if (ScaledHomerMode == 2 && raymodelMatrix != previousRaymodelMatrix) {
			//modelMatrix = rot*inverse(initOrientation)*transform; //回転の同期(初期の姿勢を基準)
			//modelMatrix = rot*initOrientation*transform; //回転の同期(初期の姿勢を基準)
			modelMatrix = rot*initOrientation*initModelMatrix;

			//回転タスクであれば並進を行わない
			if (taskMode == RotationTask || PracticeNum >= 3){
				modelMatrix[3][0] = prevModelMatrix[3][0];
				modelMatrix[3][1] = prevModelMatrix[3][1];
				modelMatrix[3][2] = prevModelMatrix[3][2];

			//回転タスクでなければ並進(Scaled HOMER)を行う
			}else{
				vec3 V_hand = (-position) - (-prevPosition); //レシーバの速度(1フレームあたりのレシーバの変化量)
				//std::cout << V_hand[0] << "\t" << V_hand[1] << "\t" << V_hand[2] << std::endl;

				float SC = 0.007;
				float V_hand_d = 1.2 < sqrtf(pow(V_hand[0],2)+pow(V_hand[1],2)+pow(V_hand[2],2)) / SC ? 1.2 : sqrtf(pow(V_hand[0],2)+pow(V_hand[1],2)+pow(V_hand[2],2)) / SC; //速度のスケーリング
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
				//デバイスからの入力ログ(push B)
				LOGSTRING("B");

				//ボタンのカウント数のログ
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
				//デバイスからの入力ログ(push A)
				LOGSTRING("A");

				A_count = 1;
				LOGINT(A_count);
				A_count++;
				break;
			}
			if ((appInputState == idle || appInputState == rot || appInputState == trans) && remote.Button.A() && remote.Button.B() && objectHit && !(taskMode == RotationTask || PracticeNum >= 3)) {
				//if (appInputState == rot) デバイスからの入力ログ(pop A)
				//else if (appInputState == trans) デバイスからの入力ログ(pop B)
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
				//デバイスからの入力ログ(push B)
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

//				/*//並進ではフリックを使用しない
				//フリック処理
				if (FlickEnable == 1){
					translateFlickMode = 1;
//					t.restart(); // タイマーのリセット
					tflag = 0;
					tcount = 0; //カウントの初期化
				}
//				*/

				std::cout << "idle" << '\n';
				//デバイスからの入力ログ
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}
			if (appInputState == rot && !remote.Button.A()) {
				appInputState = idle;
				rotateMode = 0;

				//フリック処理
				if (FlickEnable == 1){
					rotateFlickMode = 1;
//					t.restart(); // タイマーのリセット
					tflag = 0;
					tcount = 0; //カウントの初期化
				}

				std::cout << "idle" << '\n';
				//デバイスからの入力ログ
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}
			if (appInputState == rot_and_trans && (!remote.Button.A() || !remote.Button.B())) {
				appInputState = idle;
				RotateTranslateMode = 0;

				//フリック処理
				if (FlickEnable == 1){
					RotTransFlickMode = 1;
//					t.restart(); // タイマーのリセット
					tflag = 0;
					tcount = 0; //カウントの初期化
				}

				std::cout << "idle" << '\n';
				//デバイスからの入力ログ
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
				//デバイスからの入力ログ
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}


			//ログ用
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
				//デバイスからの入力ログ(push B)
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
				//デバイスからの入力ログ
				LOGSTRING("idle");

				idle_count = 1;
				LOGINT(idle_count);
				idle_count++;
				break;
			}

			//ログ用
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
			//デバイスからの入力ログ(空)
			break;
		}
	}
}


void Engine::checkSpaceNavigator() { 

#define SNSCALE 0.2f
#define RTSCALE 5.0f

	float position[6];
	std::fill_n(position,6,0.0f);



	if (t.elapsed() >= 1.0 && PedalPushed == 1) PedalPushed = 0; //ペダルが押されて1.0秒経過したとき

	if (glfwGetJoystickPos( GLFW_JOYSTICK_1, position,6) == 6 ) {

		//std::cout << position[2] << std::endl;

		//タスク終了/次のターゲットに進む
		if (LogStartFlg == 1 && targetFlg == 0 && position[2] == 1 && PedalPushed == 0) {
			targetFlg = 1;
			NextTaskFlg = 1;
			PedalPushed = 1;
		}else if (position[2] != 1) { //キーから指が離れた後
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

	glEnable(GL_DEPTH_TEST); //depthBufferを有効
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
		//オブジェクトの周りのsphereを描画
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 1.0f);
		mat4 SphereModelMatrix;
		SphereModelMatrix = glm::translate(mat4(), vec3(modelMatrix[3]));

		pvm = projectionMatrix*viewMatrix*SphereModelMatrix; //calculate pvm
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.spheredraw();
	}


	//平面の描画
	glUseProgram(flatShader);
	glUniform4f(flatShaderColor, 0.8f, 0.8f ,0.8f, 1.0f);
	mat4 PlaneModelMatrix;
	PlaneModelMatrix = glm::translate(mat4(), vec3(0.1,-4.0,-15));

	pvm = projectionMatrix*viewMatrix*PlaneModelMatrix; //calculate pvm
	glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
	cursor.planedraw();

	glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 1.0f);
	cursor.planelinedraw();

	//影の描画
	mat4 ShadowModelMatrix;
	ShadowModelMatrix = glm::translate(mat4(), vec3(modelMatrix[3][0],-3.9,modelMatrix[3][2]));
	float scale = 1 / (modelMatrix[3][1] + 4.9);
	if (scale > 1) scale = 1;
	if (scale < 0) scale = 0; //オブジェクトが平面と重なったとき
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
	float targetrot; //ターゲットの回転量
	TargetModelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15)); //ターゲットを配置する球の中心となる位置



	switch(taskMode){
	case DockingTask:

		//タスクの切り替え
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
		TargetModelMatrix = glm::rotate(TargetModelMatrix, randRot, randAxis); //ランダムな回転
		break;


	//練習用タスクの処理(タスクの順番は固定)
	case PracticeTask:
		
		//タスクの切り替え
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
			TargetModelMatrix = glm::rotate(TargetModelMatrix, 60.0f, vec3(-1.0f, -1.0f, -1.0f)); //ランダムな回転
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
	//ターゲットの姿勢判別用のvertexを描画
	glUseProgram(shaderProgram);
	pvm = projectionMatrix*viewMatrix*TargetModelMatrix;
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm));
	cursor.vertexdraw();
	*/

	//ターゲットの影を描画
	glUseProgram(flatShader);
	ShadowModelMatrix = glm::translate(mat4(), vec3(TargetModelMatrix[3][0],-3.9,TargetModelMatrix[3][2]));
	scale = 1 / (TargetModelMatrix[3][1] + 4.9);
	if (scale > 1) scale = 1;
	if (scale < 0) scale = 0; //オブジェクトが平面と重なったとき
	pvm = projectionMatrix*viewMatrix*glm::scale(ShadowModelMatrix,glm::vec3(scale,scale,scale));
	glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
	glUniform4f(flatShaderColor, 0.0f, 0.0f ,0.0f, 0.5f);
////	cursor.shadowdraw();


	/*
	float w = sqrtf(modelMatrix[0][0]+modelMatrix[1][1]+modelMatrix[2][2]+1.0f) * 0.5f;
	float teata = acos(w)*2;
	std::cout << teata << '\n';
	*/

	//ターゲットとの角度誤差の計算
	mat4 rotMatrix = inverse(modelMatrix)*TargetModelMatrix;
	float w = sqrtf(rotMatrix[0][0]+rotMatrix[1][1]+rotMatrix[2][2]+1.0f) * 0.5f;
	float rot = degrees(acos(w)*2); //ターゲットとの角度誤差

	//ターゲットとの位置誤差の計算
	float length = sqrtf( pow(TargetModelMatrix[3][0]-modelMatrix[3][0], 2) 
						+ pow(TargetModelMatrix[3][1]-modelMatrix[3][1], 2)
						+ pow(TargetModelMatrix[3][2]-modelMatrix[3][2], 2));

//	std::cout << rot << '\t' << length << '\n';
	LOG(rot);
	LOG(length);


	//ターゲットとカーソルのvertexごとの距離の計算
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

	//vertexの色の変更
	if (vertexD1 < vertLength) cursor.vertexColorChange(0);
	else cursor.vertexColorRes(0);
	if (vertexD2 < vertLength) cursor.vertexColorChange(1);
	else cursor.vertexColorRes(1);
	if (vertexD3 < vertLength) cursor.vertexColorChange(2);
	else cursor.vertexColorRes(2);
	if (vertexD4 < vertLength) cursor.vertexColorChange(3);
	else cursor.vertexColorRes(3);

	//次のタスクに移動できることを示す音
	if (vertexD1 < vertLength && vertexD2 < vertLength && vertexD3 < vertLength && vertexD4 < vertLength && DockingCompFlg == 0 && LogStartFlg == 1){
		//std::cout << "OK" << '\n';
		DockingCompFlg = 1;
		mciSendString(TEXT("play myFile from 0"), NULL, 0, 0);
	}

	//オブジェクト(カーソル)の姿勢判別用
	glUseProgram(shaderProgram);
	pvm = projectionMatrix*viewMatrix*modelMatrix;
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm));
////	cursor.vertexdraw();

	//ターゲットの姿勢判別用のvertexを描画
	glUseProgram(shaderProgram);
	pvm = projectionMatrix*viewMatrix*TargetModelMatrix;
	glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(pvm));
////	cursor.vertexdraw();


	//オブジェクトの描画
	/*//透明
	glUseProgram(targetShader);  //select shader
	pvm = projectionMatrix*viewMatrix*modelMatrix; //calculate pvm
	glUniformMatrix4fv(targetShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm)); //upload it	
	cursor.draw();
	*/


	if ((appMode == rayCastingOfSphere && cursor.findSphereIntersection(modelMatrix,raymodelMatrix,objectIntersectionPoint,sphereIntersectionDistance,sphereIntersectionNormal)) ||
		(appMode == rayCasting         && cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint))										 ||
		(appMode == ScaledHOMER        && cursor.findIntersection(RotateTranslateMode,modelMatrix,raymodelMatrix,objectIntersectionPoint) && ScaledHomerMode == 0)){

		/*
		//オブジェクトの強調表示の描画
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 1.0f ,0.0f, 0.2f); //緑
		if (RotateTranslateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 0.0f ,0.0f, 0.2f); //赤
		if (translateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 1.0f ,0.0f, 0.2f); //黄
		if (rotateMode >= 1) glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.2f); //青
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.03f,1.03f,1.03f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.draw();

		//triangleの描画
		glUniform4f(flatShaderColor, 0.0f, 1.0f ,0.0f, 0.3f); //緑
		if (RotateTranslateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 0.0f ,0.0f, 0.3f); //赤
		if (translateMode >= 1) glUniform4f(flatShaderColor, 1.0f, 1.0f ,0.0f, 0.3f); //黄
		if (rotateMode >= 1) glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.3f); //青
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
	//オブジェクトは選択されていないが、rotateModeのとき(rotateModeではrayがオブジェクトから離れることもありうる)
	}else if (rotateMode >= 1) {
		//rotation用の強調表示
		glUseProgram(flatShader);
		glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.1f); //青
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.1f,1.1f,1.1f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.draw();

		glUniform4f(flatShaderColor, 0.0f, 0.0f ,1.0f, 0.2f); //青
		pvm = projectionMatrix*viewMatrix*glm::scale(modelMatrix,glm::vec3(1.11f,1.11f,1.11f));
		glUniformMatrix4fv(flatShaderPvm, 1, GL_FALSE, glm::value_ptr(pvm));
		cursor.Trdraw(); //triangleの描画
	*/
	}


	//rayの描画(Scaled HOMER mode では描画しない)
	if (ScaledHomerMode == 0) {
		glUseProgram(shaderProgram);  //select shader
		raypvm = rayprojectionMatrix*rayviewMatrix*raymodelMatrix; //calculate pvm
		glUniformMatrix4fv(shaderProgramPVM, 1, GL_FALSE, glm::value_ptr(raypvm)); //upload it
		glBindVertexArray(rayVAO); //select ray
		glDrawArrays(GL_TRIANGLE_STRIP,0,rayVerticesCount);
	}

	//オブジェクト、rayのログ
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

	//タスク終了時の処理(ドッキングタスク)
	if (NextTaskFlg == 1){
	//if (rot < 10 && length < 0.5){

		switch(taskMode){
			case PracticeTask:
				std::cout << PracticeNum << '\n'; //終了したタスク
				//初期化
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

				if (targetNumber == 1) Logger::resultInitialize("docking_task_result.txt"); //タスクの結果用ファイルを作成
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
				//初期化
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

				//タスク一巡の終了処理
				//if (targetNumber == 12){
				if (targetNumber == 23){
					Logger::Fileclose();

					mciSendString(TEXT("play myFile2 from 0"), NULL, 0, 0);
					_mkdir("log");

					switch(appMode){
					case rayCasting:
						_mkdir("log\\rayCasting");
						CopyFile( _T("docking_task_log.txt"), _T("log\\rayCasting\\docking_task_log.txt"), FALSE ); //ログファイルのコピー
						CopyFile( _T("docking_task_result.txt"), _T("log\\rayCasting\\docking_task_result.txt"), FALSE );
						break;
					case rayCastingOfSphere:
						_mkdir("log\\rayCastingOfSphere");
						CopyFile( _T("docking_task_log.txt"), _T("log\\rayCastingOfSphere\\docking_task_log.txt"), FALSE ); //ログファイルのコピー
						CopyFile( _T("docking_task_result.txt"), _T("log\\rayCastingOfSphere\\docking_task_result.txt"), FALSE );
						break;
					case ScaledHOMER:
						_mkdir("log\\ScaledHOMER");
						CopyFile( _T("docking_task_log.txt"), _T("log\\ScaledHOMER\\docking_task_log.txt"), FALSE ); //ログファイルのコピー
						CopyFile( _T("docking_task_result.txt"), _T("log\\ScaledHOMER\\docking_task_result.txt"), FALSE );
						break;
					}

					Logger::Initialize("dast_log.txt"); //タスク終了後のログはこちらに移す
					LogStartFlg = 0;
					std::cout << "docking task end" << '\n';

				}else targetNumber++;
				NextTaskFlg = 0;
				DockingCompFlg = 0;
				//t.restart(); // タイマーのリセット
				restartFlg = 1;
				break;



			case RotationTask:
		
				LOGLINE;
				LOGLINE;
				Logger::Fileclose();

				if (rotateNum == 1) Logger::resultInitialize("rotation_task_result.txt"); //タスクの結果用ファイルを作成
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
				//初期化
				//modelMatrix = glm::translate(mat4(), vec3(1.3,1.3,-15));
				appInputState = idle;
				RotateTranslateMode = 0;
				translateMode = 0;
				rotateMode = 0;
				RotTransFlickMode = 0;
				translateFlickMode = 0;
				rotateFlickMode = 0;
				idle_count = 1;

				//タスク一巡の終了処理
				if (rotateNum == 20){
					Logger::Fileclose();

					mciSendString(TEXT("play myFile2 from 0"), NULL, 0, 0);
					_mkdir("log");

					switch(appMode){
					case rayCasting:
						_mkdir("log\\rayCasting");
						CopyFile( _T("rotation_task_log.txt"), _T("log\\rayCasting\\rotation_task_log.txt"), FALSE ); //ログファイルのコピー
						CopyFile( _T("rotation_task_result.txt"), _T("log\\rayCasting\\rotation_task_result.txt"), FALSE );
						break;
					case rayCastingOfSphere:
						_mkdir("log\\rayCastingOfSphere");
						CopyFile( _T("rotation_task_log.txt"), _T("log\\rayCastingOfSphere\\rotation_task_log.txt"), FALSE ); //ログファイルのコピー
						CopyFile( _T("rotation_task_result.txt"), _T("log\\rayCastingOfSphere\\rotation_task_result.txt"), FALSE );
						break;
					case ScaledHOMER:
						_mkdir("log\\ScaledHOMER");
						CopyFile( _T("rotation_task_log.txt"), _T("log\\ScaledHOMER\\rotation_task_log.txt"), FALSE ); //ログファイルのコピー
						CopyFile( _T("rotation_task_result.txt"), _T("log\\ScaledHOMER\\rotation_task_result.txt"), FALSE );
						break;
					}

					Logger::Initialize("dast_log.txt"); //タスク終了後のログはこちらに移す
					LogStartFlg = 0;
					std::cout << "rotation task end" << '\n';

				}else rotateNum++;

				//ランダムな回転を与える
				randRot = rand()%180;
				randAxis[0] = rand()%101-50;
				randAxis[1] = rand()%101-50;
				randAxis[2] = rand()%101-50;
				randAxis = normalize(randAxis);

				//軸が不定な場合の処理
				while (randAxis[0] == 0 && randAxis[1] == 0 && randAxis[2] == 0) { 
					randAxis[0] = rand()%101-50;
					randAxis[1] = rand()%101-50;
					randAxis[2] = rand()%101-50;
					randAxis = normalize(randAxis);
				}

				NextTaskFlg = 0;
				DockingCompFlg = 0;
				//t.restart(); // タイマーのリセット
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
	srand((unsigned)time(NULL)); //乱数の初期化
	for(i=0; i<TARGETNUMBER; i++) TargetNum[i] = i+1;
	
	for(i=0; i<TARGETNUMBER; i++){
		a = rand()%TARGETNUMBER;
		b = rand()%TARGETNUMBER;
		swap(TargetNum[a],TargetNum[b]);
	}

	//ドッキングタスクの二順目の作成
	int tmpNum[TARGETNUMBER];
	for(i=0; i<TARGETNUMBER; i++) tmpNum[i] = i+1;
	
	for(i=0; i<TARGETNUMBER; i++){
		a = rand()%TARGETNUMBER;
		b = rand()%TARGETNUMBER;
		swap(tmpNum[a],tmpNum[b]);
	}
	
	for(i=0; i<TARGETNUMBER; i++) TargetNum[i+TARGETNUMBER] = tmpNum[i];

	/*
	//デバッグ用(1～12の数が重複無く表示されているかどうか)
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

	if (!(rot[0] == 0 && rot[1] == 0 && rot[2] == 0)) { //軸が不定でないとき
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
		//フリック処理(判定)
		if (FlickEnable == 1){
			if (t.elapsed() > FLICKJUDGETIME){
				endRayPosition = end;
				float rayMoveDistance = sqrt(pow(endRayPosition[0]-firstRayPosition[0],2)+pow(endRayPosition[1]-firstRayPosition[1],2)+pow(endRayPosition[2]-firstRayPosition[2],2));

				if (rayMoveDistance >= FLICKJUDGEDISTANCE){
					flickRot = rot;
					flickMove = move;
					RotateTranslateMode = 0; //オブジェクトが交点から離れる
					RotTransFlickMode = 1;
					tcount = 0; //カウントの初期化
				}

				std::cout << rayMoveDistance << "\t" << t.elapsed() << " sec" << std::endl;
				t.restart(); // タイマーのリセット
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
	if (!(rot[0] == 0 && rot[1] == 0 && rot[2] == 0)) { //軸が不定でないとき
		modelMatrix = glm::rotate(modelMatrix, rot[3], vec3(rot[0], rot[1], rot[2]));

		/*
		vec4 end = modelMatrix*vec4(arcBallPreviousPoint[0],arcBallPreviousPoint[1],arcBallPreviousPoint[2],1.0f);
		//フリック処理(判定)
		if (FlickEnable == 1){
			if (t.elapsed() > FLICKJUDGETIME){
				endRayPosition = end;
				float rayMoveDistance = sqrt(pow(endRayPosition[0]-firstRayPosition[0],2)+pow(endRayPosition[1]-firstRayPosition[1],2)+pow(endRayPosition[2]-firstRayPosition[2],2));

				if (rayMoveDistance >= FLICKJUDGEDISTANCE){
					flickRot = rot;
					rotateMode = 0; //オブジェクトが交点から離れる
					rotateFlickMode = 1;
					tcount = 0; //カウントの初期化
				}
				t.restart(); // タイマーのリセット
				tflag = 0;
			}
		}
		*/
		flickRot = rot;

		//オブジェクトの面とrayは連動させるべきなので、この処理は必要ない
		//arcBallPreviousPoint[0] = xpos;
		//arcBallPreviousPoint[1] = ypos;
		//arcBallPreviousPoint[2] = zpos;
	}
	limitRot = rot; //回転の限界時に使用
}


///////////////////////////////////////////////
// 回転行列→クォータニオン変換
//
// qx, qy, qz, qw : クォータニオン成分（出力）
// m11-m33 : 回転行列成分

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

    // 最大成分を検索
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
        return false; // 引数の行列に間違いあり！

    // 最大要素の値を算出
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
	Logger::Initialize("practice_log.txt"); //ログファイルの作成
	modelinit();
	rayinit();
	framebufferinit();
	targetinit();
	taskshuffle(); //タスクの順番をランダムにする
	t.restart(); // タイマーの初期化
	//rendering loop
	while(true) {
		//LOG(t.elapsed()); //時刻の記録
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