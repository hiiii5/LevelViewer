#pragma once
#include "Globals.h"

struct Shader {
	Shader() : VsFile(""), PsFile(""), VertShader(nullptr), PixShader(nullptr) {}
	Shader(const std::string VsFile, const std::string PsFile) : VsFile(VsFile), PsFile(PsFile), VertShader(nullptr), PixShader(nullptr) {}

	std::string VsFile, PsFile;
	VkShaderModule VertShader, PixShader;
};

