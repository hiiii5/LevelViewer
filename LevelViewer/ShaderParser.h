#pragma once
#include <iostream>
#include <string>
#include "../gateware/Gateware.h"

class ShaderParser
{
public:
	/**
	 * \brief Attempts to load a hlsl shader from disk.
	 * \param ShaderFilePath Filepath of the hlsl file.
	 * \return String of file data.
	 */
	static std::string ShaderAsString(const char* ShaderFilePath) {
        std::string output;
        unsigned int stringLength = 0;
        GW::SYSTEM::GFile file; file.Create();
        file.GetFileSize(ShaderFilePath, stringLength);
        if (stringLength && +file.OpenBinaryRead(ShaderFilePath)) {
            output.resize(stringLength);
            file.Read(&output[0], stringLength);
        }
        else
            std::cout << "ERROR: Shader Source File \"" << ShaderFilePath << "\" Not Found!" << std::endl;
        return output;
	}
};

