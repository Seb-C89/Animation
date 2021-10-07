#include <GL/gl.h>

void Draw_axes(float x, float y, float z)
{
	glPushMatrix();
		glScaled(x, y, z);
		glBegin(GL_LINES);
			glColor3ub(255, 0, 0); /* Axe X */
			glVertex3f(1, 0, 0);
			glVertex3f(-1, 0, 0);

			glColor3ub(0, 255, 0); /* Axe Y */
			glVertex3f(0, 1, 0);
			glVertex3f(0, -1, 0);

			glColor3ub(0, 0, 255); /* Axe Z */
			glVertex3f(0, 0, 1);
			glVertex3f(0, 0, -1);

			glColor3ub(255, 255, 255);
		glEnd();
	glPopMatrix();
}

void Draw_cube(float x, float y, float z)
{
	glPushMatrix();
		glScaled(x, y, z);
		glBegin(GL_QUADS);
			glColor3ub(255,0,0); /* face red */
			glVertex3d(1,1,1);
			glVertex3d(1,1,-1);
			glVertex3d(-1,1,-1);
			glVertex3d(-1,1,1);

			glColor3ub(0,255,0); /* face green */
			glVertex3d(1,-1,1);
			glVertex3d(1,-1,-1);
			glVertex3d(1,1,-1);
			glVertex3d(1,1,1);

			glColor3ub(0,0,255); /* face blue */
			glVertex3d(-1,-1,1);
			glVertex3d(-1,-1,-1);
			glVertex3d(1,-1,-1);
			glVertex3d(1,-1,1);

			glColor3ub(255,255,0); /* face yellow */
			glVertex3d(-1,1,1);
			glVertex3d(-1,1,-1);
			glVertex3d(-1,-1,-1);
			glVertex3d(-1,-1,1);

			glColor3ub(0,255,255); /* face cyan */
			glVertex3d(1,1,-1);
			glVertex3d(1,-1,-1);
			glVertex3d(-1,-1,-1);
			glVertex3d(-1,1,-1);

			glColor3ub(255,0,255); /* face magenta */
			glVertex3d(1,-1,1);
			glVertex3d(1,1,1);
			glVertex3d(-1,1,1);
			glVertex3d(-1,-1,1);

			glColor3ub(255, 255, 255);
		glEnd();
	glPopMatrix();
}
