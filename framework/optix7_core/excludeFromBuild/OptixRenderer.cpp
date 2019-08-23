// This source file was auto-generated by ClassMate++
// Created: 18 Aug 2019 4:00:14 pm
// Copyright (c) 2019, HurleyWorks

using sabi::PixelBuffer;

// ctor
OptixRenderer::OptixRenderer (const PropertyService& properties)
	: properties(properties)
{	
}

// dtor
OptixRenderer::~OptixRenderer ()
{	
}

void OptixRenderer::render(CameraHandle& camera)
{
	Eigen::Vector4f bg = properties.renderProps->getVal<Eigen::Vector4f>(RenderKey::BackgroundColor);
	Vector3c c(bg.x() * 255, bg.y() * 255, bg.z() * 255);

	PixelBuffer & buffer = camera->getPixelBuffer();

	// Release mode only. Eigen is DOG SLOW here in Debug mode for this operation for some reaon
	#ifdef NDEBUG
	int pixelCount = buffer.getPixelCount();
	for (int i = 0; i < pixelCount; i++)
	{
		buffer.uint8Pixels.col(i) = c;
	}
	#endif 
}

