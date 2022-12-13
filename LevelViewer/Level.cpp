#include "Globals.h"

AssetType Level::StringToAssetType(const std::string& Str) {
	AssetType assetType = UnknownAsset;
	static std::unordered_map<std::string, AssetType> const table = { {"MESH",MeshAsset}, {"LIGHT",LightAsset} };
	auto it = table.find(Str);
	if (it != table.end()) {
		return it->second;
	}

	return UnknownAsset;
}

void Level::StringToMatrix(const std::string& Str, GW::MATH::GMATRIXF& Mat) {
	/* Example identity matrix data.
	 <Matrix 4x4 ( 1.0000,  0.0000, 0.0000, 0.0000)
				 ( 0.0000,  1.0000, 0.0000, 0.0000)
				 ( 0.0000,  0.0000, 1.0000, 0.0000)
				 ( 0.0000,  0.0000, 0.0000, 1.0000)>
	 */
	std::string cur = Str;

	// Get all the rows as stripped strings.
	unsigned first = cur.find('(');
	unsigned last = cur.find(')');
	std::string row1 = cur.substr(first + 1, last-first - 1); // Strip all character but the coordinates, spaces, and commas.

	cur = cur.substr(last+2);
	first = cur.find('(');
	last = cur.find(')');
	std::string row2 = cur.substr(first + 1, last - first - 1); // Strip all character but the coordinates, spaces, and commas.

	cur = cur.substr(last + 2);
	first = cur.find('(');
	last = cur.find(')');
	std::string row3 = cur.substr(first + 1, last - first - 1); // Strip all character but the coordinates, spaces, and commas.

	cur = cur.substr(last + 2);
	first = cur.find('(');
	last = cur.find(')');
	std::string row4 = cur.substr(first + 1, last - first - 1); // Strip all character but the coordinates, spaces, and commas.

	// Remove any whitespace from the row strings.
	row1.erase(std::remove(row1.begin(), row1.end(), ' '), row1.end());
	row2.erase(std::remove(row2.begin(), row2.end(), ' '), row2.end());
	row3.erase(std::remove(row3.begin(), row3.end(), ' '), row3.end());
	row4.erase(std::remove(row4.begin(), row4.end(), ' '), row4.end());

	// Convert each substring of the rows to floats for the final matrix.
	float row11, row12, row13, row14,
		  row21, row22, row23, row24,
		  row31, row32, row33, row34,
		  row41, row42, row43, row44;

	// TODO: Fix this tediousness, but in the interest of time.
	// Walk to the end of the grouping, by stripping off substring broken by a ','
	last = row1.find(',');
	row11 = std::stof(row1.substr(0, last));
	row1.erase(0, last + 1); // Remove the float + ,

	last = row1.find(',');
	row12 = std::stof(row1.substr(0, last));
	row1.erase(0, last + 1); // Remove the float + ,

	last = row1.find(',');
	row13 = std::stof(row1.substr(0, last));
	row1.erase(0, last + 1); // Remove the float + ,

	// The last string is the final float in the row.
	row14 = std::stof(row1);

	// TODO: Fix this tediousness, but in the interest of time.
	// Walk to the end of the grouping, by stripping off substring broken by a ','
	last = row2.find(',');
	row21 = std::stof(row2.substr(0, last));
	row2.erase(0, last + 1); // Remove the float + ,

	last = row2.find(',');
	row22 = std::stof(row2.substr(0, last));
	row2.erase(0, last + 1); // Remove the float + ,

	last = row2.find(',');
	row23 = std::stof(row2.substr(0, last));
	row2.erase(0, last + 1); // Remove the float + ,

	// The last string is the final float in the row.
	row24 = std::stof(row2);

	// TODO: Fix this tediousness, but in the interest of time.
	// Walk to the end of the grouping, by stripping off substring broken by a ','
	last = row3.find(',');
	row31 = std::stof(row3.substr(0, last));
	row3.erase(0, last + 1); // Remove the float + ,

	last = row3.find(',');
	row32 = std::stof(row3.substr(0, last));
	row3.erase(0, last + 1); // Remove the float + ,

	last = row3.find(',');
	row33 = std::stof(row3.substr(0, last));
	row3.erase(0, last + 1); // Remove the float + ,

	// The last string is the final float in the row.
	row34 = std::stof(row3);

	// TODO: Fix this tediousness, but in the interest of time.
	// Walk to the end of the grouping, by stripping off substring broken by a ','
	last = row4.find(',');
	row41 = std::stof(row4.substr(0, last));
	row4.erase(0, last + 1); // Remove the float + ,

	last = row4.find(',');
	row42 = std::stof(row4.substr(0, last));
	row4.erase(0, last + 1); // Remove the float + ,

	last = row4.find(',');
	row43 = std::stof(row4.substr(0, last));
	row4.erase(0, last + 1); // Remove the float + ,

	// The last string is the final float in the row.
	row44 = std::stof(row4);

	Mat.row1.data[0] = row11;
	Mat.row1.data[1] = row12;
	Mat.row1.data[2] = row13;
	Mat.row1.data[3] = row14;

	Mat.row2.data[0] = row21;
	Mat.row2.data[1] = row22;
	Mat.row2.data[2] = row23;
	Mat.row2.data[3] = row24;

	Mat.row3.data[0] = row31;
	Mat.row3.data[1] = row32;
	Mat.row3.data[2] = row33;
	Mat.row3.data[3] = row34;

	Mat.row4.data[0] = row41;
	Mat.row4.data[1] = row42;
	Mat.row4.data[2] = row43;
	Mat.row4.data[3] = row44;
}

bool Level::ParseLevelAsset(const std::string& LevelFileDir, std::stringstream& Work, AssetType& Asset, GW::MATH::GMATRIXF& Mat, std::string& Name) {
	std::string curLine;
	// Get the assets type.
	std::getline(Work, curLine);
	auto strAsset = curLine.erase(curLine.size() - 1);
	Asset = StringToAssetType(strAsset);
	if(Asset == UnknownAsset) {
		std::cout << "ERROR: Unknown asset type: \"" << strAsset << "\" found in \"" << LevelFileDir << "\"";
		return false;
	}

	// Get the assets name.
	std::getline(Work, Name);
	Name.erase(Name.length() - 1); // Remove /r at end of line.

	// Get the assets transform.
	// The next 4 lines are the matrix data.
	std::string matData, temp;
	for (int i = 0; i < 4; ++i) {
		std::getline(Work, temp);
		matData += temp;
	}
	//GW::MATH::GMatrix::IdentityF(Mat);
	StringToMatrix(matData, Mat);
	return true;
}

Level::Level() {
	Actors = {};
	Lights = {};
	MainCamera = {};
}

Level::~Level() {
	
}

void Level::Load(const std::string& LevelFileDir) {
	auto ret = LevelParser::LevelAsString(LevelFileDir.c_str());
	std::stringstream work(ret);
	std::string curLine;

	// Get all version data from the level exporter.
	std::getline(work, curLine);
	auto verMinor = std::stoi(curLine.substr(curLine.size() - 2, 1));
	auto verMajor = std::stoi(curLine.substr(curLine.size() - 4, 1));

	// Until the end of file is found, parse every level asset.
	while (work.peek() != std::string::npos) {
		AssetType asset;
		GW::MATH::GMATRIXF assetMat{};
		std::string name = "";
		if (!ParseLevelAsset(LevelFileDir, work, asset, assetMat, name)) {
			std::cout << "ERROR: Unable to parse level asset.";
		}

		// Initialize the new asset based on the data.
		switch (asset) {
		case MeshAsset:
		{
			// Create a new static mesh actor.
			auto a = new Actor(("../" + name + ".h2b").c_str());
			if (a->MeshComp->Version[0] == 0 &&
				a->MeshComp->Version[1] == 0 &&
				a->MeshComp->Version[2] == 0 &&
				a->MeshComp->Version[3] == 0) {
				a->Destroy();// Don't add an invalid mesh.
				break;
			}

			a->Name = name;
			a->Transform = assetMat;
			Actors.emplace_back(a);
		}

		break;
		case LightAsset:
		{
			auto l = new Light();
			Lights.emplace_back(l);
		}

		break;
		case UnknownAsset: break;
		}
	}

	for (const auto actor : Actors) {
		for (unsigned int i = 0; i < actor->MeshComp->MaterialCount; i++) {
			Renderer::Get().InstanceData.Attributes[i] = actor->MeshComp->Materials[i].attrib;
		}
	}

	// Prepare mesh for display, this is the default cubemesh TODO: REMOVE
	for (const auto actor : Actors) {
		if (!actor) continue;
		if (!actor->MeshComp) continue; // Don't try to compile null meshes.
		if (actor->MeshComp->Version[0] == 0 &&
			actor->MeshComp->Version[1] == 0 &&
			actor->MeshComp->Version[2] == 0 &&
			actor->MeshComp->Version[3] == 0) continue; // Don't try to compile if the mesh version is wrong, because it is likely other data is too.
		Renderer::Get().CompileMesh(actor->MeshComp);
	}
}

void Level::Render(SHADER_MODEL_DATA& InstanceData) const {
	for (unsigned int i = 0; i < Actors.size(); ++i) {
		//Renderer::Get().InstanceData.Transforms[i] = Actors[i]->Transform;
		Actors[i]->Render();
	}
}

void Level::Destroy() {
	for (const auto actor : Actors) {
		actor->Destroy();
	}
	Actors.clear();

	for (const auto light : Lights) {
		delete light;
	}
	Lights.clear();
}

