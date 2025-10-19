#include "mathlib/mathlib.h"

void AngleVectors(const QAngle & angles, Vector * forward, Vector * right, Vector * up)
{
  float sr, sp, sy, cr, cp, cy;

  const float* a = reinterpret_cast<const float*>(&angles);

  float pitch_rad = a[0] * (3.14159265358979323846f / 180.0f);
  float yaw_rad = a[1] * (3.14159265358979323846f / 180.0f);
  float roll_rad = a[2] * (3.14159265358979323846f / 180.0f);

  sy = sinf(yaw_rad);
  cy = cosf(yaw_rad);
  sp = sinf(pitch_rad);
  cp = cosf(pitch_rad);
  sr = sinf(roll_rad);
  cr = cosf(roll_rad);

  if (forward)
  {
    forward->x = cp * cy;
    forward->y = cp * sy;
    forward->z = -sp;
  }

  if (right)
  {
    right->x = -sr * sp * cy + cr * sy;
    right->y = -sr * sp * sy - cr * cy;
    right->z = -sr * cp;
  }

  if (up)
  {
    up->x = cr * sp * cy + sr * sy;
    up->y = cr * sp * sy - sr * cy;
    up->z = cr * cp;
  }
};
