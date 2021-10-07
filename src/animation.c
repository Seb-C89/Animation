#include <stdlib.h>
#include <stdio.h>
#include "../header/animation.h"

/*
q = Quaternion((0.921,0.390,0.0,0.0))
o = Vector((0, 0, 1))
p = Vector((0, 0, 5.9))
o+(q*(p-o))
 */

void Rotate_Bone(Bone *bone, const Quat q)
{
	unsigned int i;
	Vec3f tmp, arround;
	Vec3f v;

	bone->quaternion = Quat_mult_Quat(bone->quaternion, q);

	for(i=0; i < bone->childs_size; i++)
	{
		tmp.x = (bone->childs[i]->joint.x + bone->childs[i]->translation.x) - (bone->joint.x + bone->translation.x);
		tmp.y = (bone->childs[i]->joint.y + bone->childs[i]->translation.y) - (bone->joint.y + bone->translation.y);
		tmp.z = (bone->childs[i]->joint.z + bone->childs[i]->translation.z) - (bone->joint.z + bone->translation.z);

		tmp = Quat_rotatePoint(q, tmp);

		tmp.x += (bone->joint.x + bone->translation.x);
		tmp.y += (bone->joint.y + bone->translation.y);
		tmp.z += (bone->joint.z + bone->translation.z);

		v.x = tmp.x - (bone->childs[i]->joint.x + bone->childs[i]->translation.x);
		v.y = tmp.y - (bone->childs[i]->joint.y + bone->childs[i]->translation.y);
		v.z = tmp.z - (bone->childs[i]->joint.z + bone->childs[i]->translation.z);

		/*printf("Rotate: %p arround: %f %f %f tail after rotation: %f %f %f so translate: %f %f %f\n", bone, bone->joint.x, bone->joint.y, bone->joint.z, tmp.x, tmp.y, tmp.z, v.x, v.y, v.z);*/

		Translate_Bone(bone->childs[i], v);
		Rotate_Bone(bone->childs[i], q);
	}
}

void Translate_Bone(Bone *bone, const Vec3f v)
{
	unsigned int i;

	/*printf("Tanslate: %p %f %f %f + %f %f %f\n", bone, bone->translation.x, bone->translation.y, bone->translation.z, v.x, v.y, v.z);*/

	bone->translation.x += v.x;
	bone->translation.y += v.y;
	bone->translation.z += v.z;

	for(i=0; i < bone->childs_size; i++)
	{
		/*printf("-> %p\n", bone->childs[i]);*/
		Translate_Bone(bone->childs[i], v);
	}
}

void Play_Animation(const Mesh *mesh, const unsigned int id_anim, int tick)
{
	/*printf("************************\n");*/

	unsigned int i,j,k,key;
	float t;
	Vec3f tmp;

	if(mesh->skeleton != NULL)
	{
		if(id_anim < mesh->skeleton->animation_set->count)
		{
			/* Start the anim */
			if(mesh->skeleton->animation_set->animations[id_anim].start < 0)
				mesh->skeleton->animation_set->animations[id_anim].start = tick;

			/* Clean bones */
			for(i=0; i < mesh->skeleton->bones_size; i++)
			{
				Quat_to_identity(&mesh->skeleton->bones[i].quaternion);
				Vec3f_to_identity(&mesh->skeleton->bones[i].translation);
			}

			/* Compute current key */
			/*printf("KEY MAX: %u\n", mesh->skeleton->animation_set->animations[id_anim].count);*/
			for(i=0; i < mesh->skeleton->animation_set->animations[id_anim].count; i++)
			{
				for(key=0; (tick - mesh->skeleton->animation_set->animations[id_anim].start) > mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key] && key < mesh->skeleton->animation_set->animations[id_anim].poses[i].count; key++);

				/*printf("%u KEY %u / %u\n", i, key, mesh->skeleton->animation_set->animations[id_anim].poses[i].count);*/

				if(key <= 0)
				{
					/* between rest_pose and first_key */
					Rotate_Bone(&mesh->skeleton->bones[mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index], mesh->skeleton->animation_set->animations[id_anim].poses[i].quaternion[0]);
					Translate_Bone(&mesh->skeleton->bones[mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index], mesh->skeleton->animation_set->animations[id_anim].poses[i].translation[0]);
				}
				else if(key >= mesh->skeleton->animation_set->animations[id_anim].poses[i].count)
				{
					/* between two last_key */
					Rotate_Bone(&mesh->skeleton->bones[mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index], mesh->skeleton->animation_set->animations[id_anim].poses[i].quaternion[mesh->skeleton->animation_set->animations[id_anim].poses[i].count-1]);
					Translate_Bone(&mesh->skeleton->bones[mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index], mesh->skeleton->animation_set->animations[id_anim].poses[i].translation[mesh->skeleton->animation_set->animations[id_anim].poses[i].count-1]);
				}
				else
				{
					/* between two key */

					/*for(k=0; k < mesh->skeleton->animation_set->animations[0].count; k++)
					{
						for(j=0;  j < mesh->skeleton->animation_set->animations[0].poses[k].count; j++)
						{
							printf("Pose[%u], keys[%u]: %f\n", k, j, mesh->skeleton->animation_set->animations[0].poses[k].keys[j]);
						}
					}*/

					/*printf("%i, %i, %f, %f\n", tick, mesh->skeleton->animation_set->animations[id_anim].start, mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key], mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key-1]);
					printf("%i - %f / %f\n", tick - mesh->skeleton->animation_set->animations[id_anim].start, mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key-1], mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key] - mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key-1]);*/
					t = ((tick - mesh->skeleton->animation_set->animations[id_anim].start) - mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key-1]) / (mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key] - mesh->skeleton->animation_set->animations[id_anim].poses[i].keys[key-1]);
					/*printf("> t = %f\n", t);*/
					/*Quat q = Quat_slerp(mesh->skeleton->animation_set->animations[id_anim].poses[i].quaternion[key-1], mesh->skeleton->animation_set->animations[0].poses[i].quaternion[key], t);*/
					/*Vec3f v = Vec3f_lerp(mesh->skeleton->animation_set->animations[id_anim].poses[i].translation[key-1], mesh->skeleton->animation_set->animations[0].poses[i].translation[key], t);
					printf("%u %u %f v: %f %f %f (%f %f %f / %f %f %f)\n", i, mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index, t, v.x, v.y, v.z, mesh->skeleton->animation_set->animations[0].poses[i].translation[key].x, mesh->skeleton->animation_set->animations[0].poses[i].translation[key].y, mesh->skeleton->animation_set->animations[0].poses[i].translation[key].z, mesh->skeleton->animation_set->animations[0].poses[i].translation[key-1].x, mesh->skeleton->animation_set->animations[0].poses[i].translation[key-1].y, mesh->skeleton->animation_set->animations[0].poses[i].translation[key-1].z);*/
					Rotate_Bone(&mesh->skeleton->bones[mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index], Quat_slerp(mesh->skeleton->animation_set->animations[id_anim].poses[i].quaternion[key-1], mesh->skeleton->animation_set->animations[0].poses[i].quaternion[key], t));
					Translate_Bone(&mesh->skeleton->bones[mesh->skeleton->animation_set->animations[id_anim].poses[i].bone_index], Vec3f_lerp(mesh->skeleton->animation_set->animations[id_anim].poses[i].translation[key-1], mesh->skeleton->animation_set->animations[0].poses[i].translation[key], t));
				}
			}

			/* Compute vertices position */
			for(i=0; i < mesh->vertices_size; i++)
			{
				mesh->vertices_buffer[i].x = 0;
				mesh->vertices_buffer[i].y = 0;
				mesh->vertices_buffer[i].z = 0;

				/*printf("%u:", i);*/

				for(j=0; j < mesh->skeleton->vertex_weights[i].count; j++)
				{
					tmp = mesh->vertices[i];

					tmp.x += mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.x;
					tmp.y += mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.y;
					tmp.z += mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.z;

					tmp.x -= mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].joint.x + mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.x;
					tmp.y -= mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].joint.y + mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.y;
					tmp.z -= mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].joint.z + mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.z;

					tmp = Quat_rotatePoint(mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].quaternion, tmp);

					tmp.x += mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].joint.x + mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.x;
					tmp.y += mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].joint.y + mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.y;
					tmp.z += mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].joint.z + mesh->skeleton->bones[mesh->skeleton->vertex_weights[i].bones_index[j]].translation.z;

					tmp.x *= mesh->skeleton->vertex_weights[i].weights[j];
					tmp.y *= mesh->skeleton->vertex_weights[i].weights[j];
					tmp.z *= mesh->skeleton->vertex_weights[i].weights[j];

					mesh->vertices_buffer[i].x += tmp.x;
					mesh->vertices_buffer[i].y += tmp.y;
					mesh->vertices_buffer[i].z += tmp.z;

					/*printf("%u\n", j);*/
					/*printf("%f ", mesh->skeleton->vertex_weights[i].weights[j]);*/
				}
				printf("\n");
			}
		}
	}
}
