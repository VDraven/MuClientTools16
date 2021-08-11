#include "BMD.h"

struct
{
	float  m[3][4];
	float  im[3][4];
	vec3_t WorldOrg;
} BoneFixup[MAX_BONES];

vec3_t DUMMY_VEC3{ 0.0f, 0.0f,0.0f };
BoneMatrix_t DUMMY_BONE_MATRIX{ &DUMMY_VEC3 , &DUMMY_VEC3 };

std::vector<std::vector<std::vector<float>>> Temp_Bone_Pos;
std::vector<std::vector<std::vector<float>>> Temp_Bone_Rot;

std::vector<std::vector<Vertex_t>>		Temp_Vertex;
std::vector<std::vector<Normal_t>>		Temp_Normal;
std::vector<std::vector<TexCoord_t>>	Temp_TexCoord;
std::vector<std::vector<Triangle_t>>	Temp_Triangle;

void ClearTemp()
{
	Temp_Bone_Pos.clear();
	Temp_Bone_Rot.clear();

	Temp_Vertex.clear();
	Temp_Normal.clear();
	Temp_TexCoord.clear();
	Temp_Triangle.clear();
}
BOOL BMD::Unpack(const char* szSrc, const char* szDest)
{
	fs::path pDest;
	if (!szDest)
	{
		fs::path pName = fs::path(szSrc).filename().replace_extension("");
		pDest = fs::path(szSrc).parent_path().append(pName.string() + "_model").append(pName.string()+".smd");

	}

	return LoadBmd(szSrc)
		&& SaveSmd(szDest ? szDest : pDest.string().c_str());
}

BOOL BMD::Pack(const char* szSrc, const char* szDest)
{
	fs::path pDest;
	if (!szDest)
	{
		fs::path pName = fs::path(szSrc).filename().replace_extension(".smd");
		pDest = fs::path(szSrc).parent_path().parent_path().append(pName.string());
	}

	return LoadSmd(szSrc)
		&& SaveBmd(szDest ? szDest : pDest.string().c_str());
}

BOOL BMD::LoadBmd(const char* szFile)
{
	if (!Release() || !FileOpen(szFile) || !Decrypt())
		return FALSE;

	ReadBmd();

	FixUpBones();
	return TRUE;
}

void BMD::ReadBmd()
{

	size_t pos = 0;

	m_data.Name = std::string((const char*)&_buf[pos], min(32, strlen((const char*)&_buf[pos]))); pos += 32;
	m_data.NumMeshs = *(short*)&_buf[pos]; pos += 2;	assert(m_data.NumMeshs <= MAX_MESH);
	m_data.NumBones = *(short*)&_buf[pos]; pos += 2;	assert(m_data.NumBones <= MAX_BONES);
	m_data.NumActions = *(short*)&_buf[pos]; pos += 2;

	if (m_data.NumMeshs < 0) m_data.NumMeshs = 0;
	if (m_data.NumBones < 0) m_data.NumBones = 0;
	if (m_data.NumActions < 0) m_data.NumActions = 0;

	m_data.Meshs.resize(m_data.NumMeshs);
	m_data.Bones.resize(m_data.NumBones);
	m_data.Actions.resize(m_data.NumActions);
	m_data.Textures.resize(m_data.NumMeshs);
	//m_data.IndexTexture.resize(m_data.NumMeshs);

#ifdef DEBUG_LOAD_BMD
	std::cout << "[BMD] : " << m_data.Name << std::endl;
	std::cout << "\tNumMeshs : " << m_data.NumMeshs << std::endl;
	std::cout << "\tNumBones : " << m_data.NumBones << std::endl;
	std::cout << "\tNumActions : " << m_data.NumActions << std::endl;
	std::cout << "========================================" << std::endl;
#endif

	for (int i = 0; i < m_data.NumMeshs; i++)
	{
		Mesh_t* m = &m_data.Meshs[i];
		m->NumVertices = *(short*)&_buf[pos]; pos += 2;
		m->NumNormals = *(short*)&_buf[pos]; pos += 2;
		m->NumTexCoords = *(short*)&_buf[pos]; pos += 2;
		m->NumTriangles = *(short*)&_buf[pos]; pos += 2;
		m->Texture = *(short*)&_buf[pos]; pos += 2;
		m->NoneBlendMesh = false;

		if (m->NumVertices < 0) m->NumVertices = 0;
		if (m->NumNormals < 0) m->NumNormals = 0;
		if (m->NumTexCoords < 0) m->NumTexCoords = 0;
		if (m->NumTriangles < 0) m->NumTriangles = 0;

		//don't need to copy, pointing to data in _buf instead
		m->Vertices = (Vertex_t*)&_buf[pos]; pos += (m->NumVertices * sizeof(Vertex_t));
		m->Normals = (Normal_t*)&_buf[pos]; pos += (m->NumNormals * sizeof(Normal_t));
		m->TexCoords = (TexCoord_t*)&_buf[pos]; pos += (m->NumTexCoords * sizeof(TexCoord_t));
		m->Triangles = (Triangle_t*)&_buf[pos]; pos += (m->NumTriangles * sizeof(Triangle_t));
		m_data.Textures[i].FileName = std::string((const char*)&_buf[pos], min(32, strlen((const char*)&_buf[pos]))); pos += 32;

		//TextureScriptParsing skip

#ifdef DEBUG_LOAD_BMD
		std::cout << "[Mesh] : " << i << std::endl;
		std::cout << "\tNumVertices: " << m->NumVertices << std::endl;
		std::cout << "\tNumNormals: " << m->NumNormals << std::endl;
		std::cout << "\tNumTexCoords: " << m->NumTexCoords << std::endl;
		std::cout << "\tNumTriangles: " << m->NumTriangles << std::endl;
		std::cout << "\tTexture: " << m->Texture << std::endl;
		std::cout << "\tTexture Name: " << m_data.Textures[i].FileName << std::endl;
		std::cout << "========================================" << std::endl;

		std::cout << "[Vertices] Mesh : " << i << std::endl;
		std::cout << "\tNumVertices: " << m->NumVertices << std::endl;
		for (int ii = 0; ii < m->NumVertices; ii++)
		{
			std::cout << std::fixed
				<< "\t\t" << m->Vertices[ii].Node
				<< "\t" << m->Vertices[ii].Position[0]
				<< "\t" << m->Vertices[ii].Position[1]
				<< "\t" << m->Vertices[ii].Position[2]
				<< std::endl;
		}
		std::cout << "________________________________________" << std::endl;

		std::cout << "[Normals] Mesh : " << i << std::endl;
		std::cout << "\tNumNormals: " << m->NumNormals << std::endl;
		for (int ii = 0; ii < m->NumNormals; ii++)
		{
			std::cout << std::fixed
				<< "\t\t" << m->Normals[ii].Node
				<< "\t" << m->Normals[ii].BindVertex
				<< "\t" << m->Normals[ii].Normal[0]
				<< "\t" << m->Normals[ii].Normal[1]
				<< "\t" << m->Normals[ii].Normal[2]
				<< std::endl;
		}
		std::cout << "________________________________________" << std::endl;

		std::cout << "[TexCoords] Mesh : " << i << std::endl;
		std::cout << "\tNumTexCoords: " << m->NumTexCoords << std::endl;
		for (int ii = 0; ii < m->NumTexCoords; ii++)
		{
			std::cout << std::fixed
				<< "\t\t" << m->TexCoords[ii].TexCoordU
				<< "\t" << m->TexCoords[ii].TexCoordV
				<< std::endl;
		}
		std::cout << "________________________________________" << std::endl;

		std::cout << "[Triangles] Mesh : " << i << std::endl;
		std::cout << "\tNumTriangles: " << m->NumTriangles << std::endl;
		for (int ii = 0; ii < m->NumTriangles; ii++)
		{
			std::cout << std::fixed
				<< "\t\t" << m->Triangles[ii].VertexIndex[0]
				<< "\t" << m->Triangles[ii].VertexIndex[1]
				<< "\t" << m->Triangles[ii].VertexIndex[2]
				<< "\t|"
				<< "\t" << m->Triangles[ii].NormalIndex[0]
				<< "\t" << m->Triangles[ii].NormalIndex[1]
				<< "\t" << m->Triangles[ii].NormalIndex[2]
				<< "\t|"
				<< "\t" << m->Triangles[ii].TexCoordIndex[0]
				<< "\t" << m->Triangles[ii].TexCoordIndex[1]
				<< "\t" << m->Triangles[ii].TexCoordIndex[2]
				<< std::endl;
			std::cout << std::endl;
	}
		std::cout << "========================================" << std::endl;

#endif
}

	for (int i = 0; i < m_data.NumActions; i++)
	{
		Action_t* a = &m_data.Actions[i];
		//a->Loop = false;

		a->NumAnimationKeys = *(short*)&_buf[pos]; pos += 2;
		if (a->NumAnimationKeys < 0) a->NumAnimationKeys = 0;

		a->LockPositions = *(bool*)&_buf[pos]; pos += 1;
		if (a->LockPositions)
		{
			//don't need to copy, pointing to data in _buf instead
			a->Positions = (vec3_t*)&_buf[pos]; pos += (a->NumAnimationKeys * sizeof(vec3_t));
		}
		else
		{
			a->Positions = NULL;
		}

#ifdef DEBUG_LOAD_BMD
		std::cout << "[Actions] : " << i << std::endl;
		std::cout << "\tNumAnimationKeys : " << a->NumAnimationKeys << std::endl;
		std::cout << "\tLockPositions : " << a->LockPositions << std::endl;
		if (a->LockPositions)
		{
			for (int ii = 0; ii < a->NumAnimationKeys; ii++)
			{
				std::cout << std::fixed
					<< "\t\t" << a->Positions[ii][0]
					<< "\t" << a->Positions[ii][1]
					<< "\t" << a->Positions[ii][2]
					<< std::endl;
	}
		}
		std::cout << "========================================" << std::endl;

#endif
	}

	for (int i = 0; i < m_data.NumBones; i++)
	{
		Bone_t* b = &m_data.Bones[i];
		b->Dummy = *(char*)&_buf[pos]; pos += 1;
		if (!b->Dummy)
		{
			b->Name = std::string((const char*)&_buf[pos], min(32, strlen((const char*)&_buf[pos]))); pos += 32;
			b->Parent = *(short*)&_buf[pos]; pos += 2;
			b->BoneMatrixes.resize(m_data.NumActions);
			for (int j = 0; j < m_data.NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];
				//don't need to copy, pointing to data in _buf instead
				bm->Position = (vec3_t*)&_buf[pos]; pos += (m_data.Actions[j].NumAnimationKeys * sizeof(vec3_t));
				bm->Rotation = (vec3_t*)&_buf[pos]; pos += (m_data.Actions[j].NumAnimationKeys * sizeof(vec3_t));

				//AngleQuaternion skip
			}
		}
		else
		{
			b->Name = "Null " + std::to_string(i);
			b->Parent = -1;
			//for (int j = 0; j < m_data.NumActions; j++)
			b->BoneMatrixes.push_back(DUMMY_BONE_MATRIX);
		}

#ifdef DEBUG_LOAD_BMD
		std::cout << "[Bones] : " << i << std::endl;
		std::cout << "\tDummy : " << ((int)b->Dummy) << std::endl;
		if (!b->Dummy)
		{
			std::cout << "\tName : " << b->Name << std::endl;
			std::cout << "\tParent : " << b->Parent << std::endl;
			for (int ii = 0; ii < m_data.NumActions; ii++)
			{
				for (int jj = 0; jj < m_data.Actions[ii].NumAnimationKeys; jj++)
				{
					std::cout << std::fixed
						<< "\t\t" << b->BoneMatrixes[ii].Position[jj][0]
						<< "\t" << b->BoneMatrixes[ii].Position[jj][1]
						<< "\t" << b->BoneMatrixes[ii].Position[jj][2]
						<< "\t" << b->BoneMatrixes[ii].Rotation[jj][0]
						<< "\t" << b->BoneMatrixes[ii].Rotation[jj][1]
						<< "\t" << b->BoneMatrixes[ii].Rotation[jj][2]
						<< std::endl;
		}
	}
		}
		std::cout << "========================================" << std::endl;
#endif
	}
}

BOOL BMD::Release()
{
	ClearTemp();
	m_data.clear();
	_buf.clear();
	return TRUE;
}


void BMD::FixUpBones()
{
	for (int i = 0; i < m_data.NumBones; i++)
	{
		Bone_t* b = &m_data.Bones[i];
		vec3_t Angle;
		vec3_t Pos;
		//if (!b->Dummy)
		//{
			BoneMatrix_t* bm = &b->BoneMatrixes[0];
			Angle[0] = bm->Rotation[0][0] * (180.0f / Q_PI);
			Angle[1] = bm->Rotation[0][1] * (180.0f / Q_PI);
			Angle[2] = bm->Rotation[0][2] * (180.0f / Q_PI);
			Pos[0] = bm->Position[0][0];
			Pos[1] = bm->Position[0][1];
			Pos[2] = bm->Position[0][2];

		//}
		//else
		//{
		//	Angle[0] = Angle[1] = Angle[2] = 0.0f;
		//	Pos[0] = Pos[1] = Pos[2] = 0.0f;
		//}

		// calc true world coord.
		if (b->Parent >= 0 && b->Parent < m_data.NumBones)
		{
			BoneMatrix_t* bm = &b->BoneMatrixes[0];
			float m[3][4];
			vec3_t p;

			AngleMatrix(Angle, m);
			R_ConcatTransforms(BoneFixup[b->Parent].m, m, BoneFixup[i].m);
			AngleIMatrix(Angle, m);
			R_ConcatTransforms(m, BoneFixup[b->Parent].im, BoneFixup[i].im);

			VectorTransform(Pos, BoneFixup[b->Parent].m, p);
			VectorAdd(p, BoneFixup[b->Parent].WorldOrg, BoneFixup[i].WorldOrg);
		}
		else
		{
			AngleMatrix(Angle, BoneFixup[i].m);
			AngleIMatrix(Angle, BoneFixup[i].im);
			VectorCopy(Pos, BoneFixup[i].WorldOrg);
		}
	}
}

BOOL BMD::Decrypt()
{
	if (_buf.size() < 8) return FALSE;

	BYTE ver = _buf[3];
	if (ver == 0xC)
	{
		size_t size = *(size_t*)&_buf[4];
		_buf.erase(_buf.begin(), _buf.begin() + 8);

		if (_buf.size() < size) return FALSE;

		MapFileDecrypt(&_buf[0], size);
	}
	else if (ver == 0xE)
	{
		size_t size = *(size_t*)&_buf[4];
		_buf.erase(_buf.begin(), _buf.begin() + 8);

		if (_buf.size() < size) return FALSE;

		ModulusDecrypt(_buf);
	}
	else //ver == 0xA
	{
		_buf.erase(_buf.begin(), _buf.begin() + 4);
	}

	m_data.Version = ver;

	return TRUE;
}

BOOL BMD::SaveSmd(const char* szDest)
{
	assert(szDest);
	fs::path pFileName = fs::path(szDest).filename().replace_extension("");
	fs::path pFile(szDest);
	Utls::CreateParentDir(pFile);

	fs::path pAnimDir(pFile);
	//pAnimDir = pAnimDir.replace_extension("").string() + "_anims";
	pAnimDir = pAnimDir.parent_path().append("anims");

	assert(m_data.NumActions > 0 && m_data.Actions[0].NumAnimationKeys > 0);

	//if (m_data.NumMeshs > 0)	player.bmd has no mesh (only animations)
	{
		std::ofstream os1(pFile);
		WriteSmd(os1, SMD_REFERENCE);
		os1.close();
	}

	if (m_data.NumActions == 1 && m_data.Actions[0].NumAnimationKeys == 1)
	{
		return TRUE;
	}

	for (short action = 0; action < m_data.NumActions; action++)
	{
		fs::path pFileAnim(pAnimDir);
		pFileAnim.append(pFileName.string());					// ./example_anims -> ./example_anims/example
		pFileAnim += "_" + std::to_string(action + 1) + ".smd";	// ./example_anims/example -> ./example_anims/example_1.smd
		Utls::CreateParentDir(pFileAnim);

		std::ofstream os2(pFileAnim);
		if (!os2.is_open())
		{
			//std::cout << "Error: Failed to write the smd file: " << pFileAnim << '\n';
			continue;
		}

		WriteSmd(os2, action);

		os2.close();
	}

	return TRUE;
}

void BMD::WriteSmd(std::ofstream& os, short action)
{
	assert(action >= SMD_REFERENCE && action < m_data.NumActions);

	SMD_TYPE SmdType;
	if (action == SMD_REFERENCE)
	{
		SmdType = SMD_REFERENCE;
		action = 0;
	}
	else
	{
		SmdType = SMD_ANIMATION;
	}

	Action_t* a = &m_data.Actions[action];

	os << "version 1" << std::endl;

	//================nodes=====================
	//		[Index Name Parent]
	
	os << "nodes" << std::endl;

	for (int i = 0; i < m_data.Bones.size(); i++)
	{
		Bone_t* b = &m_data.Bones[i];
		os << i << " \"" << b->Name << "\" " << b->Parent << std::endl;
	}

	os << "end" << std::endl;
	
	//================skeleton=====================
	//		[time x]
	//		[Node Position[3] Rotation[3]]
	
	os << "skeleton" << std::endl;

	for (int i = 0; i < a->NumAnimationKeys; i++)
	{
		//Only include 1 frame of animation to define the default pose in smd reference
		if (SmdType == SMD_REFERENCE && i > 0)
			break;

		os << "time " << i << std::endl;

		for (int j = 0; j < m_data.Bones.size(); j++)
		{
			Bone_t* b = &m_data.Bones[j];

			if (!b->Dummy)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[action];

				os << j << ' ' << std::fixed;
				os << bm->Position[i][0] << ' ';
				os << bm->Position[i][1] << ' ';
				os << bm->Position[i][2] << ' ';

				os << bm->Rotation[i][0] << ' ';
				os << bm->Rotation[i][1] << ' ';
				os << bm->Rotation[i][2] << std::endl;
			}
			else
			{
				os << j << ' ' << std::fixed;
				os << 0.0f << ' ';
				os << 0.0f << ' ';
				os << 0.0f << ' ';

				os << 0.0f << ' ';
				os << 0.0f << ' ';
				os << 0.0f << std::endl;
			}
		}

	}
	os << "end" << std::endl;

	if (SmdType == SMD_ANIMATION) return;

	//================triangles=====================
	//		[TextureName]
	//		[Node Position[3] Normal[3] TexCoord[2]]
			
	os << "triangles" << std::endl;

	assert(m_data.NumMeshs == m_data.Meshs.size() && m_data.NumMeshs == m_data.Textures.size());
	for (int i = 0; i < m_data.NumMeshs; i++)
	{
		Mesh_t* m = &m_data.Meshs[i];
		for (int j = 0; j < m->NumTriangles; j++)
		{
			os << m_data.Textures[i].FileName << std::endl;

			Triangle_t* t = &m->Triangles[j];
			for (int k = 0; k < 3; k++)	// k < t->Polygon
			{
				vec3_t p;
				short vertex_index = t->VertexIndex[k];
				short vertex_node = m->Vertices[vertex_index].Node;
				short normal_index = t->NormalIndex[k];
				short normal_node = m->Normals[normal_index].Node;
				short texcoord_index = t->TexCoordIndex[k];
				short node = vertex_node;

				os << std::fixed
					<< node << ' ';

				VectorTransform(m->Vertices[vertex_index].Position, BoneFixup[vertex_node].m, p);
				VectorAdd(p, BoneFixup[vertex_node].WorldOrg, p);

				os << p[0] << ' '
					<< p[1] << ' '
					<< p[2] << ' ';

				VectorTransform(m->Normals[normal_index].Normal, BoneFixup[normal_node].m, p);
				VectorNormalize(p);

				os << p[0] << ' '
					<< p[1] << ' '
					<< p[2] << ' ';

				os << m->TexCoords[texcoord_index].TexCoordU << ' '
					<< 1.0f - m->TexCoords[texcoord_index].TexCoordV << std::endl;
			}
		}
	}

	os << "end" << std::endl;
}

BOOL BMD::LoadSmd(const char* szSrc)
{
	if (!szSrc) return FALSE;

	fs::path pFileName = fs::path(szSrc).filename().replace_extension("");
	fs::path pAnimDir = fs::path(szSrc).parent_path().append("anims");

	std::ifstream is(szSrc);
	if (!is.is_open())
	{
		std::cout << "Error: Failed to read the txt file: " << szSrc << '\n';
		return FALSE;
	}

	Release();
	m_data.Name = pFileName.string();
	BMD::ReadSmd(is, SMD_REFERENCE);
	is.close();

	for (short action = 0; ; action++)
	{
		fs::path pFileAnim(pAnimDir);
		pFileAnim.append(pFileName.string());
		pFileAnim += "_" + std::to_string(action + 1) + ".smd";

		if (!fs::exists(pFileAnim))
			break;

		std::ifstream is2(pFileAnim);
		BMD::ReadSmd(is2, action);
		is2.close();
	}
	
	return TRUE;
}

void BMD::ReadSmd(std::ifstream& is, short action)
{
	SMD_TYPE SmdType;
	if (action == SMD_REFERENCE)
	{
		SmdType = SMD_REFERENCE;
		action = 0;
	}
	else
	{
		if (action == 0)
		{
			//clear the 1st frame obtained from ref smd
			Temp_Bone_Pos.clear();
			Temp_Bone_Rot.clear();
		}
		SmdType = SMD_ANIMATION;
	}

	m_data.Actions.resize(action + 1);
	m_data.NumActions = m_data.Actions.size();

	Action_t* a = &m_data.Actions[action];
	a->NumAnimationKeys = 0;
	a->LockPositions = false;
	a->Positions = NULL;

	std::string line;

	//================nodes=====================

	if (SmdType == SMD_REFERENCE)
	{
		while (std::getline(is, line))
		{
			if (line == "nodes")
				break;
		}

		while (std::getline(is, line))
		{
			if (line == "end")
				break;

			short node;
			short parent;
			char name[256];
			sscanf(line.c_str(), "%hd \"%[^\"]%*c %hd", &node, name, &parent);
			//std::cout << node << " " << name << " " << parent << std::endl;
			Bone_t b;

			//b.Dummy = (name[0] == 'N' && name[1] == 'u' && name[2] == 'l' && name[3] == 'l' && parent == -1) ? 1 : 0;
			b.Dummy = (parent == -1 && node != 0) ? 1 : 0;
			b.Parent = parent;
			b.Name = std::string(name, min(32, strlen(name)));

			assert(node == m_data.Bones.size());
			m_data.Bones.push_back(b);
			m_data.NumBones = m_data.Bones.size();
		}
	}

	//================skeleton=====================

	while (std::getline(is, line))
	{
		if (line == "skeleton")
			break;
	}

	Temp_Bone_Pos.resize(m_data.Bones.size());
	Temp_Bone_Rot.resize(m_data.Bones.size());

	for (int i = 0; i < m_data.Bones.size(); i++)
	{
		m_data.Bones[i].BoneMatrixes.resize(action + 1);
		Temp_Bone_Pos[i].resize(m_data.NumActions);
		Temp_Bone_Rot[i].resize(m_data.NumActions);

	}

	while (std::getline(is, line))
	{
		if (line == "end")
			break;

		if (line[0] == 't' && line[1] == 'i' && line[2] == 'm' && line[3] == 'e')	//time x
		{
			short key;
			sscanf(line.c_str(), "time %hd", &key);
			assert(key == a->NumAnimationKeys);
			a->NumAnimationKeys++;
		}
		else
		{
			short node;
			vec3_t pos;
			vec3_t rot;
			sscanf(line.c_str(), "%hd %f %f %f %f %f %f", &node, &pos[0], &pos[1], &pos[2], &rot[0], &rot[1], &rot[2]);
			if (node >= 0 && node < m_data.Bones.size())
			{
				Temp_Bone_Pos[node][action].push_back(pos[0]);
				Temp_Bone_Pos[node][action].push_back(pos[1]);
				Temp_Bone_Pos[node][action].push_back(pos[2]);

				Temp_Bone_Rot[node][action].push_back(rot[0]);
				Temp_Bone_Rot[node][action].push_back(rot[1]);
				Temp_Bone_Rot[node][action].push_back(rot[2]);

				m_data.Bones[node].BoneMatrixes[action].Position = (vec3_t*) Temp_Bone_Pos[node][action].data();
				m_data.Bones[node].BoneMatrixes[action].Rotation = (vec3_t*) Temp_Bone_Rot[node][action].data();
			}
		}
	}

	if (SmdType == SMD_REFERENCE) 
	{
		FixUpBones();
	}

	//================Triangles=====================



	if (SmdType == SMD_REFERENCE)
	{
		while (std::getline(is, line))
		{
			if (line == "triangles")
				break;
		}

		while (std::getline(is, line))
		{
			if (line == "end")
				break;

			char buffer[256];
			sscanf(line.c_str(), "%s", &buffer);	//texture name, no space ' '
			std::string texture(buffer, min(32, strlen(buffer)));

			short texture_index = -1;
			for (int i = 0; i < m_data.Textures.size(); i++)
			{
				if (texture == m_data.Textures[i].FileName)
				{
					texture_index = i;
					break;
				}
			}

			if (texture_index == -1)
			{
				texture_index = m_data.Textures.size();
				m_data.Textures.emplace_back();
				m_data.Textures[m_data.Textures.size() - 1].FileName = texture;

				m_data.Meshs.emplace_back();
				m_data.Meshs[m_data.Meshs.size()-1].Texture = texture_index;
				m_data.NumMeshs++;

				Temp_Vertex.emplace_back();
				Temp_Normal.emplace_back();
				Temp_TexCoord.emplace_back();
				Temp_Triangle.emplace_back();
			}

			Mesh_t* m = NULL;
			short mesh_index = -1;
			for (int i = 0; i < m_data.Meshs.size(); i++) 
			{
				if (m_data.Meshs[i].Texture == texture_index)
				{
					m = &m_data.Meshs[i];
					mesh_index = i;
					break;
				}
			}
			assert(m != NULL && mesh_index != -1);
			

			Triangle_t triangle;
			for (int k = 0; k < 3; k++)
			{
				std::getline(is, line);
				short node;	//bone_index
				Vertex_t v;
				Normal_t n;
				TexCoord_t t;
				sscanf(line.c_str(), "%hd %f %f %f %f %f %f %f %f", &node, &v.Position[0], &v.Position[1], &v.Position[2], &n.Normal[0], &n.Normal[1], &n.Normal[2], &t.TexCoordU, &t.TexCoordV);
				t.TexCoordV = 1 - t.TexCoordV;

				vec3_t p;
				// move vertex position to object space.
				VectorSubtract(v.Position, BoneFixup[node].WorldOrg, p);
				VectorTransform(p, BoneFixup[node].im, v.Position);

				// move normal to object space.
				VectorCopy(n.Normal, p);
				VectorTransform(p, BoneFixup[node].im, n.Normal);
				VectorNormalize(n.Normal);

				short vertex_index = -1;
				for (int i = Temp_Vertex[mesh_index].size() - 1; i >= 0; i--)
				{
					if (VectorCompare(v.Position, Temp_Vertex[mesh_index][i].Position)) //&& node == Temp_Vertex[mesh_index][i].Node
					{
						vertex_index = i;
						break;
					}
				}

				if (vertex_index == -1)
				{
					vertex_index = Temp_Vertex[mesh_index].size();
					v.Node = node;
					Temp_Vertex[mesh_index].push_back(v);
				}

				short normal_index = -1;
				for (int i = Temp_Normal[mesh_index].size() - 1; i >= 0 ; i--)
				{
					if (VectorCompare(n.Normal, Temp_Normal[mesh_index][i].Normal))// && node == Temp_Normal[mesh_index][i].Node
					{
						normal_index = i;
						break;
					}
				}

				if (normal_index == -1)
				{
					normal_index = Temp_Normal[mesh_index].size();
					n.Node = node;
					Temp_Normal[mesh_index].push_back(n);
				}

				short texcoord_index = -1;
				for (int i = Temp_TexCoord[mesh_index].size() - 1; i >= 0; i--)
				{
					if (fabs(t.TexCoordU - Temp_TexCoord[mesh_index][i].TexCoordU) < EQUAL_EPSILON
						&& fabs(t.TexCoordV - Temp_TexCoord[mesh_index][i].TexCoordV) < EQUAL_EPSILON)
					{
						texcoord_index = i;
						break;
					}
				}

				if (texcoord_index == -1)
				{
					texcoord_index = Temp_TexCoord[mesh_index].size();
					Temp_TexCoord[mesh_index].push_back(t);
				}

				triangle.VertexIndex[k] = vertex_index;
				triangle.NormalIndex[k] = normal_index;
				triangle.TexCoordIndex[k] = texcoord_index;
			}
			triangle.Polygon = 3;
			Temp_Triangle[mesh_index].push_back(triangle);

			m->NumVertices = Temp_Vertex[mesh_index].size();
			m->NumNormals = Temp_Normal[mesh_index].size();
			m->NumTexCoords = Temp_TexCoord[mesh_index].size();
			m->NumTriangles = Temp_Triangle[mesh_index].size();

			m->Vertices = Temp_Vertex[mesh_index].data();
			m->Normals = Temp_Normal[mesh_index].data();
			m->TexCoords = Temp_TexCoord[mesh_index].data();
			m->Triangles = Temp_Triangle[mesh_index].data();
		}
	}
}


BOOL BMD::SaveBmd(const char* szDest)
{
	if (!szDest) return FALSE;

	Utls::CreateParentDir(szDest);

	WriteBmd(szDest);

	return TRUE;
}

void BMD::WriteBmd(const char* szDest)
{
	size_t size = 0;
	size += 4;	//"BMD."
	size += 38;	//header
	for (int i = 0; i < m_data.NumMeshs; i++)
	{
		Mesh_t* m = &m_data.Meshs[i];
		size += 10;
		size += m->NumVertices * sizeof(Vertex_t);
		size += m->NumNormals * sizeof(Normal_t);
		size += m->NumTexCoords * sizeof(TexCoord_t);
		size += m->NumTriangles * sizeof(Triangle_t);
		size += 32;
	}

	for (int i = 0; i < m_data.NumActions; i++)
	{
		Action_t* a = &m_data.Actions[i];
		size += 3;
		if (a->LockPositions)
			size += a->NumAnimationKeys * sizeof(vec3_t);
	}

	for (int i = 0; i < m_data.NumBones; i++)
	{
		Bone_t* b = &m_data.Bones[i];
		size += 1;
		if (!b->Dummy)
		{
			size += 32;
			size += 2;
			for (int j = 0; j < m_data.NumActions; j++)
			{
				size += 2 * m_data.Actions[j].NumAnimationKeys * sizeof(vec3_t);
			}
		}
	}

	_buf.resize(size);

	size_t pos = 0;
	size_t sz = 0;
	_buf[pos] = 'B'; pos += 1;
	_buf[pos] = 'M'; pos += 1;
	_buf[pos] = 'D'; pos += 1;
	_buf[pos] = 0xA; pos += 1;	//version 0xA (no crypt)

	//header
	memcpy(&_buf[pos], m_data.Name.data(), min(32, 1 + m_data.Name.length())); pos += 32;
	*(short*)&_buf[pos] = m_data.NumMeshs; pos += 2;
	*(short*)&_buf[pos] = m_data.NumBones; pos += 2;
	*(short*)&_buf[pos] = m_data.NumActions; pos += 2;

	for (int i = 0; i < m_data.NumMeshs; i++)
	{
		Mesh_t* m = &m_data.Meshs[i];
		*(short*)&_buf[pos] = m->NumVertices; pos += 2;
		*(short*)&_buf[pos] = m->NumNormals; pos += 2;
		*(short*)&_buf[pos] = m->NumTexCoords; pos += 2;
		*(short*)&_buf[pos] = m->NumTriangles; pos += 2;
		*(short*)&_buf[pos] = m->Texture; pos += 2;

		sz = m->NumVertices * sizeof(Vertex_t);
		memcpy(&_buf[pos], m->Vertices, sz); pos += sz;
		sz = m->NumNormals * sizeof(Normal_t);
		memcpy(&_buf[pos], m->Normals, sz); pos += sz;
		sz = m->NumTexCoords * sizeof(TexCoord_t);
		memcpy(&_buf[pos], m->TexCoords, sz); pos += sz;
		sz = m->NumTriangles * sizeof(Triangle_t);
		memcpy(&_buf[pos], m->Triangles, sz); pos += sz;
		
		memcpy(&_buf[pos], m_data.Textures[i].FileName.data(), min(32, 1 + m_data.Textures[i].FileName.length())); pos += 32;
	}

	for (int i = 0; i < m_data.NumActions; i++)
	{
		Action_t* a = &m_data.Actions[i];
		*(short*)&_buf[pos] = a->NumAnimationKeys; pos += 2;
		*(bool*)&_buf[pos] = a->LockPositions; pos += 1;

		if (a->LockPositions && a->Positions)
		{
			sz = a->NumAnimationKeys * sizeof(vec3_t);
			memcpy(&_buf[pos], a->Positions, sz); pos += sz;
		}
	}

	for (int i = 0; i < m_data.NumBones; i++)
	{

		Bone_t* b = &m_data.Bones[i];
		*(char*)&_buf[pos] = b->Dummy; pos += 1;

		if (!b->Dummy)
		{
			memcpy(&_buf[pos], b->Name.data(), min(32, 1 + b->Name.length())); pos += 32;
			*(short*)&_buf[pos] = b->Parent; pos += 2;
			for (int j = 0; j < m_data.NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];

				sz = m_data.Actions[j].NumAnimationKeys * sizeof(vec3_t);
				memcpy(&_buf[pos], bm->Position, sz); pos += sz;
				memcpy(&_buf[pos], bm->Rotation, sz); pos += sz;
			}
		}
	}

	FileWrite(szDest);
}