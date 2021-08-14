#ifndef BMD_H
#define BMD_H

#define MAX_BONES    200
#define MAX_MESH     50
#define MAX_VERTICES 15000
#define MAX_MONSTER_SOUND   10//5

enum SMD_TYPE {SMD_REFERENCE = -1, SMD_ANIMATION };

struct BoneMatrix_t
{
	vec3_t* Position;
	vec3_t* Rotation;
	//vec4_t* Quaternion;

	BoneMatrix_t(vec3_t* p = NULL, vec3_t* r = NULL)
	{
		Position = p;
		Rotation = r;
		//Quaternion = NULL;
	}
} ;

struct Bone_t
{
	std::string					Name;
	short						Parent;
	char						Dummy;
	std::vector<BoneMatrix_t>	BoneMatrixes;
	//char						BoundingBox;
	//vec3_t					BoundingVertices[8];

	Bone_t()
	{
		Name = "NULL";
		Parent = -1;
		Dummy = 0;
	}
};

struct Texture_t
{
	std::string FileName;
};

struct Vertex_t
{
	short  Node;
	vec3_t Position;

	Vertex_t()
	{
		Node = -1;
		Position[0] = Position[1] = Position[2] = 0.0f;
	}
};

struct Normal_t
{
	short  Node;
	vec3_t Normal;
	short  BindVertex;

	Normal_t()
	{
		Node = -1;
		Normal[0] = Normal[1] = Normal[2] = 0.0f;
		BindVertex = -1;
	}
};

struct TexCoord_t
{
	float TexCoordU;
	float TexCoordV;

	TexCoord_t()
	{
		TexCoordU = 0.0f;
		TexCoordV = 0.0f;
	}
};

struct Triangle_t
{
	char       Polygon;
	short      VertexIndex[4];
	short      NormalIndex[4];
	short      TexCoordIndex[4]; 
	//EdgeTriangleIndex, Front, LightMapCoord, LightMapIndexes are unsed
	short      EdgeTriangleIndex[4];
	bool       Front;
	DWORD	Nothing[7];
	//TexCoord_t LightMapCoord[4]; //ver1.2
	//short      LightMapIndexes; //ver1.2

	Triangle_t()
	{
		Polygon = 3;
		VertexIndex[0] = VertexIndex[1] = VertexIndex[2] = VertexIndex[3] = 0;
		NormalIndex[0] = NormalIndex[1] = NormalIndex[2] = NormalIndex[3] = 0;
		TexCoordIndex[0] = TexCoordIndex[1] = TexCoordIndex[2] = TexCoordIndex[3] = 0;
	}
};

struct Action_t
{
	//bool			Loop;
	//float			PlaySpeed;
	short			NumAnimationKeys;
	bool			LockPositions;
	vec3_t*			Positions;

	Action_t()
	{
		//Loop = false;
		//PlaySpeed = 0.0f;
		NumAnimationKeys = 0;
		LockPositions = false;
		Positions = NULL;
	}
};

struct Mesh_t
{
	bool			NoneBlendMesh;
	short			NumVertices;
	short			NumNormals;
	short			NumTexCoords;
	short			NumTriangles;
	short			Texture;
	Vertex_t*		Vertices;
	Normal_t*		Normals;
	TexCoord_t*		TexCoords;
	Triangle_t*		Triangles;
	//TextureScript* m_csTScript;

	Mesh_t()
	{
		Vertices = NULL;
		Normals = NULL;
		TexCoords = NULL;
		Triangles = NULL;
		//m_csTScript = NULL;
		NoneBlendMesh = false;
		NumVertices = NumNormals = NumTexCoords = NumTriangles = 0;
	}

};

struct BMD_DATA
{
	BYTE			Version;
	std::string		Name;
	short			NumMeshs;
	short			NumBones;
	short			NumActions;
	std::vector<Mesh_t>			Meshs;
	std::vector<Bone_t>			Bones;
	std::vector<Action_t>		Actions;
	std::vector<Texture_t>		Textures;
	//std::vector<GLuint>			IndexTexture;

	void clear()
	{
		Version = 0;
		Name.clear();
		NumMeshs = NumBones = NumActions = 0;
		Meshs.clear();
		Bones.clear();
		Actions.clear();
		Textures.clear();
	}
};

class BMD : public BaseIO
{
public:
	BMD() { };
	virtual ~BMD() { Release(); };

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

#endif // !BMD_H
