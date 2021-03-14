#ifndef RT_SAMPLE_NEW_GEOMETRY_H
#define RT_SAMPLE_NEW_GEOMETRY_H

#include "LiteMath.h"
#include "RayTracer.h"
#include "Material.h"

using namespace HydraLiteMath;

//Базовый виртуальный класс для геометрического объекта, с которым может пересекаться луч
class GeoObject
{
public:
  virtual bool Intersect(const Ray &ray, float t_min, float t_max, SurfHit &surf) const = 0;
  std::shared_ptr<Material> m_ptr;
  explicit GeoObject(Material *a_m) : m_ptr(a_m) {} ;
  virtual ~GeoObject() = default;
};

//Бесконечная плоскость
class Plane : public GeoObject
{
public:
  Plane(const float3 a_point, const float3 a_normal, Material *a_m) : GeoObject(a_m), point(a_point), normal(a_normal) {}
  ~Plane() = default;

  bool Intersect(const Ray &ray, float t_min, float t_max, SurfHit &surf) const override;

private:
  float3 point;
  float3 normal;
};

class Sphere : public GeoObject //сфера
{
public:

	Sphere(const float3& a_center, const float& a_r, Material* a_m) : GeoObject(a_m), center(a_center), r(a_r), r_sq(a_r* a_r) {}
	~Sphere() = default;

	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;

private:
	float3 center;
	float r;
	float r_sq;
};



class Triangle : public GeoObject
{
public:
	Triangle(const float3 A, const float3 B, const float3 C, Material* a_m) : GeoObject(a_m), A(A), B(B), C(C) {}
	~Triangle() = default;
	bool Intersect(const Ray& ray, float t_min, float t_max, SurfHit& surf) const override;
private:
	float3 A, B, C;
	float3 normal;
};

#endif //RT_SAMPLE_NEW_GEOMETRY_H
