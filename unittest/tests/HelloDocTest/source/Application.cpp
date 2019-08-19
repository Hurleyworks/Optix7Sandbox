#include "Jahley.h"

const std::string APP_NAME = "HelloDocTest";

#ifdef CHECK
#undef CHECK
#endif

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

TEST_CASE("[string] testing std::string")
{
	std::string a("omg");
	CHECK(a == "omg");
}

TEST_CASE("[math] basic stuff")
{
	CHECK(6 > 5);
	CHECK(6 > 7);
}

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		doctest::Context().run();
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
