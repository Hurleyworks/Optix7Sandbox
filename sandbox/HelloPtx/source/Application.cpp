#include "Jahley.h"
#include "PtxHandler.h"

class Application : public Jahley::App
{

  public:
	Application(bool windowApp = false)
		: Jahley::App(windowApp)
	{
		const std::string cudaFolder("../../../scratch/cuda");
		const std::string ptxFolder("../../../scratch/ptx");
		
		try
		{
			PtxHandler handler;
			handler.findCudaFiles(cudaFolder);
			if (handler.getCudaFileCount())
			{
				handler.createPtxFiles(ptxFolder, true);
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
