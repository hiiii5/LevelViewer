#include "Globals.h"

std::string ShaderParser::ShaderAsString(const char* ShaderFilePath)
{
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
