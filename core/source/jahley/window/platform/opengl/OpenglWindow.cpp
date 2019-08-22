
// This source file was auto-generated by ClassMate++
// Created: 1 Aug 2019 10:48:43 am
// Copyright (c) 2019, HurleyWorks

#include "berserkpch.h"
#include "OpenglWindow.h"
#include "../../InputHandler.h"

static OpenglWindow & get(GLFWwindow* window) { return *reinterpret_cast<OpenglWindow*>(glfwGetWindowUserPointer(window)); }

// ctor
OpenglWindow::OpenglWindow (InputHandler & input)
	: input(input)
{	
	glfwInit();
}

// dtor
OpenglWindow::~OpenglWindow ()
{	
	glfwTerminate();

	if(textureID)
		glDeleteTextures(1, &textureID);
}

void OpenglWindow::renderEnd(bool wait)
{
	wait ? glfwWaitEvents() : glfwPollEvents();

	glfwSwapBuffers(window);
}

void OpenglWindow::renderImage(ImagePixels && pixels, const ImageInfo& spec)
{
	if (!textureID)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

		if (spec.channels == 3)
		{
			MatrixXc test;
			test.resize(3, spec.width * spec.height * spec.channels);
			uint8_t * p = test.data();
			p = pixels.data();

			glTextureStorage2D(textureID, 1, GL_RGB8, spec.width, spec.height);
			glTextureSubImage2D(textureID, 0, 0, 0, spec.width, spec.height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
		}
		else if (spec.channels == 4)
		{
			glTextureStorage2D(textureID, 1, GL_RGBA8, spec.width, spec.height);
			glTextureSubImage2D(textureID, 0, 0, 0, spec.width, spec.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		}
		
		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTextureUnit(0, textureID);
	}
	else
	{
		if (spec.channels == 3)
		{
			MatrixXc test;
			test.resize( 3, spec.width * spec.height);
			for (int i = 0; i < spec.width * spec.height; i++)
			{
				test.col(i) = Vector3c(100, 50, 0);
			}

			//std::memcpy(test.data(), pixels.data(), spec.width * spec.height * spec.channels);
			

			glTextureSubImage2D(textureID, 0, 0, 0, spec.width, spec.height, GL_RGB, GL_UNSIGNED_BYTE, test.data());
		}
		else if( spec.channels == 4)
		{
			glTextureSubImage2D(textureID, 0, 0, 0, spec.width, spec.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
		}
	}
	OpenglUtil::gl_check_error(__FILE__, __LINE__);
}

void OpenglWindow::renderBegin(const Eigen::Vector4f & clearColor)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glClearColor(clearColor.x(), clearColor.y(), clearColor.z(), clearColor.w());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw image on full screen textured triangles
	if (textureID != 0)
		quad.draw(textureID);

	OpenglUtil::gl_check_error(__FILE__, __LINE__);
}

void OpenglWindow::onWindowFocus(int focused)
{
	//LOG(DBUG) << "FOCUSED";
}

void OpenglWindow::onWindowResize(int width, int height)
{
	//LOG(DBUG) << width << "," << height;
}

void OpenglWindow::onWindowClose()
{
	input.onWindowClose();
	glfwSetWindowShouldClose(window, 1);
}

void OpenglWindow::create(const Vector2i& size, const std::string& caption,
							bool resizable, bool fullscreen, int colorBits,
							int alphaBits, int depthBits, int stencilBits,
							int nSamples, unsigned int glMajor, unsigned int glMinor)
{
	// Request a forward compatible OpenGL glMajor.glMinor core profile context.
	//  Default value is an OpenGL 3.3 core profile context. 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, nSamples);
	glfwWindowHint(GLFW_RED_BITS, colorBits);
	glfwWindowHint(GLFW_GREEN_BITS, colorBits);
	glfwWindowHint(GLFW_BLUE_BITS, colorBits);
	glfwWindowHint(GLFW_ALPHA_BITS, alphaBits);
	glfwWindowHint(GLFW_STENCIL_BITS, stencilBits);
	glfwWindowHint(GLFW_DEPTH_BITS, depthBits);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);

	if (fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		window = glfwCreateWindow(mode->width, mode->height,
			caption.c_str(), monitor, nullptr);
	}
	else
	{
		window = glfwCreateWindow(size.x(), size.y(),
			caption.c_str(), nullptr, nullptr);
	}

	if (!window)
		throw std::runtime_error("Could not create an OpenGL " +
			std::to_string(glMajor) + "." +
			std::to_string(glMinor) + " context!");

	glfwMakeContextCurrent(window);

	if (!gladInitialized)
	{
		gladInitialized = true;
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			gladInitialized = false;
			throw std::runtime_error("Could not initialize GLAD!");
		}

		glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
	}
	glfwSetWindowUserPointer(window, this);

	// window events
	glfwSetWindowFocusCallback(window, [](GLFWwindow* window, int focused) { get(window).onWindowFocus(focused); });
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {get(window).onWindowResize(width, height); });
	glfwSetWindowCloseCallback(window, [](GLFWwindow* window) { get(window).onWindowClose(); });

	// input events
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) { get(window).getInput().onCursorPos(xpos, ypos); });
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) { get(window).getInput().onMouseButtonEvent(button, action, mods);  });
	glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** filenames)
	{
		std::vector<std::string> arg(count);
		for (int i = 0; i < count; ++i)
			arg[i] = filenames[i];

		get(window).getInput().onDrop(arg);
	});
	
	quad.init(true);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	OpenglUtil::gl_check_error(__FILE__, __LINE__);
}

