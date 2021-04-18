#include <limits>
#include "RayTracer.h"
#include "Material.h"
#include "Geometry.h"
#include "LightSource.h"


//Базовый алгоритм трассировки луча
float3 SimpleRT::TraceRay(const Ray & ray, const std::vector<std::shared_ptr<GeoObject>> &geo, const int &depth)
{
  float tnear = std::numeric_limits<float>::max();
  int   geoIndex = -1;

  SurfHit surf;

  for (int i = 0; i < geo.size(); ++i)
  {
    SurfHit temp;

    if (geo.at(i)->Intersect(ray, 0.001, tnear, temp))
    {
      if (temp.t < tnear)
      {
        tnear = temp.t;
        geoIndex = i;
        surf = temp;
      }
    }
  }


  if (geoIndex == -1)
  {
    float3 unit_direction = normalize(ray.d);
    float t = 0.5f * (unit_direction.y + 1.0f);

    return (1.0f - t) * float3(1.0f, 1.0f, 1.0f) + t * bg_color;
  }

  float3 surfColor(0.0f, 0.0f, 0.0f);
  if (dot(ray.d, surf.normal) > 0)
  {
    surf.normal = -surf.normal;
  }

  Ray scattered;
  if(depth < max_ray_depth && surf.m_ptr->Scatter(ray, surf, surfColor, scattered))
  {
    return surfColor * TraceRay(scattered, geo, depth + 1);
  }
  else
  {
    return float3(0.0f, 0.0f, 0.0f);
  }
}


bool WhittedRT::ShadowRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geoobjects) {
	Ray timeRay = ray;
	float tnear = std::numeric_limits<float>::max();
	int   GOIndex = -1;
	SurfHit surf;
	for (int i = 0; i < geoobjects.size(); ++i)
	{
		SurfHit temp;
		if (geoobjects.at(i)->Intersect(timeRay, 0.001, tnear, temp))
		{
			if (temp.t < tnear)
			{
				tnear = temp.t;
				GOIndex = i;
				surf = temp;
			}
			return true;
		}
	}
	return false;
}


float3 WhittedRT::TraceRay(const Ray& ray, const std::vector<std::shared_ptr<GeoObject>>& geoobjects, const std::vector<std::shared_ptr<LightSource>>& light) {
	float3 color = float3(1.0f, 0.9f, 0.8f); 
	float3 timeColor = float3(1.0f, 1.0f, 1.0f); // базовый цвет, белый
	SurfHit surf;
	Ray timeRay = ray;

	while (1) {
		color *= timeColor;
		float tnear = std::numeric_limits<float>::max();

		int   GOIndex = -1;
		SurfHit surf;

		for (int i = 0; i < geoobjects.size(); ++i)
		{
			SurfHit temp;

			if (geoobjects.at(i)->Intersect(timeRay, 0.001, tnear, temp))//  есть пересечение
			{
				if (temp.t < tnear)
				{
					tnear = temp.t;
					GOIndex = i;
					surf = temp;
				}
			}
		}

		if (GOIndex == -1)// не случилось пересечения
		{
			float3 unit_direction = normalize(timeRay.d);
			float t = 0.5f * (unit_direction.y + 1.0f);
			timeColor = (1.0f - t) + t * bg_color;


			break;
		}

		Ray scattered;

		if (typeid(*surf.m_ptr) != typeid(LightSource))
		{

			if ((typeid(*surf.m_ptr).name() == typeid(Diffuse).name())) //пресечение с диффузным материалом
			{
				timeColor = color/10; // не разделишь, не сможешь нормально регулировать цвет
				// в папке out есть пример такого "испорченного" изображения
				float3 time; //угасание
				
				for (int i = 0; i < light.size(); i++) {
					Ray rayIn;
					rayIn.o = light.at(i)->position;
					rayIn.d = normalize(rayIn.o - surf.hitPoint);


					Ray shadow(surf.hitPoint + normalize(surf.normal) * 10e-5, rayIn.d); //избежать бага чёрных пикселей, путём растягивания точки попадания

					if (!ShadowRay(shadow, geoobjects))	
					{
						surf.m_ptr->Scatter(rayIn, surf, time, scattered);
						timeColor += time * (light.at(i)->intensity) * (light.at(i)->color) * color;  //о да, фикс по кол-ву источников
				
					}
				}
				break;
			}
			
			
			else
				if (surf.m_ptr->Scatter(timeRay, surf, timeColor, scattered))//  зеркало
				{
					
					timeRay = scattered;
				}
				else
				{
					timeColor = float3(0.0f, 0.0f, 0.0f);
				}
		}
		else {		
			surf.m_ptr->Scatter(timeRay, surf, timeColor, scattered);
			break;
		}
	}
	color *= timeColor;
	return color;
}


