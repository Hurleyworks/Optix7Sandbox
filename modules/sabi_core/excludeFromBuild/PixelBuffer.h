#pragma once

struct PixelBuffer
{
	PixelBuffer()
	{
		spec.width = DEFAULT_DESKTOP_WINDOW_WIDTH;
		spec.height = DEFAULT_DESKTOP_WINDOW_HEIGHT;
		spec.channels = 3; // FIXME

		int pixelCount = spec.width * spec.height;
		uint8Pixels.resize(spec.channels, pixelCount);

		for (int i = 0; i < pixelCount; i++)
		{
			uint8Pixels.col(i) = Vector3c(200, 0, 0);
		}
	}

	ImageInfo spec;
	MatrixXc uint8Pixels;
	MatrixXf floatPixels;
	
};
