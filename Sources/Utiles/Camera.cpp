#include "Camera.h"

#define M_PI 3.14159265358979323846264338327

//  Default
Camera::Camera(float screenRatio)
{
    configuration = FREEFLY;

    position = glm::vec3(0,-10,5);
    forward  = glm::vec3(0,0,1)- position;

    teta = 0;           phi = 0;
    sensivity = 0.2f;    speedMag = 0.003f; // = 3m/ms
    radius = 3;         radiusMin = 1;
    radiusMax = 10;
	frustrumAngleVertical = 30.0f;
	frustrumAngleHorizontal = screenRatio*frustrumAngleVertical;
	
    anglesFromVectors();
    vectorsFromAngles();
}
Camera::~Camera()
{
    
}
//

//  Public functions
void Camera::animate(float elapseTime,bool goForw,bool goBack,bool goLeft,bool goRight,bool option1,bool option2)
{
    mutex.lock();

    float tmpSpeed = speedMag;
	glm::vec3 direction(0.,0.,0.);

    switch(configuration&MODE_MASK)
    {
        case FREEFLY:
            if(!EventHandler::getInstance()->getCursorMode())
            {
                phi  -= sensivity*EventHandler::getInstance()->getCursorPositionRelative().x;
                teta -= sensivity*EventHandler::getInstance()->getCursorPositionRelative().y;
                vectorsFromAngles();
            }
            if(goForw) direction += forward;
            if(goBack) direction -= forward;
            if(goLeft) direction += left;
            if(goRight) direction -= left;

            if(option2) tmpSpeed /= 10.f;
            if(option1) tmpSpeed *= 10.f;

			if (direction.x || direction.z || direction.z)
				position += glm::normalize(direction)*elapseTime*tmpSpeed;
            break;

        case CINEMATIC:
            break;

        case TRACKBALL:
            if(!EventHandler::getInstance()->getCursorMode())
            {
                phi  -= sensivity*EventHandler::getInstance()->getCursorPositionRelative().x;
                teta -= sensivity*EventHandler::getInstance()->getCursorPositionRelative().y;
                vectorsFromAngles();
            }
            break;

        case ISOMETRIC:
            if(option2)
            {
                phi  -= sensivity*EventHandler::getInstance()->getCursorPositionRelative().x;
                teta -= sensivity*EventHandler::getInstance()->getCursorPositionRelative().y;
                vectorsFromAngles();
            }

            if(option1) tmpSpeed *= 10.f;

			if (goForw) direction += glm::normalize(glm::dvec3(forward.x, forward.y, 0));
			if (goBack) direction -= glm::normalize(glm::dvec3(forward.x, forward.y, 0));
			if (goLeft) direction += left;
			if (goRight) direction -= left;

			if (direction.x || direction.z || direction.z)
				position += glm::normalize(direction)*elapseTime*tmpSpeed;
			break;

        default: break;
    }

    mutex.unlock();
}
glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 m;
    mutex.lock();
	m = glm::lookAt(position, position + forward, glm::vec3(0, 0, 1));
    mutex.unlock();
	return m;
}
void Camera::move(glm::vec3 v)
{
	glm::vec3 p = v + getPosition();
    setPosition(p);
}

void Camera::pause()
{
    mutex.lock();
    if((configuration&MODE_MASK) == TRACKBALL)
        configuration &= ~PLAY;
    mutex.unlock();
}
void Camera::play()
{
    mutex.lock();
    if((configuration&MODE_MASK) == TRACKBALL)
        configuration |= PLAY;
    mutex.unlock();
}
//

//  Set/get functions
void Camera::setMode(CameraMode mode)
{
    mutex.lock();
    configuration &= ~MODE_MASK;
    configuration |= mode;

    switch(mode)
    {
        case CINEMATIC:
            configuration |= PLAY;
            break;

        default:
            configuration &= ~PLAY;
            break;
    }
    mutex.unlock();
}
void Camera::setSpeed(float s)
{
    mutex.lock();
    speedMag = s;
    mutex.unlock();
}
void Camera::setRadiusMin(float r)
{
    mutex.lock();
    radiusMin = r;
    boundingRadius();
    mutex.unlock();
}
void Camera::setRadiusMax(float r)
{
    mutex.lock();
    radiusMax = r;
    boundingRadius();
    mutex.unlock();
}
void Camera::setRadius(float r)
{
    mutex.lock();
    radius = r;
    boundingRadius();
    mutex.unlock();
}
void Camera::setAllRadius(float r, float rmin, float rmax)
{
    mutex.lock();
    radius = r;
    radiusMax = rmax;
    radiusMin = rmin;
    boundingRadius();
    mutex.unlock();
}
void Camera::setPosition(glm::vec3 pos)
{
    mutex.lock();
    if((configuration&MODE_MASK)==TRACKBALL)
    {
		glm::vec3 target = position + radius*forward;
        position += pos;
        forward = target - position;
        radius = forward.length();
		forward = glm::normalize(forward);

        boundingRadius();
        anglesFromVectors();
        vectorsFromAngles(target);
    }
    else position = pos;
    mutex.unlock();
}
void Camera::setOrientation(glm::vec3 orientation)
{
    mutex.lock();
    forward = glm::normalize(orientation);
    anglesFromVectors();
    vectorsFromAngles();
    mutex.unlock();
}
void Camera::setTarget(glm::vec3 target)
{
    mutex.lock();
    forward = target-position;
    radius = forward.length();
	forward = glm::normalize(forward);

    boundingRadius();
    anglesFromVectors();
    vectorsFromAngles(target);
    mutex.unlock();
}
void Camera::setFrustrumAngleVertical(float angle)
{
    mutex.lock();
    frustrumAngleVertical = angle;
    mutex.unlock();
}
void Camera::setFrustrumAngleHorizontal(float angle)
{
    mutex.lock();
    frustrumAngleHorizontal = angle;
    mutex.unlock();
}
void Camera::setSensitivity(float sens)
{
    mutex.lock();
    sensivity = sens;
    mutex.unlock();
}


Camera::CameraMode Camera::getMode()
{
    CameraMode m;
    mutex.lock();
    m = (CameraMode)(configuration&MODE_MASK);
    mutex.unlock();
    return m;
}
glm::vec3 Camera::getTarget()
{
	glm::vec3 v;
    mutex.lock();
    if((configuration&MODE_MASK) == TRACKBALL) v = position + radius*forward;
    else v = position + forward;
    mutex.unlock();
    return v;
}
glm::vec3 Camera::getLeft()
{
	glm::vec3 l;
    mutex.lock();
    l = left;
    mutex.unlock();
    return l;
}
glm::vec3 Camera::getForward()
{
	glm::vec3 f;
    mutex.lock();
    f = forward;
    mutex.unlock();
    return f;
}
glm::vec3 Camera::getVertical()
{
	glm::vec3 v;
    mutex.lock();
    v = vertical;
    mutex.unlock();
    return v;
}
glm::vec3 Camera::getPosition()
{
	glm::vec3 p;
    mutex.lock();
    p = position;
    mutex.unlock();
    return p;
}
float Camera::getSpeed()
{
    float s;
    mutex.lock();
    s = speedMag;
    mutex.unlock();
    return s;
}
double Camera::getRadiusMin()
{
    double r;
    mutex.lock();
    r = radiusMin;
    mutex.unlock();
    return r;
}
double Camera::getRadiusMax()
{
    double r;
    mutex.lock();
    r = radiusMax;
    mutex.unlock();
    return r;
}
double Camera::getRadius()
{
    double r;
    mutex.lock();
    r = radius;
    mutex.unlock();
    return r;
}
float Camera::getFrustrumAngleVertical()
{
    float a;
    mutex.lock();
    a = frustrumAngleVertical;
    mutex.unlock();
    return a;
}
float Camera::getFrustrumAngleHorizontal()
{
    float a;
    mutex.lock();
    a = frustrumAngleHorizontal;
    mutex.unlock();
    return a;
}
float Camera::getSensitivity()
{
    float s;
    mutex.lock();
    s = sensivity;
    mutex.unlock();
    return s;
}
//

//  Private functions
void Camera::vectorsFromAngles(glm::vec3 target)
{
    if(phi>180.f) phi -= 360.f;
    else if(phi < -180.f) phi += 360.f;
    if(teta>89.f) teta = 89.f;
    else if(teta < -89.f) teta = -89.f;

	float radTeta = glm::radians(teta);
	float radPhi = glm::radians(phi);

    switch(configuration&MODE_MASK)
    {
        case FREEFLY: case ISOMETRIC:
			forward.x = cos(radTeta)*cos(radPhi);
			forward.y = cos(radTeta)*sin(radPhi);
			forward.z = sin(radTeta);
            break;

        case TRACKBALL:
			forward.x = cos(radTeta)*cos(radPhi);
			forward.y = cos(radTeta)*sin(radPhi);
			forward.z = sin(radTeta);
            position = target - radius*forward;
            break;

        default: break;
    }

	left = glm::normalize(glm::cross(glm::vec3(0, 0, 1), forward));
	vertical = glm::normalize(glm::cross(forward, left));
}
void Camera::anglesFromVectors()
{
	teta = (float) atan2(forward.z, sqrt(forward.x*forward.x + forward.y*forward.y))*180 / (float)M_PI;
	phi = (float) atan2(forward.y, forward.x)*180 / (float)M_PI;
}
void Camera::boundingRadius()
{
	radius = max(min(radius, radiusMax), radiusMin);
}
//
