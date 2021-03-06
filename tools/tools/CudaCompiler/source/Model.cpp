// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 6:10:35 am
// Copyright (c) 2019, HurleyWorks

#include "Model.h"

using juce::File;
using juce::StringArray;
using juce::String;
using mace::FileServices;

void Model::onDrop(const std::vector<std::string>& fileList)
{
	for (auto p : fileList)
	{
		File f(p);
		if (f.isDirectory())
		{
			StringArray files;
			String wildCard("*.cu*");
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

