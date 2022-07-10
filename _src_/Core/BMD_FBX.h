#ifndef BMD_FBX_H
#define BMD_FBX_H

#include "BMD.h"

class BMD_FBX : public BaseIO
{
public:
	BMD_FBX() {};
	virtual ~BMD_FBX() { Release(); };

	BOOL Unpack(const char* szSrc, const char* szDest = NULL, std::vector<std::pair<std::string, fs::path>>* find_textures = NULL, bool rename_textures = false);
	BOOL Pack(const char* szSrc, const char* szDest = NULL);

	static void SetFrameTime(double frame_time);
private:
	BOOL Release();
	BOOL FixUpBones();

	BOOL Encrypt();
	BOOL Decrypt();

	BOOL LoadBmd(const char* szSrc);
	BOOL ReadBmd();
	BOOL SaveFbx(const char* szDest, std::vector<std::pair<std::string, fs::path>>* find_textures = NULL, bool rename_textures = false);

private:
	static double FRAME_TIME;
	BMD_DATA m_data;
	struct
	{
		float  m[3][4];
		float  im[3][4];
		vec3_t WorldOrg;
	} BoneFixup[MAX_BONES];

	std::vector<std::vector<std::vector<float>>> Temp_Bone_Pos;
	std::vector<std::vector<std::vector<float>>> Temp_Bone_Rot;
	std::vector<std::vector<float>>			Temp_Lock_Pos;
	std::vector<std::vector<Vertex_t>>		Temp_Vertex;
	std::vector<std::vector<Normal_t>>		Temp_Normal;
	std::vector<std::vector<TexCoord_t>>	Temp_TexCoord;
	std::vector<std::vector<Triangle_t>>	Temp_Triangle;
};


#endif // !BMD_FBX_H
