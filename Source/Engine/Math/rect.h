#pragma once

#include <Engine/PCH/pch.h>

namespace Dog {

	struct Rect
	{
		float left, right, top, bottom;
		float width, height;

		Rect() = default;
		Rect(float l, float t, float w, float h)
			: left(l), right(l + w), top(t), bottom(t + h), width(w), height(h) {}

		bool contains(const glm::vec2& p) const
		{
			return p.x >= left && p.x <= right && p.y >= top && p.y <= bottom;
		}

		bool contains(const Rect& r) const
		{
			return r.left >= left && r.right <= right && r.top >= top && r.bottom <= bottom;
		}

		bool overlaps(const Rect& r) const
		{
			return left < r.right&& right > r.left && top < r.bottom&& bottom > r.top;
		}

		bool bordersOverlap(const Rect& r) const
		{
			return left <= r.right && right >= r.left && top <= r.bottom && bottom >= r.top;
		}

		void SetScale(const glm::vec2& scale) {
			width = scale.x;
			height = scale.y;
			right = left + width;
			bottom = top + height;
		}

		void UpdateBoundsFromTopLeft() {
			right = left + width;
			bottom = top + height;
		}

		void PrintBounds() const {
			printf("Left: %f, Top: %f, Right: %f, Bottom: %f\n", left, top, right, bottom);
		}
	};

}