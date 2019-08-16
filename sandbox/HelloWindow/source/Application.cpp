#include "Jahley.h"

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		
	}

	void update() override
	{
		if (isRunning)
		{
			
		}
	}

  private:
	 
	  
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = "Hello Window App";
	settings.bgColor = Eigen::Vector4f(0.2f, 0.23f, 0.26f, 1.0f);

	return new Application(settings, true);
}
