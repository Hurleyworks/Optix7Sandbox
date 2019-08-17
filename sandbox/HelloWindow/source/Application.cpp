#include "Jahley.h"

using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;

const std::string APP_NAME = "HelloWindow";

class Application : public Jahley::App
{

  public:
	Application(DesktopWindowSettings settings, bool windowApp = false)
		: Jahley::App(settings, windowApp)
	{
	}

	void update() override
	{
		if (!imageIsLoaded)
		{
			// the one and only JahleyBlue
			const String fileName = getResourcePath(APP_NAME) + "/images/jahley.jpg";
			loadImage(fileName.toStdString()); 

			imageIsLoaded = true;
		}
	}

	void loadImage(const std::string& path)
	{
		handleType image(stbi_load(path.c_str(), &width, &height, &channels, force_channels), stbi_image_free);
		if (image)
		{
			ImageInfo spec;
			spec.width = width;
			spec.height = height;
			spec.channels = channels;

			ImagePixels pixels;
			int bytes = width * height * channels * sizeof(uint8_t);
			pixels.resize(bytes);
			std::memcpy(pixels.data(), image.get(), bytes);

			window->renderImage(std::move(pixels), spec);
		}
		else
		{
			LOG(CRITICAL) << "Failed to load image from relative path: " << path;
		}
	}

  private:
	  bool imageIsLoaded = false;
	  int force_channels = 0;
	  int width, height, channels;
};

Jahley::App* Jahley::CreateApplication()
{
	DesktopWindowSettings settings;
	settings.name = APP_NAME + std::to_string(settings.width) + " x " + std::to_string(settings.height);
	settings.bgColor = Eigen::Vector4f(0.2f, 0.23f, 0.26f, 1.0f);

	return new Application(settings, true);
}
