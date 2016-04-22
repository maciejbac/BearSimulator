#include "stdafx.h"
#include <glew\glew.h>
#include <freeglut.h>
#include <CoreStructures.h>
#include "texture_loader.h"
#include "shader_setup.h"
#include "CGModel\CGModel.h"
#include "Importers\CGImporters.h"

using namespace std;
using namespace CoreStructures;

#define FIXED_FUNCTION true


bool lightToggled = false;

float theta = 0.0f;
GUClock cgClock;
int mouse_x, mouse_y;
bool mDown = false;

GLuint gDiffuse; 
GLuint gAmbient; 
GLuint gAttenuation; 
GLuint gLPosition; 

GLuint gEyePos; 

GLuint gMatAmbient;
GLuint gMatDiffuse;
GLuint gMatSpecular;

GLuint gModelMatrix;
GLuint gViewMatrix;
GLuint gProjectionMatrix;
GLuint gNormalMatrix;


GLuint myShaderProgram;

GLuint bearTexture = 0;
GLuint pyramidTexture = 0;
GLuint groundTexture = 0;
GLuint fireTexture = 0;
GLuint wallTexture = 0;

CGModel *bearModel[1];
CGModel *objectModel[1];
CGModel *groundModel[1];
CGModel *fireModel[1];
CGModel *wallModel[1];

GUVector4 bearModelPos = GUVector4(0, 0, 0, 1);
GUVector4 objectModelPos = GUVector4(0, 0, 0, 1);
GUVector4 groundModelPos = GUVector4(0, 0, 0, 1);
GUVector4 fireModelPos = GUVector4(0, 0, 0, 1);
GUVector4 wallModelPos = GUVector4(0, 0, 0, 1);

GUMatrix4 bearModelMatrix = GUMatrix4::translationMatrix(bearModelPos);
GUMatrix4 objectModelMatrix = GUMatrix4::translationMatrix(objectModelPos);
GUMatrix4 groundModelMatrix = GUMatrix4::translationMatrix(groundModelPos);
GUMatrix4 fireModelMatrix = GUMatrix4::translationMatrix(fireModelPos);
GUMatrix4 wallModelMatrix = GUMatrix4::translationMatrix(wallModelPos);

GUPivotCamera *mainCamera = NULL;

GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 }; 
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 }; 
GLfloat light_specular[] = { 10000.0, 10000.0, 10000.0, 1.0 }; 
GLfloat light_position[] = { 50.0, 150.0, 70.0, 1.0 }; 

GLfloat light_ambient2[] = { 0.25, 0.2, 0.1, 1.0 }; 
GLfloat light_diffuse2[] = { 8.0, 0.4, 0.2, 1.0 }; 
GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 }; 
GLfloat light_position2[] = { 1.0, 8.0, 20.0, 1.0 }; 

GLfloat mat_amb_diff[] = { 1.2, 1.2, 1.2, 1.0 }; 
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 15.0 };				 
GLfloat mat_emission[] = { 0.0, 0.0, 0.0, 1.0 }; 

GLfloat ground_plane[] = { 0.0, 1.0, 0.0, 0.0 }; 

void init(int argc, char* argv[]);
void report_version(void);
void update(void);
void display(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);
void mouseWheelUpdate(int wheel_number, int direction, int x, int y);


int _tmain(int argc, char* argv[]) {

	init(argc, argv);

	glutMainLoop();

	shutdownCOM();

	return 0;
}


void init(int argc, char* argv[]) {

	initCOM();

	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(64, 64);
	glutCreateWindow("GUPivotCamera Demo");

	glutDisplayFunc(display);
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseButtonDown);
	glutMotionFunc(mouseMove);
	glutMouseWheelFunc(mouseWheelUpdate);
	GLenum err = glewInit();

	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";

	} else {

		cout << "GLEW could not be initialised\n";
		throw;
	}
	
	report_version();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	mainCamera = new GUPivotCamera(-30.0f, 1.0f, 70.0f, 50.0f, 1.0f, 1.0f);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glLineWidth(4.0f);

	myShaderProgram = setupShaders(string("Shaders\\diffuse_vertex_shader.txt"), string("Shaders\\diffuse_fragment_shader.txt"));

	gLPosition = glGetUniformLocation(myShaderProgram, "lposition");
	gAmbient = glGetUniformLocation(myShaderProgram, "lambient");
	gDiffuse = glGetUniformLocation(myShaderProgram, "ldiffuse");
	gAttenuation = glGetUniformLocation(myShaderProgram, "lattenuation");
	gEyePos = glGetUniformLocation(myShaderProgram, "eyePos");

	gMatAmbient = glGetUniformLocation(myShaderProgram, "matAmbient");
	gMatDiffuse = glGetUniformLocation(myShaderProgram, "matDiffuse");
	gMatSpecular = glGetUniformLocation(myShaderProgram, "matSpecular");

	gModelMatrix = glGetUniformLocation(myShaderProgram, "g_ModelMatrix");
	gViewMatrix = glGetUniformLocation(myShaderProgram, "g_ViewMatrix");
	gProjectionMatrix = glGetUniformLocation(myShaderProgram, "g_ProjectionMatrix");
	gNormalMatrix = glGetUniformLocation(myShaderProgram, "g_NormalMatrix");


	bearTexture = fiLoadTexture("Resources\\Textures\\bear.tga");
	pyramidTexture = fiLoadTexture("Resources\\Textures\\fire.png");
	groundTexture = fiLoadTexture("Resources\\Textures\\ground.jpg");
	fireTexture = fiLoadTexture("Resources\\Textures\\fire2.jpg");
	wallTexture = fiLoadTexture("Resources\\Textures\\wall.jpg");

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	bearModel[0] = new CGModel();
	objectModel[0] = new CGModel();
	groundModel[0] = new CGModel();
	fireModel[0] = new CGModel();
	wallModel[0] = new CGModel();

	import3DS(L"Resources\\Models\\pole.3ds", fireModel[0]);
	import3DS(L"Resources\\Models\\bear.3ds", bearModel[0]);
	import3DS(L"Resources\\Models\\fire.3ds", objectModel[0]);
	import3DS(L"Resources\\Models\\ground.3ds", groundModel[0]);
	import3DS(L"Resources\\Models\\wall.3ds", wallModel[0]);

	bearModel[0]->setTextureForModel(bearTexture);
	objectModel[0]->setTextureForModel(pyramidTexture);
	groundModel[0]->setTextureForModel(groundTexture);
	fireModel[0]->setTextureForModel(fireTexture);
	wallModel[0]->setTextureForModel(wallTexture);

	cgClock.start();


}


void report_version(void) {

	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";
}

void update(void) {

	if (!lightToggled)
	{
		cgClock.tick();
		theta += cgClock.gameTimeDelta();

		float tR = 12.55;

		bearModelMatrix = GUMatrix4::translationMatrix(bearModelPos) *GUMatrix4::rotationMatrix(0.0f, (theta*0.5), 0.0f);

		cout << theta << endl;

		if (theta > tR)
			theta = 0;

		float kappa = tR - theta;

		if (theta < 6.27)
		{
			glClearColor(theta*0.01, 0.0f, theta*0.1, 0.0f);
		}
		else
		{
			glClearColor(kappa*0.01, 0.0f, kappa*0.1, 0.0f);
		}
	}
}

GUMatrix4 buildShadowMatrix(GLfloat groundplane[4], GLfloat lightpos[4])
{

	GLfloat dot;
	GLfloat shadowMat[4][4];
	int X = 0, Y = 1, Z = 2, W = 3;
	dot = groundplane[X] * lightpos[X] + groundplane[Y] * lightpos[Y] + groundplane[Z] * lightpos[Z] + groundplane[W] * lightpos[W];
	shadowMat[0][0] = dot - lightpos[X] * groundplane[X];
	shadowMat[1][0] = 0.f - lightpos[X] * groundplane[Y];
	shadowMat[2][0] = 0.f - lightpos[X] * groundplane[Z];
	shadowMat[3][0] = 0.f - lightpos[X] * groundplane[W];
	shadowMat[X][1] = 0.f - lightpos[Y] * groundplane[X];
	shadowMat[1][1] = dot - lightpos[Y] * groundplane[Y];
	shadowMat[2][1] = 0.f - lightpos[Y] * groundplane[Z];
	shadowMat[3][1] = 0.f - lightpos[Y] * groundplane[W];
	shadowMat[X][2] = 0.f - lightpos[Z] * groundplane[X];
	shadowMat[1][2] = 0.f - lightpos[Z] * groundplane[Y];
	shadowMat[2][2] = dot - lightpos[Z] * groundplane[Z];
	shadowMat[3][2] = 0.f - lightpos[Z] * groundplane[W];
	shadowMat[X][3] = 0.f - lightpos[W] * groundplane[X];
	shadowMat[1][3] = 0.f - lightpos[W] * groundplane[Y];
	shadowMat[2][3] = 0.f - lightpos[W] * groundplane[Z];
	shadowMat[3][3] = dot - lightpos[W] * groundplane[W];
	GUVector4 temp[4];
	for (int i = 0; i < 4; i++)
		temp[i] = GUVector4(shadowMat[i][0], shadowMat[i][1], shadowMat[i][2], shadowMat[i][3]);


	GUMatrix4 shadowMatrix = GUMatrix4(temp[0], temp[1], temp[2], temp[3]);

	return shadowMatrix;
}

void display(void) {

	update();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GUMatrix4 viewMatrix = mainCamera->viewTransform();
	GUMatrix4 projMatrix = mainCamera->projectionTransform();
	GUMatrix4 shadowMatrix = buildShadowMatrix(ground_plane, light_position);
	GUMatrix4 modelViewMatrix = viewMatrix * bearModelMatrix;

	if (FIXED_FUNCTION)
	{
		glUseProgram(0);

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((GLfloat*)&projMatrix);
		glMatrixMode(GL_MODELVIEW);
		GUMatrix4 modelViewMatrix = viewMatrix * bearModelMatrix;
		glLoadMatrixf((GLfloat*)&modelViewMatrix);

		glEnable(GL_TEXTURE_2D);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);

		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);

		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);




	}
	else {
		GUVector4 eyePos = mainCamera->cameraLocation();

		GUMatrix4 normalMatrix = bearModelMatrix.inverseTranspose();

		glUseProgram(myShaderProgram);
		glUniform4fv(gDiffuse, 1, (GLfloat*)&light_diffuse);
		glUniform4fv(gAmbient, 1, (GLfloat*)&light_ambient);
		glUniform4fv(gEyePos, 1, (GLfloat*)&eyePos);
		GLfloat	attenuation[] = { 1.0, 0.5, 0.05 };

		glUniform3fv(gAttenuation, 1, (GLfloat*)&attenuation);
		glUniformMatrix4fv(gModelMatrix, 1, false, (GLfloat*)&bearModelMatrix);
		glUniformMatrix4fv(gViewMatrix, 1, false, (GLfloat*)&viewMatrix);
		glUniformMatrix4fv(gProjectionMatrix, 1, false, (GLfloat*)&projMatrix);
		glUniformMatrix4fv(gNormalMatrix, 1, false, (GLfloat*)&normalMatrix);

		glUniform4fv(gMatAmbient, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(gMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
		GLfloat mat_specular_shader[] = { mat_specular[0], mat_specular[1], mat_specular[2], mat_shininess[0] };
		glUniform4fv(gMatSpecular, 1, (GLfloat*)&mat_specular_shader);

	}

	if (bearModel)
		bearModel[0]->renderTexturedModel();

	if (objectModel)
		objectModel[0]->renderTexturedModel();

	if (groundModel)
		groundModel[0]->renderTexturedModel();

	if (fireModel)
		fireModel[0]->renderTexturedModel();

	if (wallModel)
		wallModel[0]->renderTexturedModel();


	if (FIXED_FUNCTION)
	{
		glDisable(GL_LIGHTING);
		modelViewMatrix = viewMatrix*shadowMatrix*bearModelMatrix * objectModelMatrix;
		glLoadMatrixf((GLfloat*)&modelViewMatrix);
		glColor4ub(0, 0, 0, 100);
	}
	if (bearModel)
		bearModel[0]->render();

	if (objectModel)
		objectModel[0]->render();

	if (wallModel)
		wallModel[0]->render();

	glutSwapBuffers();

	glutPostRedisplay();
}

#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}


void mouseMove(int x, int y) {

	if (mDown) {

		int dx = x - mouse_x;
		int dy = y - mouse_y;

		theta += (float)dy * (3.142f * 0.01f);

		if (mainCamera)
			mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);

		mouse_x = x;
		mouse_y = y;

		glutPostRedisplay();
	}
}


void mouseWheelUpdate(int wheel_number, int direction, int x, int y) {

	if (mainCamera) {

		if (direction<0)
			mainCamera->scaleCameraRadius(1.1f);
		else if (direction>0)
			mainCamera->scaleCameraRadius(0.9f);

		glutPostRedisplay();
	}
}


void keyDown(unsigned char key, int x, int y) {

	if (key == 'r') {

		theta = 0.0f;
		glutPostRedisplay();
	}

	if (key == 'x') {

		
		if (!lightToggled)
		{
			cgClock.stop();
			lightToggled = true;
		}
		else
		{
			cgClock.start();
			lightToggled = false;
		}

		glutPostRedisplay();
	}

}

#pragma endregion
