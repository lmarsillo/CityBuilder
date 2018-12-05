
#include <stdbool.h>
#include "CubeMesh.h"

typedef struct Building
{
	int floors;
	float *p;
	float **vertices;

	int selected;

	// Material properties for drawing
	float mat_ambient[4];
	float mat_specular[4];
	float mat_diffuse[4];
	float mat_shininess[1];

	// Material properties if selected
	float highlightMat_ambient[4];
	float highlightMat_specular[4];
	float highlightMat_diffuse[4];
	float highlightMat_shininess[1];

} Building;

Building newBuilding(CubeMesh *cube);
