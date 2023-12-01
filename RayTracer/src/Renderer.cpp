//#include "Renderer.h"
//
//#include "Walnut/Timer.h"
//#include "Walnut/Random.h"
//
//namespace Utils {
//	static uint32_t ConvertToRGBA(const glm::vec4& color) {
//		uint8_t r = (uint8_t)color.x * 255.0f;
//		uint8_t g = (uint8_t)color.y * 255.0f;
//		uint8_t b = (uint8_t)color.z * 255.0f;
//		uint8_t a = (uint8_t)color.w * 255.0f;
//		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
//		return result;
//	}
//}
//
//void Renderer::OnResize(uint32_t width, uint32_t height) {
//
//	if (m_FinalImage) {
//		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
//			return;
//		m_FinalImage->Resize(width, height);
//	}
//	else {
//		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
//	}
//	delete[] m_ImageData;
//	m_ImageData = new uint32_t[width * height];
//}
//
//void Renderer::Render() {
//	Walnut::Timer timer;
//	uint32_t imageHeight = m_FinalImage->GetHeight();
//	uint32_t imageWidth = m_FinalImage->GetWidth();
//	for (uint32_t y = 0; y < imageHeight; ++y) {
//		for (uint32_t x = 0; x < imageWidth; ++x) {
//			glm::vec2 coord = { (float)x / (float)imageWidth , (float)y / (float)imageHeight };
//			coord = coord * 2.0f - 1.0f; // remappimg from 0;1 to -1;1
//			coord.x = coord.x * (imageWidth / (float)imageHeight);
//			glm::vec4 color = TraceRay(coord);
//			//color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
//			m_ImageData[y * imageWidth + x] = Utils::ConvertToRGBA(color);
//		}
//	}
//	m_FinalImage->SetData(m_ImageData);
//}
//
//glm::vec4 Renderer::TraceRay(glm::vec2 coord) {
//
//	glm::vec3 lightDirection(-1.0f, 1.0f, -1.0f);
//	lightDirection = glm::normalize(lightDirection);
//
//	glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
//	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
//	float radius = 0.5f;
//
//	//a - ray origin
//	//b - direction of the ray
//	//r - radius of sphrere
//	//t - distance to intersections
//
//	float a = glm::dot(rayDirection, rayDirection);
//	float b = 2.0f * glm::dot(rayOrigin, rayDirection);
//	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;
//
//	//quadratic formula 
//
//	float discriminant = b * b - 4.0f * a * c;
//
//	if (discriminant < 0.0f)
//		return glm::vec4(0, 0, 0, 1.0f);
//
//	float t1 = (-b - glm::sqrt(discriminant)) / 2.0f * a;
//	//float t2 = (-b + glm::sqrt(discriminant)) / 2 * a;
//	glm::vec3 hitPoint = rayOrigin + rayDirection * t1;
//	//glm::vec3 normal = glm::normalize(hitPoint);	
//
//	glm::vec3 sphereColor = hitPoint;
//
//	return glm::vec4(sphereColor, 1.0f);
//
//	
//}

#include "Renderer.h"

#include "Walnut/Random.h"

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

}

void Renderer::OnResize(uint32_t width, uint32_t height){
	if (m_FinalImage){
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render(const Camera& camera){
	Ray ray;
	ray.Origin = camera.GetPosition();

	uint32_t imageHeight = m_FinalImage->GetHeight();
	uint32_t imageWidth = m_FinalImage->GetWidth();
	for (uint32_t y = 0; y < imageHeight; ++y) {
		for (uint32_t x = 0; x < imageWidth; ++x) {

			//glm::vec2 coord = { (float)x / (float)imageWidth , (float)y / (float)imageHeight };
			//coord = coord * 2.0f - 1.0f; // remappimg from 0;1 to -1;1
			//coord.x = coord.x * (imageWidth / (float)imageHeight);
			ray.Direction = camera.GetRayDirections()[x + y * imageWidth];
			glm::vec4 color = TraceRay(ray);
			//color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
			m_ImageData[y * imageWidth + x] = Utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::TraceRay(const Ray& ray)
{
	float radius = 0.5f;
	// rayDirection = glm::normalize(rayDirection);

	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance

	float a = glm::dot(ray.Direction, ray.Direction);
	float b = 2.0f * glm::dot(ray.Origin, ray.Direction);
	float c = glm::dot(ray.Origin, ray.Origin) - radius * radius;

	// Quadratic forumula discriminant:
	// b^2 - 4ac

	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)
		return glm::vec4(0.2f, 0.3f, 0.7f, 1);

	// Quadratic formula:
	// (-b +- sqrt(discriminant)) / 2a

	float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
	//float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)

	glm::vec3 hitPoint = ray.Origin + ray.Direction * closestT;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, 1, -1));
	float lightIntensity = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 sphereColor(1, 0, 1);
	sphereColor *= lightIntensity;
	return glm::vec4(sphereColor, 1.0f);
}