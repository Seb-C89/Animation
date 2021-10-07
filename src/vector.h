#ifndef VECTOR_H_
#define VECTOR_H_

typedef struct XYZ Vertex3f;
typedef struct XYZ Vec3f;
struct XYZ
{
	float x;
	float y;
	float z;
};

typedef struct XY Vertex2f;
typedef struct XY Vec2f;
struct XY
{
	float x;
	float y;
};

void Vec3f_to_identity(Vec3f *v);
Vec3f Vec3f_lerp(const Vec3f a, const Vec3f b, const float t);

#endif /* VECTOR_H_ */
