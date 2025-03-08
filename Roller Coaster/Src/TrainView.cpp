#include "TrainView.h"  
#include <iostream>

TrainView::TrainView(QWidget *parent) :  
QGLWidget(parent)  
{  
	resetArcball();
}  
TrainView::~TrainView()  
{}  

void TrainView::initializeGL()
{
	initializeOpenGLFunctions();

	//Initialize texture 
	initializeTexture();

	terrain = new Terrain();
	terrain->Init();

	skybox = new CubemapTexture("./Textures", "right.png", "left.png", "top.png", "bottom.png", "front.png", "back.png");
	skybox->Init();

	eagle = new MovingObject();
	eagle->Init(0);

	car = new MovingObject();
	car->Init(1);

	ball = new MovingObject();
	ball->Init(2);

	water = new Water();
	water->Init();

	//fbo for reflect
	glGenFramebuffers(1, &water->FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, water->FramebufferName);

	water->GenTexture(width(), height());

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, water->reflectTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//fbo for refract
	glGenFramebuffers(1, &water->refractFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, water->refractFBO);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, water->refractTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers1[1] = {GL_COLOR_ATTACHMENT1};
	glDrawBuffers(1, DrawBuffers1); // "1" is the size of DrawBuffers
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*char *versionGL;
	versionGL = (char *)(glGetString(GL_VERSION));
	std::cout << "OpenGL version: " << versionGL << endl;*/
}

void TrainView::initializeTexture()
{
	//Load and create a texture for square;'stexture
	QOpenGLTexture* texture = new QOpenGLTexture(QImage("./Textures/colormap.png"));//0
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/PflastergrauSPEC.png"));//1
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/Stump_CU.png"));//2
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/car.png"));//3
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/waves5/000.png"));//4
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/beitun_height.png"));//5
	Textures.push_back(texture);
	texture = new QOpenGLTexture(QImage("./Textures/beitun_spec.png"));//6
	Textures.push_back(texture);
}

void TrainView:: resetArcball()
	//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, 0, 0);

	/*QVector3D refractStart = Quat2Euler(arcball.start);
	refractStart = GetRefract(refractStart);
	arcball.start = Euler2Quat(refractStart);
	QVector3D refractNow = Quat2Euler(arcball.now * arcball.start);
	refractNow = GetRefract(refractNow);
	arcball.start = Euler2Quat(refractNow);
	arcball.now = Quat();
	arcball.setProjection();*/

	/*float rate = GetRefractAngle();
	QVector3D mirrorStart = Quat2Euler(arcball.start);
	mirrorStart = QVector3D(mirrorStart.x() * rate, mirrorStart.y(), mirrorStart.z() * rate);
	arcball.start = Euler2Quat(mirrorStart);
	QVector3D mirrorNow = Quat2Euler(arcball.now);
	mirrorNow = QVector3D(mirrorNow.x() * rate, mirrorNow.y(), mirrorNow.z() * rate);
	arcball.now = Euler2Quat(mirrorNow);
	arcball.setProjection();*/
}

QVector3D TrainView::getCamPos()
{
	arcball.start.renorm();
	arcball.now.renorm();
	Quat pos = (arcball.now * arcball.start) * Quat(arcball.eyeX, arcball.eyeY, arcball.eyeZ, 0) * ((arcball.now * arcball.start).conjugate());
	return QVector3D(pos.x, pos.y, pos.z);
}

QVector3D TrainView::Quat2Euler(Quat q1)
{
	float heading, /*rotation about y axis*/
		  attitude, /*rotation about z axis*/
		  bank, /*rotation about x axis*/
		  PI = 3.14159;
	
	double test = q1.x*q1.y + q1.z*q1.w;
	if (test > 0.499) { // singularity at north pole
		heading = 2 * atan2(q1.x, q1.w);
		attitude = PI / 2;
		bank = 0;
		return QVector3D(bank, heading, attitude);
	}
	if (test < -0.499) { // singularity at south pole
		heading = -2 * atan2(q1.x, q1.w);
		attitude = -PI / 2;
		bank = 0;
		QVector3D(bank, heading, attitude);
	}
	double sqx = q1.x*q1.x;
	double sqy = q1.y*q1.y;
	double sqz = q1.z*q1.z;
	heading = atan2(2 * q1.y*q1.w - 2 * q1.x*q1.z, 1 - 2 * sqy - 2 * sqz);
	attitude = asin(2 * test);
	bank = atan2(2 * q1.x*q1.w - 2 * q1.y*q1.z, 1 - 2 * sqx - 2 * sqz);
	return QVector3D(bank, heading, attitude);
}

Quat TrainView::Euler2Quat(QVector3D e1)
{

	float heading = e1.y(), /*rotation about y axis*/
		  attitude = e1.z(), /*rotation about z axis*/
		  bank = e1.x(), /*rotation about x axis*/
		  w, x, y, z;
	// Assuming the angles are in radians.
	double c1 = cos(heading);
	double s1 = sin(heading);
	double c2 = cos(attitude);
	double s2 = sin(attitude);
	double c3 = cos(bank);
	double s3 = sin(bank);
	w = sqrt(1.0 + c1 * c2 + c1*c3 - s1 * s2 * s3 + c2*c3) / 2.0;
	double w4 = (4.0 * w);
	x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4;
	y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4;
	z = (-s1 * s3 + c1 * s2 * c3 + s2) / w4;
	return Quat(x, y, z, w);
}

float TrainView::GetRefractAngle()
{
	QVector3D camPos = getCamPos();
	float theta0 = acos(QVector3D::dotProduct(camPos, QVector3D(0,1,0))/camPos.length());
	float theta1 = asin(sin(theta0) / 1.33);
	theta0 = acos(sin(theta0));
	theta1 = acos(sin(theta1));
	float rate = theta1 / theta0;
	return theta1 - theta0;
}

float TrainView::GetTrainRefractAngle()
{
	QVector3D camPos = QVector3D(m_pTrack->trainLookAt.camDirection.x, m_pTrack->trainLookAt.camDirection.y, m_pTrack->trainLookAt.camDirection.z);
	float theta0 = acos(QVector3D::dotProduct(camPos, QVector3D(0, 1, 0)) / camPos.length());
	float theta1 = asin(sin(theta0) / 1.33);
	theta0 = acos(sin(theta0));
	theta1 = acos(sin(theta1));
	float rate = theta1 / theta0;
	return theta1 - theta0;
}

QVector3D TrainView::GetRefract(QVector3D v)
{
	//取餘角
	float thetaX = v.x();
	thetaX = acos(sin(thetaX));
	//算折射
	thetaX = asin(sin(thetaX) / 1.33);
	//取餘角
	thetaX = acos(sin(thetaX));

	float thetaZ = v.z();
	thetaZ = acos(sin(thetaZ));
	thetaZ = asin(sin(thetaZ) / 1.33);
	thetaZ = acos(sin(thetaZ));

	return QVector3D(thetaX, v.y(), thetaZ);
}

void TrainView::paintGL()
{
	/*const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string

	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);*/
	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	// Set up the view port
	glViewport(0,0,width(),height());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_CLIP_DISTANCE0);

	// top view only needs one light
	if (this->camera == 1) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]		= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);

	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	/*setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(400,10);*/


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff(false, true);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	drawStuff(false, false, true);
	glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
	glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	drawStuff();
	//*********************************************************************
	// now draw the terrain
	//*********************************************************************
	terrain->Begin();

	int viewport[4];
	// get matrixs and viewport:
	GLdouble matModelView[16];
	GLdouble matProjection[16];
	GLdouble camera_pos[3];
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluUnProject((viewport[2] - viewport[0]) / 2, (viewport[3] - viewport[1]) / 2,
		0.0, matModelView, matProjection, viewport,
		&camera_pos[0], &camera_pos[1], &camera_pos[2]);
	QVector3D camPos = QVector3D(camera_pos[0], camera_pos[1], camera_pos[2]);
	
	glActiveTexture(GL_TEXTURE0);//選擇要傳的texture unit
	switch (terrain->terrainType) {
	case 0:
		Textures[0]->bind();//把實際上的tex綁到unit上
		break;
	case 1:
		Textures[1]->bind();//把實際上的tex綁到unit上
		break;
	case 2:
		Textures[2]->bind();//把實際上的tex綁到unit上
		break;
	case 3:
		Textures[6]->bind();//把實際上的tex綁到unit上
		break;
	default:
		Textures[0]->bind();//把實際上的tex綁到unit上
		break;
	}
	terrain->shaderProgram->setUniformValue("Texture", 0);//把編號為0(GL_TEXTURE0)的unit傳進shader
	glActiveTexture(GL_TEXTURE1);
	Textures[5]->bind();
	terrain->shaderProgram->setUniformValue("heightmap", 1);
	terrain->shaderProgram->setUniformValue("clipType", 0);
	terrain->shaderProgram->setUniformValue("gEyeWorldPos", camPos);
	terrain->Paint(ProjectionMatrex, ModelViewMatrex);
	terrain->End();

	// this time drawing is for shadows (except for top view)
	if (this->camera != 1) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(width()) / static_cast<float>(height());

	// Check whether we use the world camp
	if (this->camera == 0){
		arcball.setProjection(false);
		update();
	// Or we use the top cam
	}
	else if (this->camera == 1) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
		update();
	} 
	// Or do the train view or other view here
	//####################################################################
	// TODO: 
	// put code for train view projection here!	
	//####################################################################
	else {
#ifdef EXAMPLE_SOLUTION
		trainCamView(this,aspect);
#endif
		glMatrixMode(GL_PROJECTION);

		// Compute the aspect ratio so we don't distort things
		gluPerspective(40, aspect, .1, 1000);

		// Put the camera where we want it to be
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Pnt3f camPos, centerPos, up;
		camPos = m_pTrack->trainLookAt.camPosition;
		centerPos = m_pTrack->trainLookAt.camPosition + m_pTrack->trainLookAt.camDirection;
		up = m_pTrack->trainLookAt.camUp;

		gluLookAt(camPos.x, camPos.y, camPos.z, centerPos.x, centerPos.y, centerPos.z, up.x, up.y, up.z);
		// Use the transformation
		/*glRotatef(this->m_pTrack->trainCamRotate.y, 0, 1, 0);
		glRotatef(-this->m_pTrack->trainCamRotate.x, 1, 0, 0);
		
		glRotatef(this->m_pTrack->trainCamRotate.z, 0, 0, 1);
		glTranslatef(-this->m_pTrack->trainCam.x, -m_pTrack->trainCam.y, -m_pTrack->trainCam.z);*/
		
		update();
	}
}

//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows, bool doingReflect, bool doingRefract)
{
#pragma region RR_CAMERA
	if (doingReflect && camera == 0) {
		QVector3D mirrorStart = Quat2Euler(arcball.start);
		mirrorStart = QVector3D(-mirrorStart.x(), mirrorStart.y(), -mirrorStart.z());
		arcball.start = Euler2Quat(mirrorStart);
		QVector3D mirrorNow = Quat2Euler(arcball.now);
		mirrorNow = QVector3D(-mirrorNow.x(), mirrorNow.y(), -mirrorNow.z());
		arcball.now = Euler2Quat(mirrorNow);
		arcball.setProjection();
		
		glBindFramebuffer(GL_FRAMEBUFFER, water->FramebufferName);
		GLuint clearColor[4] = { 0, 0, 0, 0 };
		glClearBufferuiv(GL_COLOR, 0, clearColor);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	}
	else if (doingReflect && camera == 2) {
		/*glPushMatrix();
		glTranslatef(0, m_pTrack->trainCam.y * 2, 0);
		glRotatef(this->m_pTrack->trainCamRotate.x * 2, 1, 0, 0);
		glRotatef(-this->m_pTrack->trainCamRotate.z * 2, 0, 0, 1);*/
		glLoadIdentity();
		Pnt3f camPos, centerPos, up;
		camPos = m_pTrack->trainLookAt.camPosition;
		camPos.y *= -1;
		centerPos = camPos + Pnt3f(m_pTrack->trainLookAt.camDirection.x, -m_pTrack->trainLookAt.camDirection.y, m_pTrack->trainLookAt.camDirection.z);
		up = m_pTrack->trainLookAt.camUp;
		up.x *= -1;
		//up.y *= -1;
		up.z *= -1;

		gluLookAt(camPos.x, camPos.y, camPos.z, centerPos.x, centerPos.y, centerPos.z, up.x, up.y, up.z);

		glBindFramebuffer(GL_FRAMEBUFFER, water->FramebufferName);
		GLuint clearColor[4] = { 0, 0, 0, 0 };
		glClearBufferuiv(GL_COLOR, 0, clearColor);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	}

	if (doingRefract && camera == 0) {
		water->refractAngle = GetRefractAngle();
		camPos = getCamPos();
		float angle = -water->refractAngle * 180.0 / M_PI;
		glRotatef(angle, -camPos.z(), 0, -camPos.x());
		
		/*water->refractStart = Quat2Euler(arcball.start);
		water->refractNow = Quat2Euler(arcball.now);
		QVector3D refract = Quat2Euler(arcball.now * arcball.start);
		refract = GetRefract(refract);
		arcball.start = Euler2Quat(refract);
		arcball.start.renorm();
		arcball.now = Quat();
		arcball.setProjection();*/

		/*QVector3D refractStart = water->refractStart = Quat2Euler(arcball.start);
		refractStart = QVector3D(refractStart.x() * rate, refractStart.y(), refractStart.z() * rate);
		arcball.start = Euler2Quat(refractStart);
		QVector3D refractNow = water->refractNow = Quat2Euler(arcball.now);
		refractNow = QVector3D(refractNow.x() * rate, refractNow.y(), refractNow.z() * rate);
		arcball.now = Euler2Quat(refractNow);
		arcball.setProjection();*/

		glBindFramebuffer(GL_FRAMEBUFFER, water->refractFBO);
		GLuint clearColor[4] = { 0, 0, 0, 0 };
		glClearBufferuiv(GL_COLOR, 0, clearColor);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	}
	else if (doingRefract && camera == 2) {

		//glTranslatef(0, -m_pTrack->trainCam.y * 1.3, 0);
		glLoadIdentity();
		Pnt3f cam, centerPos, up;
		cam = Pnt3f(0, 0, 0);
		centerPos = cam + Pnt3f(m_pTrack->trainLookAt.camDirection.x, m_pTrack->trainLookAt.camDirection.y, m_pTrack->trainLookAt.camDirection.z);
		up = m_pTrack->trainLookAt.camUp;

		gluLookAt(cam.x, cam.y, cam.z, centerPos.x, centerPos.y, centerPos.z, up.x, up.y, up.z);

		water->refractAngle = GetTrainRefractAngle();
		camPos = QVector3D(m_pTrack->trainLookAt.camPosition.x, m_pTrack->trainLookAt.camPosition.y, m_pTrack->trainLookAt.camPosition.z);
		float angle = -water->refractAngle * 180.0 / M_PI;
		glRotatef(angle, m_pTrack->trainLookAt.camDirection.z, 0, -m_pTrack->trainLookAt.camDirection.x);

		glTranslatef(-camPos.x(), -camPos.y(), -camPos.z());

		glBindFramebuffer(GL_FRAMEBUFFER, water->refractFBO);
		GLuint clearColor[4] = { 0, 0, 0, 0 };
		glClearBufferuiv(GL_COLOR, 0, clearColor);
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	}
#pragma endregion

	if (doingReflect) {
		// draw the train
		//####################################################################
		// TODO: 
		//	call your own train drawing code
		//####################################################################
		if (this->camera != 2) {
			this->m_pTrack->drawTrain(doingShadows);
			if (this->m_pTrack->trainU > 1) {
				this->m_pTrack->trainU -= 1;
			}
			update();
		}
		else {
			this->m_pTrack->calTrainCam();
			this->m_pTrack->calTrainCam2();
			if (this->m_pTrack->trainU > 1) {
				this->m_pTrack->trainU -= 1;
			}
			update();
		}
		// draw the track
		//####################################################################
		// TODO: 
		// call your own track drawing code
		//####################################################################
		switch (this->track) {
		case 0:
			this->m_pTrack->drawLine(doingShadows);
			break;
		case 1:
			this->m_pTrack->drawTrack(doingShadows);
			break;
		case 2:
			this->m_pTrack->drawRoad(doingShadows);
			break;
		}
		if (drawTunnel)
			m_pTrack->drawTunnel();

		update();

		// Draw the control points
		// don't draw the control points if you're driving 
		// (otherwise you get sea-sick as you drive through them)
		
		for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (this->camera != 2) {
				if (!doingShadows) {
					if (((int)i) != selectedCube)
						glColor3ub(240, 60, 60);
					else
						glColor3ub(240, 240, 30);
				}
				this->m_pTrack->points[i].draw();
			}
			glColor3ub(60, 60, 60);
			this->m_pTrack->points[i].drawSupport(true);
		}
		update();
		
		eagle->Begin();
		eagle->Paint(ProjectionMatrex, ModelViewMatrex);
		eagle->End();
	}
	else if(!doingRefract) {
		// Draw the control points
		// don't draw the control points if you're driving 
		// (otherwise you get sea-sick as you drive through them)
		for (size_t i = 0; i < this->m_pTrack->points.size(); ++i) {
			if (this->camera != 2) {
				if (!doingShadows) {
					if (((int)i) != selectedCube)
						glColor3ub(240, 60, 60);
					else
						glColor3ub(240, 240, 30);
				}
				this->m_pTrack->points[i].draw();
			}
			glColor3ub(60, 60, 60);
			this->m_pTrack->points[i].drawSupport();
		}
		update();

		// draw the track
		//####################################################################
		// TODO: 
		// call your own track drawing code
		//####################################################################
		switch (this->track) {
		case 0:
			this->m_pTrack->drawLine(doingShadows);
			break;
		case 1:
			this->m_pTrack->drawTrack(doingShadows);
			break;
		case 2:
			this->m_pTrack->drawRoad(doingShadows);
			break;
		}
		if (drawTunnel)
			m_pTrack->drawTunnel();

		update();

#ifdef EXAMPLE_SOLUTION
		drawTrack(this, doingShadows);
#endif

		// draw the train
		//####################################################################
		// TODO: 
		//	call your own train drawing code
		//####################################################################
		if (this->camera != 2) {
			this->m_pTrack->drawTrain(doingShadows);
			if (this->m_pTrack->trainU > 1) {
				this->m_pTrack->trainU -= 1;
			}
			update();
		}
		else {
			//this->m_pTrack->calTrainCam();
			this->m_pTrack->calTrainCam2();
			if (this->m_pTrack->trainU > 1) {
				this->m_pTrack->trainU -= 1;
			}
			update();
		}
	}

	if (doingReflect || doingRefract) {
		//*********************************************************************
		// now draw the terrain
		//*********************************************************************
		terrain->Begin();
		glActiveTexture(GL_TEXTURE0);//選擇要傳的texture unit
		switch (terrain->terrainType) {
		case 0:
			Textures[0]->bind();//把實際上的tex綁到unit上
			break;
		case 1:
			Textures[1]->bind();//把實際上的tex綁到unit上
			break;
		case 2:
			Textures[2]->bind();//把實際上的tex綁到unit上
			break;
		case 3:
			Textures[6]->bind();//把實際上的tex綁到unit上
			break;
		default:
			Textures[0]->bind();//把實際上的tex綁到unit上
			break;
		}
		terrain->shaderProgram->setUniformValue("Texture", 0);//把編號為0(GL_TEXTURE0)的unit傳進shader
		glActiveTexture(GL_TEXTURE1);
		Textures[5]->bind();
		terrain->shaderProgram->setUniformValue("heightmap", 1);
		if(doingReflect && (camera == 0 || camera == 2))
			terrain->shaderProgram->setUniformValue("clipType", 1);
		else if (doingRefract && camera == 0)
			terrain->shaderProgram->setUniformValue("clipType", 0);
		else
			terrain->shaderProgram->setUniformValue("clipType", 0);
		terrain->Paint(ProjectionMatrex, ModelViewMatrex);
		terrain->End();
	}

	if (!doingRefract) {
		/*draw the eagle*/
		eagle->Begin();
		eagle->Paint(ProjectionMatrex, ModelViewMatrex);
		eagle->End();

		car->Begin();
		car->Paint(ProjectionMatrex, ModelViewMatrex);
		car->End();

		ball->Begin();
		ball->shaderProgram->setUniformValue("eye_position", getCamPos());
		ball->Paint(ProjectionMatrex, ModelViewMatrex);
		ball->End();
	}

	if (!doingReflect && !doingRefract) {
		if (terrain->terrainType == 0) {
			//*********************************************************************
			// now draw the water
			//*********************************************************************
			water->Begin();

			//use render texture
			//Active Texture
			glActiveTexture(GL_TEXTURE0);
			//Bind square's texture
			glBindTexture(GL_TEXTURE_2D, water->reflectTexture);
			//Textures[1]->bind();
			//pass texture to shader
			water->shaderProgram->setUniformValue("reflectTexture", 0);
			glActiveTexture(GL_TEXTURE1);
			//Bind square's texture
			if(camera == 0)
				glBindTexture(GL_TEXTURE_2D, water->refractTexture);
			else if(camera == 2)
				glBindTexture(GL_TEXTURE_2D, water->refractTexture);
			//pass texture to shader
			water->shaderProgram->setUniformValue("refractTexture", 1);

			water->shaderProgram->setUniformValue("camera", camera);

			//skybox reflect
			water->camPos = camPos = getCamPos();
			water->CalProjectorMatrix(static_cast<float>(width()) / static_cast<float>(height()));
			water->shaderProgram->setUniformValue("ProjectorMatrix", water->projMat);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_textureObj);
			water->shaderProgram->setUniformValue("cubemap", 2);
			glActiveTexture(GL_TEXTURE3);
			Textures[4]->bind();
			water->shaderProgram->setUniformValue("heightmap", 3);
			water->shaderProgram->setUniformValue("WorldCameraPosition", camPos);
			water->Paint(ProjectionMatrex, ModelViewMatrex);
			water->End();
		}

		//*********************************************************************
		// now draw the skybox
		//*********************************************************************
		skybox->Begin();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->m_textureObj);
		skybox->shaderProgram->setUniformValue("cubemap", 1);
		skybox->Paint(ProjectionMatrex, ModelViewMatrex);
		skybox->End();
	}

#pragma region REVERSE
	if (doingReflect && camera == 0) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		QVector3D mirrorStart = Quat2Euler(arcball.start);
		mirrorStart = QVector3D(-mirrorStart.x(), mirrorStart.y(), -mirrorStart.z());
		arcball.start = Euler2Quat(mirrorStart);
		QVector3D mirrorNow = Quat2Euler(arcball.now);
		mirrorNow = QVector3D(-mirrorNow.x(), mirrorNow.y(), -mirrorNow.z());
		arcball.now = Euler2Quat(mirrorNow);
		arcball.setProjection();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	}
	else if (doingReflect && camera == 2) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glLoadIdentity();
		Pnt3f camPos, centerPos, up;
		camPos = m_pTrack->trainLookAt.camPosition;
		centerPos = m_pTrack->trainLookAt.camPosition + m_pTrack->trainLookAt.camDirection;
		up = m_pTrack->trainLookAt.camUp;

		gluLookAt(camPos.x, camPos.y, camPos.z, centerPos.x, centerPos.y, centerPos.z, up.x, up.y, up.z);
		//glPopMatrix();
		glGetFloatv(GL_MODELVIEW_MATRIX, ModelViewMatrex);
		glGetFloatv(GL_PROJECTION_MATRIX, ProjectionMatrex);
	}

	if (doingRefract && camera == 0) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*float rate = water->refractRate;
		QVector3D refractStart = water->refractStart = Quat2Euler(arcball.start);
		refractStart = QVector3D(refractStart.x() / rate, refractStart.y(), refractStart.z() / rate);
		arcball.start = Euler2Quat(refractStart);
		QVector3D refractNow = water->refractNow = Quat2Euler(arcball.now);
		refractNow = QVector3D(refractNow.x() / rate, refractNow.y(), refractNow.z() / rate);
		arcball.now = Euler2Quat(refractNow);
		arcball.setProjection();*/
		/*arcball.start = Euler2Quat(water->refractStart);
		arcball.now = Euler2Quat(water->refractNow);*/
		glRotatef(water->refractAngle * 180.0 / M_PI, -camPos.z(), 0, camPos.x());
		arcball.setProjection();
	}
	else if (doingRefract && camera == 2) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glLoadIdentity();
		Pnt3f camPos, centerPos, up;
		camPos = m_pTrack->trainLookAt.camPosition;
		centerPos = m_pTrack->trainLookAt.camPosition + m_pTrack->trainLookAt.camDirection;
		up = m_pTrack->trainLookAt.camUp;

		gluLookAt(camPos.x, camPos.y, camPos.z, centerPos.x, centerPos.y, centerPos.z, up.x, up.y, up.z);
		//arcball.setProjection();
		//glTranslatef(0, m_pTrack->trainCam.y * 1.3, 0);
	}
#pragma endregion
#ifdef EXAMPLE_SOLUTION
	// don't draw the train if you're looking out the front window
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
#endif
}

void TrainView::
	doPick(int mx, int my)
	//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	makeCurrent();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();

	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
		5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);


	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;
}
