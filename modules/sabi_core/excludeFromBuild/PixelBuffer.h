#pragma once

struct PixelBuffer
{
	PixelBuffer()
	{
		spec.width = DEFAULT_DESKTOP_WINDOW_WIDTH;
		spec.height = DEFAULT_DESKTOP_WINDOW_HEIGHT;
		spec.channels = DEFAULT_DESKTOP_WINDOW_CHANNELS; 

		uint8Pixels.resize(spec.channels, getPixelCount());

		// just make it black
		std::memset(uint8Pixels.data(), 0, byteCountUint8());
	}

	~PixelBuffer()
	{
		reset();
	}

	void init(const ImageInfo& info)
	{
		spec.width = info.width;
		spec.height = info.height;
		spec.channels = info.channels;

		uint8Pixels.resize(spec.channels, getPixelCount());

		// just make it black
		std::memset(uint8Pixels.data(), 0, byteCountUint8());
	}

	int getPixelCount() const { return spec.width * spec.height; }
	int byteCountUint8() const { return spec.width * spec.height * spec.channels * sizeof(uint8_t); } 
	int bytecountFloat() const { return spec.width * spec.height * spec.channels * sizeof(float); }

	// move only!
	PixelBuffer(const PixelBuffer& other) = delete;
	PixelBuffer& operator = (const PixelBuffer& other) = delete;

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