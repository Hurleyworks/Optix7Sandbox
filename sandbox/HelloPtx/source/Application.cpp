#include "Jahley.h"
#include "PtxHandler.h"

using juce::File;

class Application : public Jahley::App
{

  public:
	Application(bool windowApp = false)
		: Jahley::App(windowApp)
	{
		const std::string cudaFolder("../../../scratch/cuda");
		const std::string ptxFolder("../../../scratch/ptx");

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
