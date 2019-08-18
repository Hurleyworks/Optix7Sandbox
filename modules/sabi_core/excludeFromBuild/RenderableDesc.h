// This header file was auto-generated by ClassMate++
// Created: 16 Sep 2017 5:37:16 pm
// Copyright (c) 2017, HurleyWorks

#pragma once

struct RenderableDesc
{
	
	// full path to file containing mesh, materials etc
	std::string fullFilePath = INVALID_PATH;

	void resetToDefault()
	{
		
		fullFilePath = INVALID_PATH;

	}

	void debug()
	{
		
		LOG(DBUG) << "FullFilePath: " << fullFilePath;
		
	}
};