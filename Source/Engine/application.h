#pragma once

namespace Dog {

	class Application {
	public:
		// Main update function for the application. Takes in delta time.
		virtual void Update(float dt) = 0;

		// Main render function. Takes in delta time, current time, and interpolation factor
		virtual void Render(float dt, float ct, float itf) = 0;
	};

}