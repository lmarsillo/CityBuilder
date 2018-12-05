#include <stdio.h>
#include <gl/glut.h>
#include "CubeMesh.h"
#include "Building.h"

#define FLOORHEIGHT 0.5

static GLfloat quadNormals[][3] = { { 0.0, 0.0, -1.0},	// Back Face 
						  { 0.0, 1.0,  0.0},	// Top Face
						  {-1.0, 0.0,  0.0},	// Left Face
						  { 1.0, 0.0,  0.0},	// Right Face
						  { 0.0, 0.0,  1.0},	// Front Face
						  { 0.0,-1.0,  0.0} };	// Bottom Face

Building newBuilding(CubeMesh *cube)
{
	Building building;

	building.floors = (cube->sfy / FLOORHEIGHT) + 1;
	
	building.p = (float*)calloc(building.floors * 4 * 3, sizeof(float));
	building.vertices = (float**)calloc(building.floors * 4, sizeof(float*));
	for (int i = 0; i < building.floors * 4; i++)
		building.vertices[i] = building.p + i * 3;

	building.selected = 1;

	//floor vertices
	for (int i = 0; i < building.floors; i++) {
		//top left
		building.vertices[0 + (4 * i)][0] = cube->tx;
		building.vertices[0 + (4 * i)][1] = cube->ty + (i * FLOORHEIGHT) * 2;
		building.vertices[0 + (4 * i)][2] = cube->tz;

		//top right
		building.vertices[1 + (4 * i)][0] = cube->tx + cube->sfx * 2;
		building.vertices[1 + (4 * i)][1] = cube->ty + (i * FLOORHEIGHT) * 2;
		building.vertices[1 + (4 * i)][2] = cube->tz;

		//bottom left
		building.vertices[2 + (4 * i)][0] = cube->tx;
		building.vertices[2 + (4 * i)][1] = cube->ty + (i * FLOORHEIGHT) * 2;
		building.vertices[2 + (4 * i)][2] = cube->tz + cube->sfz * 2;

		//bottom right
		building.vertices[3 + (4 * i)][0] = cube->tx + cube->sfx * 2;
		building.vertices[3 + (4 * i)][1] = cube->ty + (i * FLOORHEIGHT) * 2;
		building.vertices[3 + (4 * i)][2] = cube->tz + cube->sfz * 2;
		
	}

	building.mat_ambient[0] = 0.0;
	building.mat_ambient[1] = 0.05f;
	building.mat_ambient[2] = 0.0;
	building.mat_ambient[3] = 1.0;
	building.mat_specular[0] = 0.0;
	building.mat_specular[1] = 0.0;
	building.mat_specular[2] = 0.004f;
	building.mat_specular[3] = 1.0;
	building.mat_diffuse[0] = 0.5;
	building.mat_diffuse[1] = 0.5;
	building.mat_diffuse[2] = 0.5;
	building.mat_diffuse[3] = 1.0;
	building.mat_shininess[0] = 0;

	building.highlightMat_ambient[0] = 0.4F;
	building.highlightMat_ambient[1] = 0.2F;
	building.highlightMat_ambient[2] = 0.0F;
	building.highlightMat_ambient[3] = 1.0F;
	building.highlightMat_specular[0] = 0.1F;
	building.highlightMat_specular[1] = 0.1F;
	building.highlightMat_specular[2] = 0.0F;
	building.highlightMat_specular[3] = 1.0F;
	building.highlightMat_diffuse[0] = 0.9F;
	building.highlightMat_diffuse[1] = 0.5F;
	building.highlightMat_diffuse[2] = 0.0F;
	building.highlightMat_diffuse[3] = 1.0F;
	building.highlightMat_shininess[0] = 0.0F;

	return building;
}



void drawBuilding(Building *building)
{
	if (building->selected){
		// Setup the material and lights used for selected building
	glMaterialfv(GL_FRONT, GL_AMBIENT, building->highlightMat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, building->highlightMat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, building->highlightMat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, building->highlightMat_shininess);
	}
	else
	{
		// Setup the material and lights used for the building
		glMaterialfv(GL_FRONT, GL_AMBIENT, building->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, building->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, building->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, building->mat_shininess);
	}

	glPushMatrix();

	glBegin(GL_QUADS);
	for (int i = 0; i < building->floors; i++) {
		glNormal3f(quadNormals[1][0], quadNormals[1][1], quadNormals[1][2]);
		glVertex3f(building->vertices[1 + (4 * i)][0], building->vertices[1 + (4 * i)][1], building->vertices[1 + (4 * i)][2]);
		glVertex3f(building->vertices[0 + (4 * i)][0], building->vertices[0 + (4 * i)][1], building->vertices[0 + (4 * i)][2]);
		glVertex3f(building->vertices[2 + (4 * i)][0], building->vertices[2 + (4 * i)][1], building->vertices[2 + (4 * i)][2]);
		glVertex3f(building->vertices[3 + (4 * i)][0], building->vertices[3 + (4 * i)][1], building->vertices[3 + (4 * i)][2]);
		
	}

	for (int i = 0; i < building->floors - 1; i++) {
		//back face
		glNormal3f(quadNormals[0][0], quadNormals[0][1], quadNormals[0][2]);
		glVertex3f(building->vertices[i * 4 + 4][0], building->vertices[i * 4 + 4][1], building->vertices[i * 4 + 4][2]);
		glVertex3f(building->vertices[i * 4 + 5][0], building->vertices[i * 4 + 5][1], building->vertices[i * 4 + 5][2]);
		glVertex3f(building->vertices[i * 4 + 1][0], building->vertices[i * 4 + 1][1], building->vertices[i * 4 + 1][2]);
		glVertex3f(building->vertices[i * 4][0], building->vertices[i * 4][1], building->vertices[i * 4][2]);
		
		//left face
		glNormal3f(quadNormals[2][0], quadNormals[2][1], quadNormals[2][2]);
		glVertex3f(building->vertices[i * 4 + 6][0], building->vertices[i * 4 + 6][1], building->vertices[i * 4 + 6][2]);
		glVertex3f(building->vertices[i * 4 + 4][0], building->vertices[i * 4 + 4][1], building->vertices[i * 4 + 4][2]);
		glVertex3f(building->vertices[i * 4][0], building->vertices[i * 4][1], building->vertices[i * 4][2]);
		glVertex3f(building->vertices[i * 4 + 2][0], building->vertices[i * 4 + 2][1], building->vertices[i * 4 + 2][2]);
		
		//front face
		glNormal3f(quadNormals[4][0], quadNormals[4][1], quadNormals[4][2]);
		glVertex3f(building->vertices[i * 4 + 7][0], building->vertices[i * 4 + 7][1], building->vertices[i * 4 + 7][2]);
		glVertex3f(building->vertices[i * 4 + 6][0], building->vertices[i * 4 + 6][1], building->vertices[i * 4 + 6][2]);
		glVertex3f(building->vertices[i * 4 + 2][0], building->vertices[i * 4 + 2][1], building->vertices[i * 4 + 2][2]);
		glVertex3f(building->vertices[i * 4 + 3][0], building->vertices[i * 4 + 3][1], building->vertices[i * 4 + 3][2]);
		
		//right face
		glNormal3f(quadNormals[3][0], quadNormals[3][1], quadNormals[3][2]);
		glVertex3f(building->vertices[i * 4 + 5][0], building->vertices[i * 4 + 5][1], building->vertices[i * 4 + 5][2]);
		glVertex3f(building->vertices[i * 4 + 7][0], building->vertices[i * 4 + 7][1], building->vertices[i * 4 + 7][2]);
		glVertex3f(building->vertices[i * 4 + 3][0], building->vertices[i * 4 + 3][1], building->vertices[i * 4 + 3][2]);
		glVertex3f(building->vertices[i * 4 + 1][0], building->vertices[i * 4 + 1][1], building->vertices[i * 4 + 1][2]);
	}
	
	glEnd();
	glPopMatrix();
}


void scaleFloor(Building *building, int floorToScale, int change) {

	float scale;

	if (change == 1)
		scale = 0.5;
	else
		scale = -0.5;

	//top left
	building->vertices[0 + (4 * floorToScale)][0] -= scale;
	building->vertices[0 + (4 * floorToScale)][2] -= scale;

	//top right
	building->vertices[1 + (4 * floorToScale)][0] += scale;
	building->vertices[1 + (4 * floorToScale)][2] -= scale;

	//bottom left
	building->vertices[2 + (4 * floorToScale)][0] -= scale;
	building->vertices[2 + (4 * floorToScale)][2] += scale;

	//bottom right
	building->vertices[3 + (4 * floorToScale)][0] += scale;
	building->vertices[3 + (4 * floorToScale)][2] += scale;
}



