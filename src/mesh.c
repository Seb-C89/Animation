#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

#include "../header/mesh.h"
#include "../header/quaternion.h"

Mesh* Load_Mesh(const char* path)
{
	FILE* file = NULL;
		file = fopen(path, "r");

	if(file != NULL)
	{
		Mesh* mesh = NULL;
		unsigned int v=0, n=0, t=0, i=0;

		char current_char;
		float fbuffer;
		unsigned int uibuffer;

		/* FIRST READ */
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'v':
					for(v = 0; fgetc(file) != '\n'; v++)
						fscanf(file, "%f", &fbuffer);
					break;
				case 'n':
					for(n = 0; fgetc(file) != '\n'; n++)
						fscanf(file, "%f", &fbuffer);
					break;
                case 't':
                    for(t = 0; fgetc(file) != '\n'; t++)
                        fscanf(file, "%f", &fbuffer);
                    break;
                case 'f':
					for(i = 0; fgetc(file) != '\n'; i++)
						fscanf(file, "%u", &uibuffer);
					break;
                case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		/* CHECK DATA */
		if(v%3==0 && n%3==0 && t%2==0 && i%3==0)
		{
			v /= 3;
			n /= 3;
			t /= 2;

			mesh = malloc(sizeof(Mesh));
				mesh->skeleton = NULL;

			mesh->vertices = malloc(sizeof(Vertex3f)*v);
			mesh->vertices_buffer = malloc(sizeof(Vertex3f)*v);
				mesh->vertices_size = v;
				printf("Vertices: %i\n", v);
			mesh->normals = malloc(sizeof(Vec3f)*n);
			mesh->normals_buffer = malloc(sizeof(Vec3f)*n);
				mesh->normals_size = n;
				printf("Normals: %i\n", n);
			mesh->texturesco = malloc(sizeof(Vertex2f)*t);
				mesh->texturesco_size = t;
				printf("Texco: %i\n", t);
			mesh->faces = malloc(sizeof(unsigned int)*i);
				mesh->faces_size = i;
				printf("Faces: %i\n", i);
		}
		else
		{
			fprintf(stderr, "Les données semblent corrompues:\n");
			fprintf(stderr, "Vertices - entries: %u for %f vertices\n", v, (float)v/3);
			fprintf(stderr, "Normals - entries: %u for %f normals\n", n, (float)n/3);
			fprintf(stderr, "Texco - entries: %u for %f texco\n", t, (float)t/2);
			fprintf(stderr, "Index - entries: %u for %f faces\n", i, (float)i/3);

			fclose(file);
			return NULL;
		}

		/* SECOND READ */
		rewind(file);
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'v':
					for(v = 0; fgetc(file) != '\n'; v++)
						fscanf(file, "%f %f %f", &mesh->vertices[v].x, &mesh->vertices[v].y, &mesh->vertices[v].z);
					break;
				case 'n':
					for(n = 0; fgetc(file) != '\n'; n++)
						fscanf(file, "%f %f %f", &mesh->normals[n].x, &mesh->normals[n].y, &mesh->normals[n].z);
					break;
				case 't':
					for(t = 0; fgetc(file) != '\n'; t++)
						fscanf(file, "%f %f", &mesh->texturesco[t].x, &mesh->texturesco[t].y);
					break;
				case 'f':
					for(i = 0; fgetc(file) != '\n'; i+=3)
						fscanf(file, "%u %u %u", &mesh->faces[i], &mesh->faces[i+1], &mesh->faces[i+2]);
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		fclose(file);
		return mesh;
	}
	else
	{
		fprintf(stderr, "Le fichier est introuvable: %s", path);
		return NULL;
	}
}

void Delete_Mesh(Mesh *mesh)
{
	free(mesh->vertices);
	free(mesh->vertices_buffer);
	free(mesh->normals);
	free(mesh->normals_buffer);
	free(mesh->texturesco);
	free(mesh->faces);
}

Skeleton* Load_Skeleton(const char* path)
{
	FILE* file = NULL;
		file = fopen(path, "r");

	if(file != NULL)
	{
		Skeleton *skeleton = NULL;
		unsigned int b=0, c=0, p=0, w=0, i=0;

		char current_char;
		unsigned int uibuffer, bone_id=0, group_id=0;
		float weight=0.0;

		/* FIRST READ */
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'b':
					b++;
					break;
				case 'c':
					c++;
					break;
				case 'p':
					p++;
					break;
				case 'w':
					w++;
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		/* VERIFICATION & ALLOCATION */
		if (b == c && b == p)
		{
			skeleton = malloc(sizeof(Skeleton));
				skeleton->animation_set = NULL;

			skeleton->bones = malloc(sizeof(Bone)*b);
				skeleton->bones_size = b;
			skeleton->vertex_weights = malloc(sizeof(Weight)*w);
				skeleton->vertex_weights_size = w;
		}
		else
		{
			fprintf(stderr, "Erreur lors du chargement de: %s\n", path);

			fprintf(stderr, "Bones: %u\n", b);
			fprintf(stderr, "Childs: %u\n", c);
			fprintf(stderr, "Weights: %u\n", w);

			fclose(file);
			return NULL;
		}

		/* SECOND READ */
		rewind(file);
		w=0;
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'b':
					fgetc(file);
					fscanf(file, "%u", &bone_id);
					break;
				case 'c':
					for(skeleton->bones[bone_id].childs_size=0; fgetc(file) != '\n'; skeleton->bones[bone_id].childs_size++)
						fscanf(file, "%u", &uibuffer);
					break;
				case 'w':
					for(skeleton->vertex_weights[w].count=0; fgetc(file) != '\n'; skeleton->vertex_weights[w].count++)
						fscanf(file, "%u %f", &group_id, &weight);
					w++;
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		/* ALLOCATION */
		for(b=0; b < skeleton->bones_size; b++)
		{
			skeleton->bones[b].childs = malloc(sizeof(int)*skeleton->bones[b].childs_size);
			Quat_to_identity(&skeleton->bones[b].quaternion);
			Vec3f_to_identity(&skeleton->bones[b].translation);
		}

		for(w=0; w < skeleton->vertex_weights_size; w++)
		{
			skeleton->vertex_weights[w].weights = malloc(sizeof(float)*skeleton->vertex_weights[w].count);
			skeleton->vertex_weights[w].bones_index = malloc(sizeof(int)*skeleton->vertex_weights[w].count);
		}

		/* THIRD READ */
		rewind(file);
		w=0;
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'b':
					fgetc(file);
					fscanf(file, "%u", &bone_id);
					fgetc(file);
					fscanf(file, "%f %f %f", &skeleton->bones[bone_id].joint.x, &skeleton->bones[bone_id].joint.y, &skeleton->bones[bone_id].joint.z);
					break;
				case 'c':
					for(c=0; fgetc(file) != '\n'; c++)
					{
						fscanf(file, "%u", &uibuffer);
						skeleton->bones[bone_id].childs[c] = &skeleton->bones[uibuffer];
					}
					break;
				case 'p':
					if(fgetc(file) != '\n')
					{
						fscanf(file, "%u", &uibuffer);
						skeleton->bones[bone_id].parent = &skeleton->bones[uibuffer];
					}
					else
						skeleton->bones[bone_id].parent = NULL;
					break;
				case 'w':
					for(i=0; fgetc(file) != '\n'; i++)
						fscanf(file, "%u %f", &skeleton->vertex_weights[w].bones_index[i], &skeleton->vertex_weights[w].weights[i]);
					w++;
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		fclose(file);
		return skeleton;
	}
	else
	{
		fprintf(stderr, "Le fichier est introuvable: %s", path);
		return NULL;
	}
}

void Delete_Skeleton(Skeleton *skeleton)
{
	unsigned int i=0;

	/* Delete all bones */
	for(i=0; i < skeleton->bones_size; i++)
		Delete_Bone(&skeleton->bones[i]);
	free(skeleton->bones);

	/* Delete all vertex_weights */
	for(i=0; i < skeleton->vertex_weights_size; i++)
		Delete_Weight(&skeleton->vertex_weights[i]);
	free(skeleton->vertex_weights);
}

Animation_set* Load_Animation_set(const char* path)
{
	FILE* file = NULL;
	file = fopen(path, "r");

	if(file != NULL)
	{
		char current_char;
		unsigned int i=0, a=0, b=0, k=0, q=0, t=0, s=0;
		int bone_id;
		float fbuffer;
		Animation_set *animation_set = NULL;

		/* FIRST READ */
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'a':
					a++;
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		/* ALLOCATION */
		if(a > 0)
		{
			animation_set = malloc(sizeof(animation_set));
				animation_set->count = a;
				animation_set->animations = malloc(sizeof(Animation)*a);
					for(i=0; i < animation_set->count; i++)
						animation_set->animations[i].count = 0;
		}
		else
		{
			fprintf(stderr, "Aucune animation trouvée dans: %s", path);
			fclose(file);
			return NULL;
		}

		/* SECOND READ */
		rewind(file);
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'a':
					fgetc(file);
					fscanf(file, "%u", &a);
					break;
				case 'b':
					animation_set->animations[a].count++;
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		/* ALLOCATION */
		for(i=0; i < animation_set->count; i++)
		{
			printf("Animation [%u], %u Pose\n", i, animation_set->animations[i].count);
			animation_set->animations[i].poses = malloc(sizeof(Pose)*animation_set->animations[i].count);
			animation_set->animations[i].start = -1;
		}

		/* THIRD READ */
		rewind(file);
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'a':
					fgetc(file);
					fscanf(file, "%u", &a);
					b=0;
					break;
				case 'b':
					fgetc(file);
					fscanf(file, "%u", &bone_id);
					b++;
					break;
				case 'k':
					for(animation_set->animations[a].poses[b-1].count=0; fgetc(file) != '\n'; animation_set->animations[a].poses[b-1].count++)
						fscanf(file, "%f", &fbuffer);
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		/* ALLOCATION */
		for(a=0; a < animation_set->count; a++)
		{
			printf("animation[%i]\n", a);
			for(b=0; b < animation_set->animations[a].count; b++)
			{
				printf("Animation %u, pose %u, %u keys\n", a, b, animation_set->animations[a].poses[b].count);
				animation_set->animations[a].poses[b].keys = malloc(sizeof(float)*animation_set->animations[a].poses[b].count);
				/*animation_set->animations[a].poses[b].matrix = malloc(sizeof(Matrix4f)*animation_set->animations[a].poses[b].count);*/
				animation_set->animations[a].poses[b].quaternion = malloc(sizeof(Quat)*animation_set->animations[a].poses[b].count);
				animation_set->animations[a].poses[b].translation = malloc(sizeof(Vec3f)*animation_set->animations[a].poses[b].count);
				animation_set->animations[a].poses[b].scale = malloc(sizeof(Vec3f)*animation_set->animations[a].poses[b].count);
			}
		}

		/* FINAL READ */
		rewind(file);
		do
		{
			current_char = fgetc(file);
			switch(current_char)
			{
				case 'a':
					fgetc(file);
					fscanf(file, "%u", &a);
					b=0;
					break;
				case 'b':
					fgetc(file);
					fscanf(file, "%u", &bone_id);
					animation_set->animations[a].poses[b].bone_index = bone_id;
					b++;
					q=0; t=0; s=0;
					break;
				case 'k':
					for(k=0; fgetc(file) != '\n'; k++)
						fscanf(file, "%f", &animation_set->animations[a].poses[b-1].keys[k]);
					break;
				/*case 'm':
					fgetc(file);
					fscanf(file, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &animation_set->animations[a].poses[b-1].matrix[m].values[0], &animation_set->animations[a].poses[b-1].matrix[m].values[1], &animation_set->animations[a].poses[b-1].matrix[m].values[2], &animation_set->animations[a].poses[b-1].matrix[m].values[3], &animation_set->animations[a].poses[b-1].matrix[m].values[4], &animation_set->animations[a].poses[b-1].matrix[m].values[5], &animation_set->animations[a].poses[b-1].matrix[m].values[6], &animation_set->animations[a].poses[b-1].matrix[m].values[7], &animation_set->animations[a].poses[b-1].matrix[m].values[8], &animation_set->animations[a].poses[b-1].matrix[m].values[9], &animation_set->animations[a].poses[b-1].matrix[m].values[10], &animation_set->animations[a].poses[b-1].matrix[m].values[11], &animation_set->animations[a].poses[b-1].matrix[m].values[12], &animation_set->animations[a].poses[b-1].matrix[m].values[13], &animation_set->animations[a].poses[b-1].matrix[m].values[14], &animation_set->animations[a].poses[b-1].matrix[m].values[15]);
					print_Matrix4f(animation_set->animations[a].poses[b-1].matrix[m]);
					m++;
					break;*/
				case 'q':
					fgetc(file);
					fscanf(file, "%f %f %f %f", &animation_set->animations[a].poses[b-1].quaternion[q].w, &animation_set->animations[a].poses[b-1].quaternion[q].x, &animation_set->animations[a].poses[b-1].quaternion[q].y, &animation_set->animations[a].poses[b-1].quaternion[q].z);
					q++;
					break;
				case 't':
					fgetc(file);
					fscanf(file, "%f %f %f", &animation_set->animations[a].poses[b-1].translation[t].x, &animation_set->animations[a].poses[b-1].translation[t].y, &animation_set->animations[a].poses[b-1].translation[t].z);
					t++;
					break;
				case 's':
					fgetc(file);
					fscanf(file, "%f %f %f", &animation_set->animations[a].poses[b-1].scale[s].x, &animation_set->animations[a].poses[b-1].scale[s].y, &animation_set->animations[a].poses[b-1].scale[s].z);
					s++;
					break;
				case EOF:
					break;
				case '\n':
					break;
				default:
					while(fgetc(file) != '\n');
					break;
			}
		}while(current_char != EOF);

		fclose(file);
		return animation_set;
	}
	else
	{
		fprintf(stderr, "Le fichier est introuvable: %s", path);
		return NULL;
	}
}

void Delete_Animation_set(Animation_set *animation_set)
{
	unsigned int i=0;

	for(i=0; i < animation_set->count; i++)
		Delete_Animation(&animation_set->animations[i]);
	free(animation_set->animations);
}

void Delete_Bone(Bone *bone)
{
	free(bone->childs);
}

void Delete_Weight(Weight *weight)
{
	free(weight->bones_index);
	free(weight->weights);
}

void Delete_Pose(Pose *pose)
{
	free(pose->keys);
	free(pose->quaternion);
	free(pose->translation);
	free(pose->scale);
}

void Delete_Animation(Animation *animation)
{
	unsigned int i=0;

	for(i=0; i < animation->count; i++)
		Delete_Pose(&animation->poses[i]);
	free(animation->poses);
}

void Draw_Mesh(const Mesh *mesh)
{
	unsigned int i=0;

	glBegin(/*GL_TRIANGLES*/ GL_LINES);
		for(i=0; i < mesh->faces_size; i+=3)
		{
			glNormal3f(mesh->normals[mesh->faces[i+1]].x, mesh->normals[mesh->faces[i+1]].y, mesh->normals[mesh->faces[i+1]].z);
			glVertex3f(mesh->vertices[mesh->faces[i]].x, mesh->vertices[mesh->faces[i]].y, mesh->vertices[mesh->faces[i]].z);
		}
	glEnd();
}

void Draw_Mesh_buffer(const Mesh *mesh)
{
	unsigned int i=0;

	glBegin(GL_TRIANGLES /*GL_LINES*/);
		for(i=0; i < mesh->faces_size; i+=3)
		{
			glNormal3f(mesh->normals[mesh->faces[i+1]].x, mesh->normals[mesh->faces[i+1]].y, mesh->normals[mesh->faces[i+1]].z);
			glVertex3f(mesh->vertices_buffer[mesh->faces[i]].x, mesh->vertices_buffer[mesh->faces[i]].y, mesh->vertices_buffer[mesh->faces[i]].z);
		}
	glEnd();
}

void Draw_Skeleton(Skeleton *skeleton)
{
	unsigned int i;
	Vec3f tmp;

	glPointSize(5);
	glBegin(GL_POINTS);
		glColor3ub(255, 0, 0);
		for(i=0; i < skeleton->bones_size; i++)
		{
			tmp.x = skeleton->bones[i].joint.x + skeleton->bones[i].translation.x;
			tmp.y = skeleton->bones[i].joint.y + skeleton->bones[i].translation.y;
			tmp.z = skeleton->bones[i].joint.z + skeleton->bones[i].translation.z;

			/*tmp.x -= skeleton->bones[i].joint.x;
			tmp.y -= skeleton->bones[i].joint.y;
			tmp.z -= skeleton->bones[i].joint.z;

			tmp = Quat_rotatePoint(skeleton->bones[i].quaternion, tmp);

			tmp.x += skeleton->bones[i].joint.x;
			tmp.y += skeleton->bones[i].joint.y;
			tmp.z += skeleton->bones[i].joint.z;*/

			/*printf("> %f %f %f\n\n", tmp.x, tmp.y, tmp.z);*/
			glVertex3f(tmp.x, tmp.y, tmp.z);
		}
		glColor3ub(255, 255, 255);
		glEnd();
	glPointSize(1);
}

int Make_unique_index(Mesh *mesh)
{
	unsigned int i, j;
	int dbl;

	unsigned int *vertices, *normals, *new_index;
		vertices = calloc(sizeof(unsigned int), mesh->vertices_size);
		new_index = calloc(sizeof(unsigned int), mesh->faces_size);
	unsigned int** table;

	for(i=0; i < mesh->faces_size; i+=3)
		vertices[mesh->faces[i]]++;
	for(i=0; i < mesh->vertices_size; i++)
		printf("%u\t%u\n", i, vertices[i]);
	printf("\n");
	printf("\n");

	table = malloc(sizeof(unsigned int*)*mesh->vertices_size);
	for(i=0; i < mesh->vertices_size; i++)
		table[i] = calloc(sizeof(unsigned int), 2*2*vertices[i]);

	for(i=0; i < mesh->vertices_size; i++)
		vertices[i] = 0;

	for(i=0; i < mesh->faces_size; i+=3) /* for each index */
	{
		dbl = -1;
		for(j=0; j < vertices[mesh->faces[i]]; j++) /* search index in table */
		{
			if(table[mesh->faces[i]][j*4+1] == mesh->faces[i+1])
				dbl = j;
		}

		if(dbl > -1)
		{
			new_index[i] = table[mesh->faces[i]][j*4];
			new_index[i+1] = table[mesh->faces[i]][j*4+1];
		}
		else
		{
			table[mesh->faces[i]][vertices[mesh->faces[i]]*4] = mesh->faces[i];
			table[mesh->faces[i]][vertices[mesh->faces[i]]*4+1] = mesh->faces[i+1];

			table[mesh->faces[i]][vertices[mesh->faces[i]]*4+2] = mesh->faces[i];
			table[mesh->faces[i]][vertices[mesh->faces[i]]*4+3] = mesh->faces[i+1];

			vertices[mesh->faces[i]]++;
		}
	}

	printf("table\n", i);
	for(i=0; i < mesh->vertices_size; i++)
	{
		printf("%u", i);
		for(j=0; j < vertices[i]; j++)
			printf("\t\t%u %u %u %u", table[i][j*4+0], table[i][j*4+1], table[i][j*4+2], table[i][j*4+3]);
		printf("\n");
	}

	for(i=0; i < mesh->faces_size; i+=3)
		printf(" %u %u %u", mesh->faces[i], mesh->faces[i+1], mesh->faces[i+2]);

	return 1;
}
