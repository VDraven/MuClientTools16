#ifndef BMD_FBX_H
#define BMD_FBX_H

#include "BMD.h"

class BMD_FBX : public BaseIO
{
public:
	BMD_FBX() {};
	virtual ~BMD_FBX() { Release(); };

	BOOL Unpack(const char* szSrc, const char* szDest = NULL, bool find_textures = false, bool rename_textures = false, bool export_normals = false);
	BOOL Pack(const char* szSrc, const char* szDest = NULL);

	static void SetFrameTime(double frame_time);
	static void SetRootPath(fs::path root_path);

private:
	BOOL Release();
	BOOL FixUpBones();

	BOOL Encrypt();
	BOOL Decrypt();

	BOOL LoadBmd(const char* szSrc);
	BOOL ReadBmd();
	BOOL SaveFbx(const char* szDest, std::unordered_map<std::string, fs::path>* textures = NULL, bool rename_textures = false, bool export_normals = false);
	BOOL FindTexture(fs::path pDir, std::unordered_map<std::string, fs::path>* textures);

private:
	static fs::path ROOT_PATH;
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
