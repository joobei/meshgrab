#include "arcball.h"
#include <iostream>

static const int WINDOW_SIZE_X = 800;
static const int WINDOW_SIZE_Y = 600;


glm::vec3 pho::util::getPointOnSphere(int px, int py) {
    float x,y;

//    x = (float)px/(pho::Engine::WINDOW_SIZE_X/2);
//    y = (float)py/(pho::Engine::WINDOW_SIZE_Y/2);
    x = (float)px/(WINDOW_SIZE_X/2);
    y = (float)py/(WINDOW_SIZE_Y/2);

    x = x-1;
    y = 1-y;

    double z2 = 1 - x * x - y * y;
    double z = z2 > 0 ? glm::sqrt(z2) : 0;

    glm::vec3 p = glm::vec3(x, y, z);
    p = glm::normalize(p);
    //std::cout << "x:" << p.x << "\t y:" << p.y << "\t z:" << p.z << "\t px:" << px << "\t py:" << py << std::endl;
    return p;
}

glm::vec3 pho::util::getPointOnSphereTouch(float x, float py) {

    float y = 1-py;

    //std::cout << "GetPointOnSphereTouch: \t";

    double z2 = 1 - x * x - y * y;
    double z = z2 > 0 ? glm::sqrt(z2) : 0;

    glm::vec3 p = glm::vec3(x, y, z);
    p = glm::normalize(p);
    //std::cout << "x:" << p.x << "\t y:" << p.y << "\t z:" << p.z << "\t px:" << x << "\t py:" << py << std::endl;
    return p;
}

glm::vec4 pho::util::getRotation(float oldx, float oldy, float oldz, float newx, float newy, float newz, float cx, float cy, float cz, bool touch) {

    glm::vec3 oldp,newp;
/*
    oldp = getPointOnSphere(oldx,oldy);
    newp = getPointOnSphere(newx,newy);
//	std::cout << "old: " << oldp[0] << "\t" << oldp[1] << "\t" << oldp[2] << std::endl;
//	std::cout << "new: " << newp[0] << "\t" << newp[1] << "\t" << newp[2] << std::endl;
*/

	oldp = glm::vec3(oldx, oldy, oldz);
	newp = glm::vec3(newx, newy, newz);

	oldp = glm::normalize(oldp);
    newp = glm::normalize(newp);

//	std::cout << "old: " << oldp[0] << "\t" << oldp[1] << "\t" << oldp[2] << std::endl;
//	std::cout << "new: " << newp[0] << "\t" << newp[1] << "\t" << newp[2] << std::endl;
//	std::cout << newp[0] - oldp[0] << "\t" << newp[1] - oldp[1] << "\t" << newp[2] - oldp[2] << std::endl;

    glm::vec3 axis = glm::cross(oldp,newp);
//	std::cout << "axis: " << axis[0] << "\t" << axis[1] << "\t" << axis[2] << std::endl;
//    float vangle = (glm::angle(oldp,newp)/50); //Šp“x
	float vangle = glm::angle(oldp,newp); //Šp“x
/*
	float pi = 3.1415926535897932384626433832795;
	float vangle = acos(oldp[0]*newp[0]+oldp[1]*newp[1]+oldp[2]*newp[2]);
	vangle = vangle*(180/pi);
*/
	if (!(vangle > 0) && !(vangle < 0)) vangle = 0; //’l‚ª‚¨‚©‚µ‚¢‚Æ‚«
//	std::cout << "angle: " << vangle << std::endl;

	glm::vec4 rotation = glm::vec4(axis,vangle);

	return rotation;
/*
    //glm::gtx::quaternion::quat rotation = glm::gtx::quaternion::angleAxis(vangle,axis.x,axis.y,axis.z);
    glm::mat4 rotation;

    rotation = glm::axisAngleMatrix(glm::vec3(axis.x,axis.y,axis.z),vangle);

    //if the sum of squares of any row are not = 1 then we don't have a rotation matrix
    if (rotation[0][0]*rotation[0][0]+rotation[0][1]*rotation[0][1]+rotation[0][2]*rotation[0][2]+rotation[0][3]*rotation[0][3]!=1) {
        return glm::mat4();
    }
    else {
        return rotation;
    }
*/
}
