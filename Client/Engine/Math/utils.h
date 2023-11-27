#pragma once
#include <PCH/pch.h>

inline float Lerp(float a, float b, float t) {
	return (1.0f - t) * a + b * t;
}

inline glm::vec2 Lerp(const glm::vec2& a, const glm::vec2& b, float t) {
	return glm::vec2(Lerp(a.x, b.x, t), Lerp(a.y, b.y, t));
}

inline float InvLerp(float a, float b, float v) {
	return (v - a) / (b - a);
}

inline float Remap(float iMin, float iMax, float oMin, float oMax, float v) {
	float t = InvLerp(iMin, iMax, v);
	return Lerp(oMin, oMax, t);
}