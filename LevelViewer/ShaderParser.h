#pragma once

#include "Globals.h"

class ShaderParser
{
public:
	/**
	 * \brief Attempts to load a hlsl shader from disk.
	 * \param ShaderFilePath Filepath of the hlsl file.
	 * \return String of file data.
	 */
	static std::string ShaderAsString(const char* ShaderFilePath);
};

