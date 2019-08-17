#include "Jahley.h"
#include "PtxHandler.h"

const std::string APP_NAME = "HelloPtx";

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings = DesktopWindowSettings(), bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
		String resourceFolder = getResourcePath(APP_NAME);
		const std::string cudaFolder = (resourceFolder + "/cuda").toStdString();;
		const std::string ptxFolder = (resourceFolder + "/ptx").toStdString();;

		File cuda(cudaFolder);
		File ptx(ptxFolder);

		if (!cuda.exists())
			LOG(CRITICAL) << "Can't find cuda input folder at: " << cudaFolder;

		if (!ptx.exists())
			LOG(CRITICAL) << "Can't find ptx output folder at: " << ptxFolder;

		try
		{
			PtxHandler handler;
			handler.findCudaFiles(cudaFolder);
			if (handler.getCudaFileCount())
			{
				handler.createPtxFiles(ptxFolder, true);
			}
			else
			{
				LOG(CRITICAL) << "No cuda files found!";
			}
		}
		catch (std::exception& e)
		{
			LOG(CRITICAL) << e.what();
		}
		catch (...)
		{
			LOG(CRITICAL) << "Caught unknown exception!";
		}
		
	}

  private:
	
};

Jahley::App* Jahley::CreateApplication()
{
	return new Application();
}
