/*******************************************************************
		   Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#include "CubeMesh.h"
#include "Building.h"

const int meshSize = 40;    // Default Mesh Size
const int vWidth = 800;     // Viewport width in pixels
const int vHeight = 600;    // Viewport height in pixels

static int currentButton;
static unsigned char currentKey;

// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

// Structure defining a bounding box, currently unused
//struct BoundingBox {
//    Vector3D min;
//    Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
Vector3D ScreenToWorld(int x, int y);

//mode variables
int translate = 0;
int draw = 0;
int scale = 0;
int height = 0;
int extrusion = 0;
int floorToScale = 0;
int change = 0;

Building buildings[20];
int index = 0;
float camerax = 1.0;
int prevX = 400;

CubeMesh cube;
Building skyscrap;

// drone values
Vector3D droneOrigin;
Vector3D front;
Vector3D forwardV;
float droneRad = 0.0;
float droneAngle = 0.0;
static GLfloat spin = 0.0;

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(500, 100);
	glutCreateWindow("Assignment 2");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);   // This light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	// Set up ground quad mesh
	Vector3D origin = NewVector3D(-20.0f, -0.01f, 20.0f);
	Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
	Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
	groundMesh = NewQuadMesh(meshSize);
	InitMeshQM(&groundMesh, meshSize, origin, 40.0, 40.0, dir1v, dir2v);

	Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
	Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
	Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
	SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

	// Set up the bounding box of the scene
	// Currently unused. You could set up bounding boxes for your objects eventually.
	//Set(&BBox.min, -8.0f, 0.0, -8.0);
	//Set(&BBox.max, 8.0f, 6.0,  8.0);

	droneOrigin = NewVector3D(0.0f, 4.0f, 0.0f);
	front = NewVector3D(-1.3f, 4.0f, 0.0f);
	Subtract(&front, &droneOrigin, &forwardV);
}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set drone material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);


	glPushMatrix();

	if (draw == 1) {
		glTranslatef(cube.tx + cube.sfx, cube.ty + cube.sfy, cube.tz + cube.sfz);
		glScalef(cube.sfx, cube.sfy, cube.sfz);
		drawCube(&cube);
	}
	glPopMatrix();

	glPushMatrix();
	for (int i = 0; i <= index; i++) {
		drawBuilding(&buildings[i]);
	}
	drawBuilding(&skyscrap);
	glPopMatrix();
	// Draw ground mesh
	DrawMeshQM(&groundMesh, meshSize);

	glutSwapBuffers();   // Double buffering, swap buffers
}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (GLdouble)w / h, 0.2, 60.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0, 15.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':
		if (extrusion == 1)
			break;
		translate = 1;
		scale = height = 0;
		break;
	case 'h':
		if (extrusion == 1)
			break;
		height = 1;
		scale = translate = 0;
		break;
	case 's':
		if (extrusion == 1)
			break;
		scale = 1;
		height = translate = 0;
		break;
	case 'e':
		extrusion = 1;
		draw = scale = height = translate = 0;
		skyscrap = newBuilding(&cube);
		break;
	case 'a':
		if (extrusion == 1) {
			skyscrap.selected = 0;
			buildings[index] = skyscrap;
			index++;
		}
		break;
	/*case 'p':
		FILE *f = fopen("citys.txt", "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}

		for (int a = 0; a < index; a++) {
			for (int i = 0; i < buildings[a].floors * 4; i++) {
				for (int j = 0; j < 3; j++) {
					fprintf(f, "%.3f, ", buildings[a].vertices[i][j]);
				}
				putchar('\n');
				putchar('\n');
			}
		}
		fclose(f);
		break;
		*/
	case 'p':
		for (int a = 0; a < index; a++) {
			for (int i = 0; i < buildings[a].floors * 4; i++) {
				for (int j = 0; j < 3; j++) {
					printf("%.3f, ", buildings[a].vertices[i][j]);
				}
				putchar('\n');
			}
			putchar('\n');
		}
	}
	glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		floorToScale = 0;
		extrusion = 0;
		draw = 1;
		cube = newCube();
	}

	switch (key)
	{
	case GLUT_KEY_DOWN:
		if (translate == 1)
			cube.tz += 1;
		else if (height == 1)
			cube.sfy -= 0.5;
		else if (scale == 1)
			cube.sfz += 0.5;
		else if (extrusion == 1) {
			if (floorToScale > 0)
				floorToScale -= 1;
			//printf("%d\n", floorToScale);
		}
		break;
	case GLUT_KEY_UP:
		if (translate == 1)
			cube.tz -= 1;
		else if (height == 1)
			cube.sfy += 0.5;
		else if (scale == 1)
			cube.sfz -= 0.5;
		else if (extrusion == 1) {
			if (floorToScale < skyscrap.floors - 1)
				floorToScale += 1;
			//printf("%d\n", floorToScale);
		}
		break;
	case GLUT_KEY_RIGHT:
		if (translate == 1)
			cube.tx += 1;
		else if (height == 1)
			;
		else if (scale == 1)
			cube.sfx += 0.5;
		else if (extrusion == 1) {
			change = 1;
			scaleFloor(&skyscrap, floorToScale, change);
		}
		break;
	case GLUT_KEY_LEFT:
		if (translate == 1)
			cube.tx -= 1;
		else if (height == 1)
			;
		else if (scale == 1)
			cube.sfx -= 0.5;
		else if (extrusion == 1) {
			change = 0;
			scaleFloor(&skyscrap, floorToScale, change);
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{

	if (currentButton == GLUT_LEFT_BUTTON)
	{
		if (prevX > xMouse) {
			if (camerax > 0)
				camerax = -camerax;
		}
		else if (prevX < xMouse) {
			if (camerax < 0)
				camerax = -camerax;
		}

		glRotatef(camerax, 0.0, 1.0, 0.0);
	}

	prevX = xMouse;

	glutPostRedisplay();   // Trigger a window redisplay
}


Vector3D ScreenToWorld(int x, int y)
{
	// you will need to finish this if you use the mouse
	return NewVector3D(0, 0, 0);
}



