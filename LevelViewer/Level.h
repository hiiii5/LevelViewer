#pragma once

#include "Globals.h"

class Level {
private:
	static AssetType StringToAssetType(const std::string& Str);

	void StringToMatrix(const std::string& Str, GW::MATH::GMATRIXF& Mat);
	bool ParseLevelAsset(const std::string& LevelFileDir, std::stringstream& Work, AssetType& Asset, GW::MATH::GMATRIXF& Mat, std::string& Name);

public:
	Level();
	~Level();

	void Load(const std::string& LevelFileDir);
	void Render(SHADER_MODEL_DATA& InstanceData) const;
	void Destroy();

	std::vector<Actor*> Actors;
	std::vector<Light*> Lights;
	Camera MainCamera;
};
