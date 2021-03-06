// This header file was auto-generated by ClassMate++
// Created: 4 Aug 2019 4:20:04 pm
// Copyright (c) 2019, HurleyWorks

#pragma once

#include <sabi_core/sabi_core.h>
#include <stb/include/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nanovg.h>
#include <nanogui/glutil.h>

using nanogui::GLShader;
using Eigen::Vector2f;

namespace OpenglUtil
{
	// from Polymer https://github.com/ddiakopoulos/polymer/tree/dev/lib-polymer
	static bool gEnableGLDebugOutputErrorBreakpoints = false;
	inline void has_gl_extension(std::vector<std::pair<std::string, bool>>& extension_list)
	{
		int numExtensions = 0;
		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

		// Loop through all extensions
		for (int i = 0; i < numExtensions; ++i)
		{
			auto* ext = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
			for (auto& requiredExt : extension_list)
			{
				if (requiredExt.first == ext) requiredExt.second = true;
			}
		}
	}
	inline std::string gl_src_to_str(GLenum source)
	{
		switch (source)
		{
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
			case GL_DEBUG_SOURCE_API: return "API";
			default: return "UNKNOWN";
		}
	}

	inline std::string gl_enum_to_str(GLenum type)
	{
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR: return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATION";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
			case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
			case GL_DEBUG_TYPE_OTHER: return "OTHER";
			default: return "UNKNOWN";
		}
	}

	inline std::string gl_severity_to_str(GLenum severity)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "UNKNOWN";
		}
	}

	static void __stdcall gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if (type != GL_DEBUG_TYPE_ERROR) return;
		auto sourceStr = gl_src_to_str(source);
		auto typeStr = gl_enum_to_str(type);
		auto severityStr = gl_severity_to_str(severity);
		std::cout << "gl_debug_callback: " << sourceStr << ", " << severityStr << ", " << typeStr << " , " << id << ", " << message << std::endl;
		if ((type == GL_DEBUG_TYPE_ERROR) && (gEnableGLDebugOutputErrorBreakpoints)) __debugbreak();
	}

	inline void gl_check_error(const char* file, int32_t line)
	{
#if defined(_DEBUG) || defined(DEBUG)
		GLint error = glGetError();
		if (error)
		{
			const char* errorStr = 0;
			switch (error)
			{
				case GL_INVALID_ENUM: errorStr = "GL_INVALID_ENUM"; break;
				case GL_INVALID_VALUE: errorStr = "GL_INVALID_VALUE"; break;
				case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break;
				case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMORY"; break;
				default: errorStr = "unknown error"; break;
			}
			LOG(CRITICAL) << "GL error : " <<  file << "::" << line << "::" <<  errorStr;
			error = 0;
		}
#endif
	}

	inline size_t gl_size_bytes(GLenum type)
	{
		switch (type)
		{
			case GL_UNSIGNED_BYTE: return sizeof(uint8_t);
			case GL_UNSIGNED_SHORT: return sizeof(uint16_t);
			case GL_UNSIGNED_INT: return sizeof(uint32_t);
			default: throw std::logic_error("unknown element type"); break;
		}
	}

	struct FullScreenQuad
	{
		FullScreenQuad()
		{}
		
		~FullScreenQuad()
		{
			if(shader)
				delete shader;
		}

		void init(bool flipVertical)
		{
			// from Polymer https://github.com/ddiakopoulos/polymer/tree/dev/lib-polymer
			static const char vertexShader[] = R"(#version 330
                layout(location = 0) in vec3 position;
                layout(location = 1) in vec2 uvs;
                uniform mat4 u_mvp = mat4(1);
                out vec2 texCoord;
                void main()
                {
                    texCoord = uvs;
                    gl_Position = u_mvp * vec4(position.xy, 0.0, 1.0);
    
                }
            )";

			static const char fragmentShader[] = R"(#version 330
                uniform sampler2D s_texture;
                in vec2 texCoord;
                out vec4 f_color;
                void main()
                {
                    vec4 sample = texture(s_texture, texCoord);
                    f_color = vec4(sample.rgba);
                }
            )";

			shader = new GLShader();
			shader->init("FullScreenQuad", vertexShader, fragmentShader);

			MatrixXf positions(2, 6);   // 6 vertices for 2 triangles
			MatrixXf uvs(2, 6);			// 6 uv coords
			MatrixXu indices(3, 2);		// 2 triangles

			const Vector2f verts[6] = { { -1.0f, -1.0f },{ 1.0f, -1.0f},{ -1.0f, 1.0f },{ -1.0f, 1.0f },{ 1.0f, -1.0f },{ 1.0f, 1.0f } };
			for (int i = 0; i < 6; i++)
			{
				positions.col(i) = verts[i];
			}

			const Vector2f texcoords[6] = { { 0, 0 },{ 1, 0 },{ 0, 1 },{ 0, 1 },{ 1, 0 },{ 1, 1 } };
			for (int i = 0; i < 6; i++)
			{
				uvs.col(i) = texcoords[i];
			}
		
			if (flipVertical)
			{
				// just flip the Y component to flip the image vertically
				uvs.col(0) = Vector2f(0, 1);
				uvs.col(1) = Vector2f(1, 1);
				uvs.col(2) = Vector2f(0, 0);
				uvs.col(3) = Vector2f(0, 0);
				uvs.col(4) = Vector2f(1, 1);
				uvs.col(5) = Vector2f(1, 0);
			}

			const Vector3u faces[2] = { { 0, 1, 2 },{ 3, 4, 5 } };
			for (int i = 0; i < 2; i++)
			{
				indices.col(i) = faces[i];
			}

			shader->bind();
			shader->uploadAttrib("position", positions);
			shader->uploadAttrib("uvs", uvs);
			shader->uploadAttrib("indices", indices);;
		}

		void draw(const GLuint texture_handle)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture_handle);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			shader->bind();
			shader->drawIndexed(GL_TRIANGLES, 0, 2);

			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_BLEND);
		}

	private:
		GLShader* shader = nullptr;
	};

	inline const char* getMouseButtonAction(int action)
	{
		switch (action)
		{
		case GLFW_PRESS:
			return "pressed";
		case GLFW_RELEASE:
			return "released";
		case GLFW_REPEAT:
			return "repeated";
		}

		return "caused unknown action";
	}

	inline const char* getMouseButtonName(int button)
	{
		switch (button)
		{
			case GLFW_MOUSE_BUTTON_LEFT:
				return "left";
			case GLFW_MOUSE_BUTTON_RIGHT:
				return "right";
			case GLFW_MOUSE_BUTTON_MIDDLE:
				return "middle";
			default:
			{
				static char name[16];
				snprintf(name, sizeof(name), "%i", button);
				return name;
			}
		}
	}

	inline const char* getModifierNames(int mods)
	{
		static char name[512];

		if (mods == 0)
			return " no mods";

		name[0] = '\0';

		if (mods & GLFW_MOD_SHIFT)
			strcat(name, " shift");
		if (mods & GLFW_MOD_CONTROL)
			strcat(name, " control");
		if (mods & GLFW_MOD_ALT)
			strcat(name, " alt");
		if (mods & GLFW_MOD_SUPER)
			strcat(name, " super");
		if (mods & GLFW_MOD_CAPS_LOCK)
			strcat(name, " capslock-on");
		if (mods & GLFW_MOD_NUM_LOCK)
			strcat(name, " numlock-on");

		return name;
	}
}