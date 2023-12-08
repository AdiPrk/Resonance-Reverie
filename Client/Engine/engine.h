#pragma once

class Window;
class Application;

class DogEngine {
public:
	DogEngine(unsigned int width, unsigned int height);
	~DogEngine();

	int Run(Application* app);

	Window* GetWindow() { return m_Window; }

private:
	Window* m_Window;
	Application* m_Application;
};