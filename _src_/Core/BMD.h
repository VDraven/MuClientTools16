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
	BMD() {};
	virtual ~BMD() {};

	BOOL Unpack(const char* szSrc, const char* szDest);
	BOOL Pack(const char* szSrcTxt, const char* szDestBmd);

private:
	BOOL Release();
	BOOL Decrypt();
	void FixUpBones();

	BOOL LoadBmd(const char* szFile);
	BOOL ReadBmd();
	BOOL SaveSmd(const char* szDestTxt);
	BOOL WriteSmd(std::ofstream& os, short action);

	BOOL LoadSmd(const char* szSrc);
	void ReadSmd(std::ifstream& is, short action);
	BOOL SaveBmd(const char* szDest);
	void WriteBmd(const char* szDest);

private:
	BMD_DATA m_data;
};

#endif // !BMD_H
