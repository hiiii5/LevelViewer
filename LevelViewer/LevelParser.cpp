#include "Globals.h"

std::string LevelParser::LevelAsString(const char* LevelFilename) {
	std::string output;
	unsigned int stringLength = 0;
	GW::SYSTEM::GFile file;
	file.Create();

	file.GetFileSize(LevelFilename, stringLength);
	if(stringLength && +file.OpenBinaryRead(LevelFilename)) {
		output.resize(stringLength);
		file.Read(&output[0], stringLength);
	}
	else {
		std::cout << "ERROR: Level Source File \"" << LevelFilename << "\" Not Found!" << std::endl;
	}

	return output;
}

