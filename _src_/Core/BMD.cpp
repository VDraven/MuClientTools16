#include "BMD.h"

//#define _DEBUG_BMD_

#ifdef _DEBUG_BMD_
#define DEBUG_BMD 1
#define PRINT_DEBUG_BMD(msg) std::cout << msg << std::endl
#else
#define DEBUG_BMD 0
#define PRINT_DEBUG_BMD(msg)
#endif

vec3_t DUMMY_VEC3{ 0.0f, 0.0f,0.0f };
BoneMatrix_t DUMMY_BONE_MATRIX{ &DUMMY_VEC3 , &DUMMY_VEC3 };

//========================================================================================

BOOL BMD::Unpack(const char* szSrc, const char* szDest)
{
	if (!szSrc) return FALSE;

	auto CheckBmd = [](const char* szInputPath)->BOOL
	{
		std::ifstream is(szInputPath, std::ios::in | std::ios::binary);
		if (!is.is_open())
			return FALSE;
		char BMD[3]{};
		for (int i = 0; i < 3 && !is.eof(); i++)
			is >> BMD[i];
		is.close();
		return (BMD[0] == 'B' && BMD[1] == 'M' && BMD[2] == 'D');
	};

	if(!CheckBmd(szSrc)) return FALSE;

	PRINT_DEBUG("Unpacking " << szSrc);

	if (!szDest)
	{
		std::string sName = fs::path(szSrc).filename().replace_extension("").string();
		szDest = fs::path(szSrc).parent_path().append(sName + "_model").append(sName + ".smd").string().c_str();
	}

	return LoadBmd(szSrc)
		&& SaveSmd(szDest);
}

BOOL BMD::Pack(const char* szSrc, const char* szDest)
{
	if (!szSrc) return FALSE;
	PRINT_DEBUG("Packing " << szSrc);

	if (!szDest)
	{
		std::string sName = fs::path(szSrc).filename().replace_extension(".bmd").string();
		szDest = fs::path(szSrc).parent_path().parent_path().append(sName).string().c_str();
	}

	return LoadSmd(szSrc)
		&& SaveBmd(szDest);
}

//========================================================================================

BOOL BMD::LoadLockPostionData(const char* fname)
{
	LockPositionData.clear();
	LockPositionData_FileName = fname;
	if (!fs::exists(fname))
	{
		std::ofstream os(fname);
		if (!os.is_open()) return FALSE;
		os << "//action (tab) bmd_name" << std::endl;
		os.close();
		return TRUE;
	}

	std::ifstream is(fname);
	if (!is.is_open()) return FALSE;
	
	std::string line;
	while (std::getline(is, line))
	{
		if (line.length() >= 2 && line[0] == '/' && line[1] == '/')
			continue;

		short action;
		char buf[256];
		sscanf_s(line.c_str(), "%hd\t%s", &action, buf, 256);

		std::string bmd_name(buf);
		std::transform(bmd_name.begin(), bmd_name.end(), bmd_name.begin(), ::tolower);

		if (action > 0 && !bmd_name.empty())
		{
			LockPositionData.insert(std::make_pair(bmd_name, action));
			//PRINT_DEBUG(bmd_name << " " << action);
		}
	}

	is.close();
	return TRUE;
}

BOOL BMD::GetLockPosition(std::string& name, short action)
{
	auto range = LockPositionData.equal_range(name);
	for (auto& it = range.first; it != range.second; it++) 
	{
		if (it->second == action)
			return TRUE;
	}

	return FALSE;
}

BOOL BMD::SetLockPosition(std::string& name, short action)
{
	auto range = LockPositionData.equal_range(name);
	for (auto& it = range.first; it != range.second; it++)
	{
		if (it->second == action)
			return FALSE;
	}

	std::ofstream os(LockPositionData_FileName, std::ofstream::app);
	if (!os.is_open()) return FALSE;

	LockPositionData.insert(std::make_pair(name, action));
	os << action << "\t" << name << std::endl;
	os.close();

	return TRUE;
}

//========================================================================================

BOOL BMD::Release()
{
	Temp_Bone_Pos.clear();
	Temp_Bone_Rot.clear();
	Temp_Lock_Pos.clear();
	Temp_Vertex.clear();
	Temp_Normal.clear();
	Temp_TexCoord.clear();
	Temp_Triangle.clear();

	m_data.clear();
	_buf.clear();

	return TRUE;
}

BOOL BMD::FixUpBones()
{
	for (int i = 0; i < m_data.NumBones; i++)
	{
		Bone_t* b = &m_data.Bones[i];
		vec3_t Angle;
		vec3_t Pos;

		BoneMatrix_t* bm = &b->BoneMatrixes[0];
		Angle[0] = bm->Rotation[0][0] * (180.0f / Q_PI);
		Angle[1] = bm->Rotation[0][1] * (180.0f / Q_PI);
		Angle[2] = bm->Rotation[0][2] * (180.0f / Q_PI);
		Pos[0] = bm->Position[0][0];
		Pos[1] = bm->Position[0][1];
		Pos[2] = bm->Position[0][2];

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

	return TRUE;
}

//========================================================================================

BOOL BMD::Encrypt()
{
	if (_buf.empty()) return FALSE;

	BYTE ver = m_data.Version;
	if (ver == 0xC)
	{
		size_t size = _buf.size();

		MapFileEncrypt(&_buf[0], size);

		_buf.insert(_buf.begin(), 8, 0);
		*(size_t*)&_buf[4] = size;
	}
	else if (ver == 0xE)
	{
		size_t size = _buf.size();

		ModulusEncrypt(_buf);

		_buf.insert(_buf.begin(), 8, 0);
		*(size_t*)&_buf[4] = size;
	}
	else if (ver = 0xA)
	{
		_buf.insert(_buf.begin(), 4, 0);
	}
	else
	{
		PRINT_DEBUG("[ERROR] Unknown BMD version : " << ((int)ver));
		return FALSE;
	}

	_buf[0] = 'B';
	_buf[1] = 'M';
	_buf[2] = 'D';
	_buf[3] = ver;

	return TRUE;
}

BOOL BMD::Decrypt()
{
	if (_buf.size() < 8) return FALSE;

	if (_buf[0] != 'B' || _buf[1] != 'M' || _buf[2] != 'D') return FALSE;

	BYTE ver = _buf[3];
	if (ver == 0xC)
	{
		size_t size = *(size_t*)&_buf[4];
		_buf.erase(_buf.begin(), _buf.begin() + 8);

		if (_buf.size() < size) return FALSE;	////(_buf.size() == size)

		MapFileDecrypt(&_buf[0], size);
	}
	else if (ver == 0xE)
	{
		size_t size = *(size_t*)&_buf[4];
		_buf.erase(_buf.begin(), _buf.begin() + 8);

		if (_buf.size() < size) return FALSE;	//(_buf.size() == size)

		ModulusDecrypt(_buf);
	}
	else if (ver == 0xA)
	{
		_buf.erase(_buf.begin(), _buf.begin() + 4);
	}
	else
	{
		PRINT_DEBUG("[ERROR] Unknown BMD version : " << ((int)ver));
		return FALSE;
	}

	m_data.Version = ver;

	return TRUE;
}

//========================================================================================

BOOL BMD::LoadBmd(const char* szSrc)
{
	Release();
	m_data.Name = fs::path(szSrc).filename().replace_extension("").string();

	return FileOpen(szSrc)
		&& Decrypt()
		&& ReadBmd();
}

BOOL BMD::ReadBmd()
{
	if (_buf.size() < 38) return FALSE;

	size_t pos = 0;

	//m_data.Name = std::string((const char*)&_buf[pos], min(32, strlen((const char*)&_buf[pos]))); pos += 32;
	pos += 32;

	m_data.NumMeshs = *(short*)&_buf[pos]; pos += 2;
	if (m_data.NumMeshs < 0 || m_data.NumMeshs > MAX_MESH)
	{
		PRINT_DEBUG("[ERROR] m_data.NumMeshs < 0 || m_data.NumMeshs > MAX_MESH");
		return FALSE;
	}

	m_data.NumBones = *(short*)&_buf[pos]; pos += 2;
	if (m_data.NumBones < 0 || m_data.NumBones > MAX_BONES)
	{
		PRINT_DEBUG("[ERROR] m_data.NumBones < 0 || m_data.NumBones > MAX_BONES");
		return FALSE;
	}

	m_data.NumActions = *(short*)&_buf[pos]; pos += 2;
	if (m_data.NumActions < 1)
	{
		PRINT_DEBUG("[ERROR] m_data.NumActions < 1");
		return FALSE;
	}
	m_data.Meshs.resize(m_data.NumMeshs);
	m_data.Bones.resize(m_data.NumBones);
	m_data.Actions.resize(m_data.NumActions);
	m_data.Textures.resize(m_data.NumMeshs);
	//m_data.IndexTexture.resize(m_data.NumMeshs);

	if (DEBUG_BMD)
	{
		PRINT_DEBUG_BMD("[BMD] : " << m_data.Name);
		PRINT_DEBUG_BMD("\tNumMeshs : " << m_data.NumMeshs);
		PRINT_DEBUG_BMD("\tNumBones : " << m_data.NumBones);
		PRINT_DEBUG_BMD("\tNumActions : " << m_data.NumActions);
		PRINT_DEBUG_BMD("========================================");
	}

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
		//TextureScriptParsing skip

		if (_buf.size() < pos)
		{
			PRINT_DEBUG("[ERROR] Corrupted Bmd File");
			return FALSE;
		}

		m_data.Textures[i].FileName = std::string((const char*)&_buf[pos], min(32, strlen((const char*)&_buf[pos]))); pos += 32;

		if (DEBUG_BMD)
		{
			PRINT_DEBUG_BMD("[Mesh] : " << i);
			PRINT_DEBUG_BMD("\tNumVertices: " << m->NumVertices);
			PRINT_DEBUG_BMD("\tNumNormals: " << m->NumNormals);
			PRINT_DEBUG_BMD("\tNumTexCoords: " << m->NumTexCoords);
			PRINT_DEBUG_BMD("\tNumTriangles: " << m->NumTriangles);
			PRINT_DEBUG_BMD("\tTexture: " << m->Texture);
			PRINT_DEBUG_BMD("\tTexture Name: " << m_data.Textures[i].FileName);
			PRINT_DEBUG_BMD("========================================");

			PRINT_DEBUG_BMD("[Vertices] Mesh : " << i);
			PRINT_DEBUG_BMD("\tNumVertices: " << m->NumVertices);
			for (int ii = 0; ii < m->NumVertices; ii++)
			{
				PRINT_DEBUG_BMD(std::fixed
					<< "\t\t" << m->Vertices[ii].Node
					<< "\t" << m->Vertices[ii].Position[0]
					<< "\t" << m->Vertices[ii].Position[1]
					<< "\t" << m->Vertices[ii].Position[2]);
			}
			PRINT_DEBUG_BMD("________________________________________");

			PRINT_DEBUG_BMD("[Normals] Mesh : " << i);
			PRINT_DEBUG_BMD("\tNumNormals: " << m->NumNormals);
			for (int ii = 0; ii < m->NumNormals; ii++)
			{
				PRINT_DEBUG_BMD(std::fixed
					<< "\t\t" << m->Normals[ii].Node
					<< "\t" << m->Normals[ii].BindVertex
					<< "\t" << m->Normals[ii].Normal[0]
					<< "\t" << m->Normals[ii].Normal[1]
					<< "\t" << m->Normals[ii].Normal[2]);
			}
			PRINT_DEBUG_BMD("________________________________________");

			PRINT_DEBUG_BMD("[TexCoords] Mesh : " << i);
			PRINT_DEBUG_BMD("\tNumTexCoords: " << m->NumTexCoords);
			for (int ii = 0; ii < m->NumTexCoords; ii++)
			{
				PRINT_DEBUG_BMD(std::fixed
					<< "\t\t" << m->TexCoords[ii].TexCoordU
					<< "\t" << m->TexCoords[ii].TexCoordV);
			}
			PRINT_DEBUG_BMD("________________________________________");

			PRINT_DEBUG_BMD("[Triangles] Mesh : " << i);
			PRINT_DEBUG_BMD("\tNumTriangles: " << m->NumTriangles);
			for (int ii = 0; ii < m->NumTriangles; ii++)
			{
				PRINT_DEBUG_BMD(std::fixed
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
					<< "\t" << m->Triangles[ii].TexCoordIndex[2] << std::endl);
			}
			PRINT_DEBUG_BMD("========================================");
		}
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

			BMD::SetLockPosition(m_data.Name + ".bmd", i);
		}
		else
		{
			a->Positions = NULL;
		}

		if (_buf.size() < pos)
		{
			PRINT_DEBUG("[ERROR] Corrupted Bmd File");
			return FALSE;
		}

		if (m_data.Actions[0].NumAnimationKeys <= 0)
		{
			PRINT_DEBUG("[ERROR] m_data.Actions[0].NumAnimationKeys <= 0");
			return FALSE;
		}

		if (DEBUG_BMD)
		{
			PRINT_DEBUG_BMD("[Actions] : " << i);
			PRINT_DEBUG_BMD("\tNumAnimationKeys : " << a->NumAnimationKeys);
			PRINT_DEBUG_BMD("\tLockPositions : " << a->LockPositions);
			if (a->LockPositions)
			{
				for (int ii = 0; ii < a->NumAnimationKeys; ii++)
				{
					PRINT_DEBUG_BMD(std::fixed
						<< "\t\t" << a->Positions[ii][0]
						<< "\t" << a->Positions[ii][1]
						<< "\t" << a->Positions[ii][2]);
				}
			}
			PRINT_DEBUG_BMD("========================================");
		}
	}


	if (_buf.size() < pos)
	{
		PRINT_DEBUG("[ERROR] Corrupted Bmd File");
		return FALSE;
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

		if (_buf.size() < pos)
		{
			PRINT_DEBUG("[ERROR] Corrupted Bmd File");
			return FALSE;
		}

		if (DEBUG_BMD)
		{
			PRINT_DEBUG_BMD("[Bones] : " << i);
			PRINT_DEBUG_BMD("\tDummy : " << ((int)b->Dummy));
			if (!b->Dummy)
			{
				PRINT_DEBUG_BMD("\tName : " << b->Name);
				PRINT_DEBUG_BMD("\tParent : " << b->Parent);
				for (int ii = 0; ii < m_data.NumActions; ii++)
				{
					for (int jj = 0; jj < m_data.Actions[ii].NumAnimationKeys; jj++)
					{
						PRINT_DEBUG_BMD(std::fixed
							<< "\t\t" << b->BoneMatrixes[ii].Position[jj][0]
							<< "\t" << b->BoneMatrixes[ii].Position[jj][1]
							<< "\t" << b->BoneMatrixes[ii].Position[jj][2]
							<< "\t" << b->BoneMatrixes[ii].Rotation[jj][0]
							<< "\t" << b->BoneMatrixes[ii].Rotation[jj][1]
							<< "\t" << b->BoneMatrixes[ii].Rotation[jj][2]);
					}
				}
			}
			PRINT_DEBUG_BMD("========================================");
		}
	}

	FixUpBones();
	return TRUE;
}

BOOL BMD::SaveSmd(const char* szDest)
{
	fs::path pFileName = fs::path(szDest).filename().replace_extension("");
	fs::path pFile(szDest);
	Utls::CreateParentDir(pFile);

	fs::path pAnimDir(pFile);
	pAnimDir = pAnimDir.parent_path().append("anims");

	//if (m_data.NumMeshs > 0)	player.bmd has no mesh (only animations)
	{
		std::ofstream os1(pFile);
		Bmd2Smd(os1, SMD_REFERENCE);
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
			PRINT_DEBUG("[ERROR] Failed to write the smd file : " << pFileAnim);
			continue;
		}

		Bmd2Smd(os2, action);

		os2.close();
	}

	return TRUE;
}

BOOL BMD::Bmd2Smd(std::ofstream& os, short action)
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

	if (SmdType == SMD_ANIMATION) return TRUE;

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
				
				if (vertex_index < 0 || vertex_index >= m->NumVertices)
				{
					PRINT_DEBUG("[ERROR] vertex_index < 0 || vertex_index >= m->NumVertices");
					return FALSE;
				}

				if (normal_index < 0 || normal_index >= m->NumNormals)
				{
					PRINT_DEBUG("[ERROR] normal_index < 0 || normal_index >= m->NumNormals");
					return FALSE;
				}

				if (texcoord_index < 0 || texcoord_index >= m->NumTexCoords)
				{
					PRINT_DEBUG("[ERROR] texcoord_index < 0 || texcoord_index >= m->NumTexCoords");
					return FALSE;
				}

				if (vertex_node < 0 || vertex_node >= m_data.NumBones)
				{
					PRINT_DEBUG("[ERROR] vertex_node < 0 || vertex_node >= m_data.NumBones");
					return FALSE;
				}

				if (normal_node < 0 || normal_node >= m_data.NumBones)
				{
					PRINT_DEBUG("[ERROR] normal_index < 0 || normal_index >= m_data.NumBones");
					return FALSE;
				}

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

	return TRUE;
}

//========================================================================================

BOOL BMD::LoadSmd(const char* szSrc)
{
	fs::path p(szSrc);
	std::string sName = p.filename().replace_extension("").string();
	fs::path pAnimDir = p.parent_path().append("anims");

	std::ifstream is(szSrc);
	if (!is.is_open())
	{
		PRINT_DEBUG("[ERROR] Failed to read the smd file : " << szSrc);
		return FALSE;
	}

	Release();
	m_data.Name = sName;
	BMD::ReadSmd(is, SMD_REFERENCE);
	is.close();

	for (short action = 0; ; action++)
	{
		fs::path pFileAnim(pAnimDir);
		pFileAnim.append(sName + "_" + std::to_string(action + 1) + ".smd");

		if (!fs::exists(pFileAnim))
			break;

		std::ifstream is2(pFileAnim);
		BMD::ReadSmd(is2, action);
		is2.close();
	}

	return Smd2Bmd();
}

BOOL BMD::ReadSmd(std::ifstream& is, short action)
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
	std::string bmd_name = m_data.Name + ".bmd";
	std::transform(bmd_name.begin(), bmd_name.end(), bmd_name.begin(), ::tolower);
	a->LockPositions = BMD::GetLockPosition(bmd_name, action);
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

			Bone_t b;
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

	if (a->LockPositions)
	{
		Temp_Lock_Pos.resize(action + 1);
		Temp_Lock_Pos[action].clear();
		Bone_t* b = &m_data.Bones[0];
		for (int i = 0; i < a->NumAnimationKeys; i++)
		{
			BoneMatrix_t* bm = &b->BoneMatrixes[action];
			int j = i + 1;
			if (j > a->NumAnimationKeys - 1) 
				j = a->NumAnimationKeys - 1;

			vec3_t p;
			VectorSubtract(bm->Position[j], bm->Position[i], p);

			Temp_Lock_Pos[action].push_back(p[0]);
			Temp_Lock_Pos[action].push_back(p[1]);
			Temp_Lock_Pos[action].push_back(p[2]);
		}

		a->Positions = (vec3_t*)Temp_Lock_Pos[action].data();
	}
	else
	{
		a->Positions = NULL;
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
				t.TexCoordV = 1.0f - t.TexCoordV;

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

	return TRUE;
}

BOOL BMD::SaveBmd(const char* szDest)
{
	Utls::CreateParentDir(szDest);

	return Encrypt() 
		&& FileWrite(szDest);
}

BOOL BMD::Smd2Bmd(BYTE version)
{
	m_data.Version = version;	//default version = 0xA (no crypt)

	size_t size = 0;

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

	return TRUE;
}