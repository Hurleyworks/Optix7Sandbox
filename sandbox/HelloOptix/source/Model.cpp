// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#include "Model.h"
#include <stb_image.h>

using juce::File;
using juce::StringArray;
using juce::String;
using mace::FileServices;
using sabi::InputEvent;

// ctor
Model::Model ()
{	
}

// dtor
Model::~Model ()
{	
}

void Model::loadPrimitive(PrimitiveType type)
{
	LOG(DBUG) << PrimitiveType(type).toString();
}


void Model::loadImage(const std::string& path, ImagePixels& image, ImageInfo& spec)
{
	int force_channels = 0;
	int width, height, channels;

	handleType pixels(stbi_load(path.c_str(), &width, &height, &channels, force_channels), stbi_image_free);
	if (pixels)
	{
		spec.width = width;
		spec.height = height;
		spec.channels = channels;
	
		int bytes = width * height * channels * sizeof(uint8_t);
		image.resize(bytes);
		std::memcpy(image.data(), pixels.get(), bytes);
	}
}

void Model::onDrop(const std::vector<std::string>& fileList)
{
	for (auto p : fileList)
	{
		File f(p);
		if (f.isDirectory())
		{
			StringArray files;
			String wildCard("*.*");
			FileServices::getFiles(f.getFullPathName(), files, wildCard);

			for (auto path : files)
			{
				File f(path);
				std::string filename = f.getFullPathName().toStdString();
				LOG(DBUG) << filename;
			}
		}
		else if (f.existsAsFile())
		{
			std::string filename = f.getFullPathName().toStdString();
			LOG(DBUG) << filename;
		}
	}
}

