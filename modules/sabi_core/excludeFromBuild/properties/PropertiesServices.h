#pragma once

struct PropertyService
{

	PropertyService() = default;
	~PropertyService() = default;

	void init()
	{
		imageProps = std::make_shared<ImagingProperties>();
		worldProps = std::make_shared<WorldProperties>();
		
	}

	
	ImagingPropsRef imageProps = nullptr;
	WorldPropsRef worldProps = nullptr;
	
};