import bpy
import mathutils
import math
from mathutils import *
from math import *

def get_bone_index(bone_name):
    return mesh.vertex_groups[bone_name].index

### Script
mesh = bpy.context.active_object
armature = mesh.parent #Best way ?
scene = bpy.context.scene #Current Scenes
scene.frame_set(0)
path = 'C:\\Users\\FAMILLE\\Desktop\\cdt_work_space\\Animation\\mesh\\';

#### Prepare
#Triangulate
#Unwarp

#### Export mesh
out = open(path+'mesh', 'w')

#Vertices
out.write('v')
for v in mesh.data.vertices:
    out.write(' %f %f %f' % (v.co.x, v.co.y, v.co.z))
out.write('\n')

#Normals
#smooth = 4 differents normals (use vertex normals)
#flat = 4 sames noramls (use 4x faces normals)
normals = {}
totno = 0

def veckey3d(v):
        return v.x, v.y, v.z

out.write('n')
for f in mesh.data.polygons:
    if f.use_smooth:
        for v in f.vertices:
            if veckey3d(mesh.data.vertices[v].normal) not in normals:
                normals[veckey3d(mesh.data.vertices[v].normal)] = totno
                totno += 1
                out.write(' %f %f %f' % (mesh.data.vertices[v].normal.x, mesh.data.vertices[v].normal.y, mesh.data.vertices[v].normal.z))
    else:
        if veckey3d(f.normal) not in normals:
            normals[veckey3d(f.normal)] = totno
            totno += 1
            out.write(' %f %f %f' % (f.normal.x, f.normal.y, f.normal.z))
out.write('\n')

#UVco
uvco = {}
totco = 0;

def veckey2d(v):
    return v.x, v.y

out.write('t')
for f in mesh.data.polygons:
    for t in f.loop_indices:
        if veckey2d(mesh.data.uv_layers.active.data[t].uv) not in uvco:
            uvco[veckey2d(mesh.data.uv_layers.active.data[t].uv)] = totco
            totco += 1
            out.write(' %f %f' % (mesh.data.uv_layers.active.data[t].uv.x, mesh.data.uv_layers.active.data[t].uv.y))
out.write('\n')

#Faces
out.write('f')
for f in mesh.data.polygons:
    for v, t in zip(f.vertices, f.loop_indices):
        out.write(' %i' % (v))
        if f.use_smooth:
            out.write(' %i' % (normals[veckey3d(mesh.data.vertices[v].normal)]))
        else:
            out.write(' %i' % (normals[veckey3d(f.normal)]))
        out.write(' %i' % (uvco[veckey2d(mesh.data.uv_layers.active.data[t].uv)]))
out.write('\n')

out.close()

### Export skeleton
out = open(path+'skeleton', 'w')

for b in armature.pose.bones:
    out.write('b %i %f %f %f\n' % (get_bone_index(b.bone.name), b.head.x, b.head.y, b.head.z))
    out.write('c')
    for c in b.children:
        out.write(' %i' % (get_bone_index(c.name)))
    out.write('\n')
    out.write('p')
    if b.parent: # != None
        out.write(' %i' % (get_bone_index(b.parent.name)))
    out.write('\n')

for v in mesh.data.vertices:
    out.write('w')
    for g in v.groups:
        out.write(' %i %f' % (g.group, g.weight)) # Bone_Id Weight
    out.write('\n')

out.close()

### Anim
out = open(path+'anim', 'w')

out.write('a 0\n')
for b in armature.pose.bones:
    bone_index = get_bone_index(b.bone.name)
    out.write('b %i\n' % (bone_index))
    out.write('k')
    for k in armature.animation_data.action.groups[bone_index].channels[0].keyframe_points: #channel 0 is x location. supose use LocRotScale
        out.write(' %i' % (k.co.x/25*1000)) #A CORRIGER !
    out.write('\n')
    for k in armature.animation_data.action.groups[bone_index].channels[0].keyframe_points:
        scene.frame_set(k.co.x)
        convert_m = Matrix.Rotation(pi * -90 / 180, 4, 'X')
        m = armature.pose.bones[bone_index].matrix * armature.pose.bones[bone_index].bone.matrix_local.inverted()
        q = armature.pose.bones[bone_index].rotation_quaternion
        t = armature.pose.bones[bone_index].location * convert_m
        s = armature.pose.bones[bone_index].scale * convert_m

        #out.write('m %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n' % (m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1], m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3], m[2][3], m[3][3]))
        out.write('q %f %f %f %f\n' % (q.w, q.x, q.y, q.z))
        out.write('t %f %f %f\n' % (t.x, t.y, t.z))
        out.write('s %f %f %f\n' % (s.x, s.y, s.z))

out.close()