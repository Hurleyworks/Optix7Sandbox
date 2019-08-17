#include "Jahley.h"

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		LOG(DBUG) << "Hello World!";	
		mace::CPU cpu;
		cpu.cpuFeatures();
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
