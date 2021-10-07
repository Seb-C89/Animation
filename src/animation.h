#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "mesh.h"

void Rotate_Bone(Bone *bone, const Quat q);
void Translate_Bone(Bone *bone, const Vec3f v);
void Play_Animation(const Mesh *mesh, const unsigned int id_anim, int tick);

#endif /* ANIMATION_H_ */
