#include "Geometry.h"


bool Plane::Intersect(const Ray &ray, float t_min, float t_max, SurfHit &surf) const
{
  surf.t = dot((point - ray.o), normal) / dot(ray.d, normal);

  if(surf.t > t_min && surf.t < t_max)
  {
    surf.hit      = true;
    surf.hitPoint = ray.o + surf.t * ray.d;
    surf.normal   = normal;
    surf.m_ptr    = m_ptr;
    return true;
  }

  return false;
}


bool Sphere::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const
{
    float3 k = ray.o - center;

    float a = dot(ray.d, ray.d);
    float b = dot(2 * k, ray.d);
    float c = dot(k, k) - r_sq;

    float d = b * b - 4 * a * c;

    if (d < 0) return false;

    surf.t = (-b - sqrt(d)) / 2 * a;

    if (surf.t > t_min && surf.t < t_max)
    {
        surf.hit = true;
        surf.hitPoint = ray.o + surf.t * ray.d;
        surf.normal = normalize(surf.hitPoint - center);
        surf.m_ptr = m_ptr;
        return true;
    }

    return false;
}


bool Triangle::Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const  //проверка пересечения луча с поверхностью треугольника
{

    surf.normal = cross((B - A), (C - A));
    if (dot(ray.d, surf.normal) == 0) return false;
    float3 T = ray.o - A;
    float3 E1 = B - A;
    float3 E2 = C - A;
    float det = dot(E1, cross(ray.d, E2));
    if (det<t_min && det>t_max) return false;
    float U = dot(T, cross(ray.d, E2)) * 1 / det;
    float V = dot(ray.d, cross(T, E1)) * 1 / det;
    if (U < 0 || U>1) return false;
    if (V < 0 || U + V>1) return false;
    surf.t = dot(E2, cross(T, E1)) * 1 / det;
    if (surf.t > t_min && surf.t < t_max) {
        surf.hit = true;
        surf.hitPoint = float3(surf.t, U, V);
        surf.m_ptr = m_ptr;
        return true;
    }

    return false;
}
