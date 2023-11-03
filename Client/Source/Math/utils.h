#pragma once

inline float Lerp(float a, float b, float t) {
	return (1.0f - t) * a + b * t;
}

inline float InvLerp(float a, float b, float v) {
	return (v - a) / (b - a);
}

inline float Remap(float iMin, float iMax, float oMin, float oMax, float v) {
	float t = InvLerp(iMin, iMax, v);
	return Lerp(oMin, oMax, t);
}