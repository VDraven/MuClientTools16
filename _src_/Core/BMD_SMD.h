#ifndef BMD_SMD_H
#define BMD_SMD_H

#include "BMD.h"

class BMD_SMD : public BaseIO
{
public:
	BMD_SMD() { };
	virtual ~BMD_SMD() { Release(); };

	BOOL Unpack(const char* szSrc, const char* szDest);
	BOOL Pack(const char* szSrc, const char* szDest);

	static BOOL LoadLockPostionData(const char* fname = "LockPositionData.txt");
private:
	static BOOL GetLockPosition(std::string & name, short action);
	static BOOL SetLockPosition(std::string& name, short action);
	inline static std::string LockPositionData_FileName;
	inline static std::unordered_multimap <std::string, short> LockPositionData;
private:
	BOOL Release();
	BOOL FixUpBones();

	BOOL Encrypt();
	BOOL Decrypt();

	BOOL LoadBmd(const char* szSrc);
	BOOL ReadBmd();
	BOOL SaveSmd(const char* szDest);
	BOOL Bmd2Smd(std::ofstream& os, short action);

	BOOL LoadSmd(const char* szSrc);
	BOOL ReadSmd(std::ifstream& is, short action);
	BOOL SaveBmd(const char* szDest);
	BOOL Smd2Bmd(BYTE version = 0xA);

private:
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

#endif // !BMD_SMD_H
