#include "Jahley.h"

class Application : public Jahley::App
{

  public:
	Application(bool windowApp = false)
		: Jahley::App(windowApp)
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
