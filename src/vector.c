#include "../header/vector.h"

void Vec3f_to_identity(Vec3f *v)
{
	v->x = 0.0;
	v->y = 0.0;
	v->z = 0.0;
}

Vec3f Vec3f_lerp(const Vec3f a, const Vec3f b, const float t)
{
	Vec3f out;

	out.x = a.x+(b.x-a.x)*t;
	out.y = a.y+(b.y-a.y)*t;
	out.z = a.z+(b.z-a.z)*t;

	return out;
}
