#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "vector.h"

typedef struct Quat Quat;
struct Quat
{
    float w;
    float x;
    float y;
    float z;
};

void Quat_to_identity(Quat *q);
Quat Quat_mult_Quat(const Quat qa, const Quat qb);
Quat Quat_mult_Vec3f(const Quat q, const Vec3f v);
void Quat_normalize (Quat *q);
Quat Quat_normalized (Quat q);
Vec3f Quat_rotatePoint (const Quat q, const Vec3f v);
float Quat_dotProduct (const Quat qa, const Quat qb);
Quat Quat_slerp (const Quat qa, const Quat qb, const float t);
Vec3f Turn_arround(const Vec3f turn, const Vec3f arround, const Quat q);
Vec3f Quat_to_Vec3f(const Quat q);


#endif /* QUATERNION_H_ */
