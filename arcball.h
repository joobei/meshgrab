#ifndef ARCBALL_H
#define ARCBALL_H

//#include "photonio.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtx/matrix_interpolation.hpp"


namespace pho {

 namespace util {

glm::vec3 getPointOnSphere(int x, int y);

glm::vec3 getPointOnSphereTouch(float x, float y);

glm::vec4 getRotation(float oldx, float oldy, float oldz, float newx, float newy, float newz, float cx, float cy, float cz, bool touch);

 }


}

#endif
