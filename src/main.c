#if (_MSC_VER) // If compiled with Microsoft Visual Studio
	#include <Windows.h> // Needed for OpenGL
#endif

#include <SDL2/sdl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>

#include "primitive.h"
#include "mesh.h"
#include "animation.h"

#define LARGEUR_ECRAN   800
#define HAUTEUR_ECRAN   600
#define FPS             25
#define FRAME_RATE      1000/FPS


int main(int argc, char* argv[])
{
	int continuer = 1;
	//unsigned int i, j;
	int current_time = SDL_GetTicks(), elapsed_time = 0, last_time = 0;
	SDL_Event event;

	/* SDL Initialization */
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_OPENGL, &sdlWindow, &sdlRenderer);
	SDL_GLContext glcontext = SDL_GL_CreateContext(sdlWindow);

	/* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	/* OpenGL Initialization (bis) */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, (double)LARGEUR_ECRAN / HAUTEUR_ECRAN, 1, 100);

	/* Load files */
	Mesh* mymesh = Load_Mesh("mesh\\mesh");
	Skeleton* myskeleton = Load_Skeleton("mesh\\skeleton");
	mymesh->skeleton = myskeleton;
	Animation_set* myanimation_set = Load_Animation_set("mesh\\anim");
	mymesh->skeleton->animation_set = myanimation_set;
	Make_unique_index(mymesh);
	/*for(i=0; i < mymesh->skeleton->animation_set->animations[0].count; i++)
	{
		for(j=0;  j < mymesh->skeleton->animation_set->animations[0].poses[i].count; j++)
		{
			printf("Pose[%u], keys[%u]: %f\n", i, j, mymesh->skeleton->animation_set->animations[0].poses[i].keys[j]);
		}
	}*/

	while (continuer)
	{
		current_time = SDL_GetTicks();
		elapsed_time = current_time - last_time;

		if (elapsed_time > FRAME_RATE)
		{
			SDL_PollEvent(&event);
			switch (event.type)
			{
			case SDL_QUIT:
				continuer = 0;
				break;
			case SDLK_k:
				break;
			default:
				break;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(20, 0, 0, 0, 0, 0, 0, 0, 1);
			/*glRotated(angleX,1,0,0);*/
			/*glRotated(angleZ,0,0,1);*/

		/* Draw */

			glDisable(GL_LIGHTING);
			Draw_axes(100, 100, 100);
			glEnable(GL_LIGHTING);
			/*printf("current_time: %i", current_time);*/
			Play_Animation(mymesh, 0, SDL_GetTicks());
			/*for(i=0; i < myskeleton->bones_size; i++)
			{
				printf("B %u\n", i);
				printf("t %f %f %f\n", myskeleton->bones[i].translation.x, myskeleton->bones[i].translation.y, myskeleton->bones[i].translation.z);
				printf("q %f %f %f %f\n", myskeleton->bones[i].quaternion.w, myskeleton->bones[i].quaternion.x, myskeleton->bones[i].quaternion.y, myskeleton->bones[i].quaternion.z);
				printf("s %f %f %f\n", myskeleton->bones[i].scale.x, myskeleton->bones[i].scale.y, myskeleton->bones[i].scale.z);
			}
			printf("----------\n");*/
			Draw_Skeleton(myskeleton);

			Draw_Mesh_buffer(mymesh);

			glFlush();
			SDL_GL_SwapWindow(sdlWindow);
			last_time = current_time;
		}
		else
			SDL_Delay(FRAME_RATE - (current_time - last_time));
	}

	Delete_Mesh(mymesh);
	Delete_Skeleton(myskeleton);
	Delete_Animation_set(myanimation_set);

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
	return EXIT_SUCCESS;
}
