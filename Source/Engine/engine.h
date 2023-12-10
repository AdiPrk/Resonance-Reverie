#pragma once

namespace Dog {

	class Window;
	class Application;
	class Scene;

	class DogEngine {
	public:
		DogEngine(unsigned int width, unsigned int height);
		~DogEngine();

		int Run(Scene* startScene);

		Window* GetWindow() { return m_Window; }

	private:
		Window* m_Window;
	};

}