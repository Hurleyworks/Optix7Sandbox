#pragma once

struct PixelBuffer
{
	PixelBuffer()
	{
		spec.width = DEFAULT_DESKTOP_WINDOW_WIDTH;
		spec.height = DEFAULT_DESKTOP_WINDOW_HEIGHT;
		spec.channels = 3; // FIXME make a default

		int pixelCount = spec.width * spec.height;
		uint8Pixels.resize(spec.channels, pixelCount);

		// just make it black
		std::memset(uint8Pixels.data(), 0, pixelCount * spec.channels);
	}

	~PixelBuffer()
	{
		reset();
	}

	int getPixelCount() const { return spec.width * spec.height; }

	// move only
	PixelBuffer(const PixelBuffer& other) = delete;
	PixelBuffer& operator = (PixelBuffer other) = delete;

	PixelBuffer(PixelBuffer&& other) noexcept
	{
		spec = std::move(other.spec);
		uint8Pixels = std::move(other.uint8Pixels);
		floatPixels = std::move(other.floatPixels);

		other.reset();
	}

	void PixelBuffer::swap(PixelBuffer& other)
	{
		std::swap(spec, other.spec);
		std::swap(uint8Pixels, other.uint8Pixels);
		std::swap(floatPixels, other.floatPixels);
	}

	PixelBuffer& operator = (const PixelBuffer&& other)
	{
		spec = std::move(other.spec);
		uint8Pixels = std::move(other.uint8Pixels);
		floatPixels = std::move(other.floatPixels);

		return *this;
	}

	void reset()
	{
		uint8Pixels.resize(0, 0);
		floatPixels.resize(0, 0);
	}

    ImageInfo spec;
	MatrixXc uint8Pixels;
	MatrixXf floatPixels;
};