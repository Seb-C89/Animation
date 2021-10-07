#ifndef MESH_H_
#define MESH_H_

#include "vector.h"
#include "quaternion.h"

typedef struct Bone Bone;
struct Bone
{
	Bone *parent;
	Bone **childs;
        unsigned int childs_size;

    /*Matrix4f matrix;*/
    Quat quaternion;
    Vec3f translation;
    Vec3f scale;
    Vec3f joint;
};

typedef struct Weight Weight;
struct Weight
{
    float *weights;
    unsigned int *bones_index;
    	unsigned int count;
};

typedef struct Pose Pose;
struct Pose
{
    unsigned int bone_index;

    float *keys;
    /*Matrix4f *matrix;*/
    Quat *quaternion;
    Vec3f *translation;
    Vec3f *scale;

    unsigned int count;
};

typedef struct Animation Animation;
struct Animation
{
    Pose *poses;
        unsigned int count;
    int start;
};

typedef struct Animation_set Animation_set;
struct Animation_set
{
    Animation *animations;
    	unsigned int count;
};


typedef struct Skeleton Skeleton;
struct Skeleton
{
    Bone *bones;
        unsigned int bones_size;
    Weight *vertex_weights;
        unsigned int vertex_weights_size;

    Animation_set *animation_set;
};

typedef struct Mesh Mesh;
struct Mesh
{
	Vertex3f *vertices;
	Vertex3f *vertices_buffer;
		unsigned int vertices_size;
	Vec3f *normals;
	Vec3f *normals_buffer;
		unsigned int normals_size;
    Vertex2f *texturesco;
        unsigned int texturesco_size;
	unsigned int *faces;
		unsigned int faces_size;

	Skeleton *skeleton;
};

Mesh* Load_Mesh(const char* path);
Skeleton* Load_Skeleton(const char* path);
Animation_set* Load_Animation_set(const char* path);
void Delete_Mesh(Mesh *mesh);
void Delete_Skeleton(Skeleton *skeleton);
void Delete_Animation_set(Animation_set *animation_set);
void Delete_Bone(Bone *bone);
void Delete_Weight(Weight *weight);
void Delete_Pose(Pose *pose);
void Delete_Animation(Animation *animation);
void Draw_Mesh(const Mesh *mesh);
void Draw_Mesh_buffer(const Mesh *mesh);
void Draw_Skeleton(Skeleton *skeleton);

#endif /* MESH_H_ */
