#include <stdio.h>
#include <string.h>
#include <math.h>

#include "quaternion.h"

void Quat_to_identity(Quat *q)
{
	q->w = 1.0;
	q->x = 0.0;
	q->y = 0.0;
	q->z = 0.0;
}

Quat Quat_mult_Quat(const Quat qa, const Quat qb)
{
	Quat out;

	out.w = (qa.w * qb.w) - (qa.x * qb.x) - (qa.y * qb.y) - (qa.z * qb.z);
	out.x = (qa.x * qb.w) + (qa.w * qb.x) + (qa.y * qb.z) - (qa.z * qb.y);
	out.y = (qa.y * qb.w) + (qa.w * qb.y) + (qa.z * qb.x) - (qa.x * qb.z);
	out.z = (qa.z * qb.w) + (qa.w * qb.z) + (qa.x * qb.y) - (qa.y * qb.x);

	return out;
}

Quat Quat_mult_Vec3f(const Quat q, const Vec3f v)
{
	Quat out;

	out.w = - (q.x * v.x) - (q.y * v.y) - (q.z * v.z);
	out.x =   (q.w * v.x) + (q.y * v.z) - (q.z * v.y);
	out.y =   (q.w * v.y) + (q.z * v.x) - (q.x * v.z);
	out.z =   (q.w * v.z) + (q.x * v.y) - (q.y * v.x);

	return out;
}

void Quat_normalize (Quat *q)
{
	/* compute magnitude of the Quaternion */
	float mag = sqrt((q->x * q->x) + (q->y * q->y) + (q->z * q->z) + (q->w * q->w));

	/* check for bogus length, to protect against divide by zero */
	if (mag > 0.0f)
	{
		/* normalize it */
		float oneOverMag = 1.0f / mag;

		q->x *= oneOverMag;
		q->y *= oneOverMag;
		q->z *= oneOverMag;
		q->w *= oneOverMag;
	}
}

Quat Quat_normalized (Quat q)
{
	Quat_normalize(&q);
	return q;
}

Vec3f Quat_rotatePoint (const Quat q, const Vec3f v)
{
	Quat tmp, inv, final;
	Vec3f out;

	inv.x = -q.x;
	inv.y = -q.y;
	inv.z = -q.z;
	inv.w =  q.w;

	Quat_normalize(&inv);

	tmp = Quat_mult_Vec3f (q, v);
	final = Quat_mult_Quat (tmp, inv);

	out.x = final.x;
	out.y = final.y;
	out.z = final.z;

	return out;
}

float Quat_dotProduct (const Quat qa, const Quat qb)
{
	/* quaternion dot product */
	return ((qa.x * qb.x) + (qa.y * qb.y) + (qa.z * qb.z) + (qa.w * qb.w));
}

Quat Quat_slerp (const Quat qa, const Quat qb, const float t)
{
	Quat out;
	Quat_to_identity(&out);

	/* check for out-of range parameter and return edge points if so */
	if (t <= 0.0)
	{
		memcpy (&out, &qa, sizeof(Quat));
		return out;
	}

	if (t >= 1.0)
	{
		memcpy (&out, &qb, sizeof (Quat));
		return out;
	}

	/* compute "cosine of angle between quaternions" using dot product */
	float cosOmega = Quat_dotProduct (qa, qb);

	/* if negative dot, use -q1.  two quaternions q and -q
	represent the same rotation, but may produce
	different slerp.  we chose q or -q to rotate using
	the acute angle. */
	float q1w = qb.w;
	float q1x = qb.x;
	float q1y = qb.y;
	float q1z = qb.z;

	if (cosOmega < 0.0f)
	{
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}

	/* we should have two unit quaternions, so dot should be <= 1.0 */
	/*assert (cosOmega < 1.1f);*/
	if(!(cosOmega <= 1.1f))
	{
	    fprintf(stderr, "Quaternion assert: cosOmega %f\n", cosOmega);
	    return out;
	}

	/* compute interpolation fraction, checking for quaternions
	almost exactly the same */
	float k0, k1;

	if (cosOmega > 0.9999f)
	{
		/* very close - just use linear interpolation,
		which will protect againt a divide by zero */
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		/* compute the sin of the angle using the
		trig identity sin^2(omega) + cos^2(omega) = 1 */
		float sinOmega = sqrt (1.0f - (cosOmega * cosOmega));

		/* compute the angle from its sin and cosine */
		float omega = atan2 (sinOmega, cosOmega);

		/* compute inverse of denominator, so we only have
		to divide once */
		float oneOverSinOmega = 1.0f / sinOmega;

		/* Compute interpolation parameters */
		k0 = sin ((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sin (t * omega) * oneOverSinOmega;
	}

	/* interpolate and return new quaternion */
	out.w = (k0 * qa.w) + (k1 * q1w);
	out.x = (k0 * qa.x) + (k1 * q1x);
	out.y = (k0 * qa.y) + (k1 * q1y);
	out.z = (k0 * qa.z) + (k1 * q1z);
	return out;
}

Vec3f Turn_arround(const Vec3f turn, const Vec3f arround, const Quat q)
{
	Vec3f out;

	out.x = turn.x - arround.x;
	out.x = turn.y - arround.y;
	out.x = turn.z - arround.z;

	out = Quat_to_Vec3f(Quat_mult_Vec3f(q, out));

	out.x += arround.x;
	out.y += arround.y;
	out.z += arround.z;

	return out;
}

Vec3f Quat_to_Vec3f(const Quat q)
{
	Vec3f out;
		out.x = q.x;
		out.y = q.y;
		out.z = q.z;
	return out;
}
