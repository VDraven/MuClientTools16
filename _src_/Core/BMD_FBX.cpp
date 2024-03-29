#include "BMD_FBX.h"

#include "FBX.h"
#include "OZJ.h"
#include "OZT.h"
#include "OZB.h"
#include "OZP.h"
#include "OZD.h"
#include "ZzzMathLib.h"

#include <sstream>
#include <iomanip>

//#define _DEBUG_BMD_

#ifdef _DEBUG_BMD_
#define DEBUG_BMD 1
#define PRINT_DEBUG_BMD(msg) std::cout << msg << std::endl
#else
#define DEBUG_BMD 0
#define PRINT_DEBUG_BMD(msg)
#endif

double BMD_FBX::FRAME_TIME = 0.25;
void BMD_FBX::SetFrameTime(double frame_time)
{
	BMD_FBX::FRAME_TIME = frame_time;
}

fs::path BMD_FBX::ROOT_PATH = "";
void BMD_FBX::SetRootPath(fs::path root_path)
{
	ROOT_PATH = root_path;
}
;


//========================================================================================

BOOL BMD_FBX::Unpack(const char* szSrc, const char* szDest, bool find_textures, bool rename_textures, bool export_normals)
{
	if (!szSrc) return FALSE;

	printf("[BMD_FBX] Unpack: %s [FBX_SDK] ", szSrc);

	char szNewDest[512];
	if (!szDest)
	{
		std::string sName = fs::path(szSrc).filename().replace_extension("").string();
		strcpy_s(szNewDest, 512, fs::path(szSrc).parent_path().append(sName + "_fbx").append(sName + ".fbx").string().c_str());
		szDest = szNewDest;
	}

	fs::path pSrcDir = fs::path(szSrc).parent_path();
	std::unordered_map<std::string, fs::path> textures;

	return LoadBmd(szSrc)
		&& SaveFbx(szDest, find_textures ? &textures : NULL, rename_textures, export_normals)
		&& FindTexture(pSrcDir, find_textures ? &textures : NULL);
}

//========================================================================================

BOOL BMD_FBX::Release()
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

BOOL BMD_FBX::FixUpBones()
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

BOOL BMD_FBX::Encrypt()
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

BOOL BMD_FBX::Decrypt()
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

BOOL BMD_FBX::LoadBmd(const char* szSrc)
{
	Release();
	m_data.Name = fs::path(szSrc).filename().replace_extension("").string();

	return FileOpen(szSrc)
		&& Decrypt()
		&& ReadBmd();
}

BOOL BMD_FBX::ReadBmd()
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

			//BMD_FBX::SetLockPosition(m_data.Name + ".bmd", i);
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
			static vec3_t DUMMY_VEC3{ 0.0f, 0.0f,0.0f };
			BoneMatrix_t DUMMY_BONE_MATRIX { &DUMMY_VEC3 , &DUMMY_VEC3 };
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

BOOL BMD_FBX::SaveFbx(const char* szDest, std::unordered_map<std::string, fs::path>* textures, bool rename_textures, bool export_normals)
{
	Utls::CreateParentDir(szDest);


	// Initialize the FBX SDK.
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	InitializeSdkObjects(lSdkManager, lScene);

	// Fix Axis System. Pick the right axis for your targer game engine. 
	// This setup is for Unreal Engine.
	FbxAxisSystem mAxisSystem(FbxAxisSystem::EPreDefinedAxisSystem::eMayaZUp);
	mAxisSystem.ConvertScene(lScene);

	//FbxAxisSystem mAxisSystem(
	//	FbxAxisSystem::EUpVector::eZAxis, 
	//	(FbxAxisSystem::EFrontVector) -FbxAxisSystem::EFrontVector::eParityOdd,	// Hmm...
	//	FbxAxisSystem::eRightHanded);
	//mAxisSystem.ConvertScene(lScene);
	
	// Scene's root node
	FbxNode* lRootNode = lScene->GetRootNode();

	//==========================================================

	// Self making a new root bone at the original
	// Some BMD files use multiple root bones (parent = -1),
	//  While some game engines do not support that (such as: Unreal Engine...)
	FbxSkeleton* lSkeletonRootNodeAttribute = FbxSkeleton::Create(lScene, "root");
	lSkeletonRootNodeAttribute->SetSkeletonType(FbxSkeleton::eRoot);
	FbxNode* lSkeletonRootNode = FbxNode::Create(lScene, "root");
	lSkeletonRootNode->SetNodeAttribute(lSkeletonRootNodeAttribute);
	lSkeletonRootNode->LclTranslation.Set(FbxDouble3(0.0, 0.0, 0.0));
	lSkeletonRootNode->LclRotation.Set(FbxDouble3(0.0, 0.0, 0.0));

	std::vector<FbxCluster*> aClusters;
	std::vector<FbxNode*> aSkeletonNodes;
	if (m_data.NumBones > 0)
	{
		// Create skeleton nodes 
		for (int i = 0; i < m_data.NumBones; i++)
		{
			Bone_t* b = &m_data.Bones[i];

			char szNodeName[256];
			sprintf(szNodeName, "%s", m_data.Bones[i].Name.c_str());

			FbxDouble3 r;
			FbxDouble3 t;
			BoneMatrix_t* bm = &b->BoneMatrixes[0];			//0 : bind pose (T pose)
			r[0] = bm->Rotation[0][0] * (180.0f / Q_PI);	//rad -> deg
			r[1] = bm->Rotation[0][1] * (180.0f / Q_PI);
			r[2] = bm->Rotation[0][2] * (180.0f / Q_PI);
			t[0] = bm->Position[0][0];
			t[1] = bm->Position[0][1];
			t[2] = bm->Position[0][2];

			//if (!b->Dummy)
			//{
				FbxSkeleton* lSkeletonNodeAttribute = FbxSkeleton::Create(lScene, szNodeName);
				lSkeletonNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
				FbxNode* lSkeletonNode = FbxNode::Create(lScene, szNodeName);
				lSkeletonNode->SetNodeAttribute(lSkeletonNodeAttribute);
				FbxCluster* lClusterToNode = FbxCluster::Create(lScene, "");

				lClusterToNode->SetLink(lSkeletonNode);
				lClusterToNode->SetLinkMode(FbxCluster::eTotalOne);
				lSkeletonNode->LclTranslation.Set(t);
				lSkeletonNode->LclRotation.Set(r);

				aClusters.push_back(lClusterToNode);
				aSkeletonNodes.push_back(lSkeletonNode);
			//}
			//else
			//{
			//	aClusters.push_back(NULL);
			//	aSkeletonNodes.push_back(NULL);
			//}
		}

		// We process Dummy nodes anyway. So it must never be NULL
		//for (int i = 0; i < m_data.NumBones; i++)
		//	FBX_ASSERT(aSkeletonNodes[i] && aClusters[i]);

		// Build skeleton node hierarchy. 
		for (int i = 0; i < m_data.NumBones; i++)
		{
			short parent_idx = m_data.Bones[i].Parent;
			if (parent_idx >= 0 && parent_idx < m_data.NumBones)	
				aSkeletonNodes[parent_idx]->AddChild(aSkeletonNodes[i]);
			else //parent_idx = -1 (Old bmd root node. Add it to the new root)
				lSkeletonRootNode->AddChild(aSkeletonNodes[i]);
		}
		// Add to the Scene's root node
		lRootNode->AddChild(lSkeletonRootNode);
	}

	//==========================================================
	// Prepare

	char szMeshName[256];
	strcpy_s(szMeshName, 256, fs::path(szDest).filename().replace_extension("").string().c_str());

	// Create mesh & add to scene
	FbxMesh* lMesh = FbxMesh::Create(lScene, szMeshName);
	FbxNode* lMeshNode = FbxNode::Create(lScene, szMeshName);
	lMeshNode->SetNodeAttribute(lMesh);
	lRootNode->AddChild(lMeshNode);
	// Set the shading mode to view texture
	lMeshNode->SetShadingMode(FbxNode::EShadingMode::eTextureShading);

	// Count total vertices of all sub meshes
	int total_vectices = 0;
	for (int i = 0; i < m_data.NumMeshs; i++)
	{
		Mesh_t* m = &m_data.Meshs[i];
		total_vectices += m->NumVertices;
	}
	// Create control points
	lMesh->InitControlPoints(total_vectices);
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	// UV Element
	char UVSetName[256];
	strcpy_s(UVSetName, 256, m_data.Name.c_str());
	FbxGeometryElementUV* lUVElement = lMesh->CreateElementUV(UVSetName);
	lUVElement->SetMappingMode(FbxGeometryElement::eByPolygonVertex);
	lUVElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);

	// Normal Element
	FbxGeometryElementNormal* lNormalElement;
	if (export_normals)
	{
		lNormalElement = lMesh->CreateElementNormal();
		lNormalElement->SetMappingMode(FbxGeometryElement::eByControlPoint);
		lNormalElement->SetReferenceMode(FbxGeometryElement::eDirect);
	}

	// Material Element
	FbxGeometryElementMaterial* lMaterialElement = lMesh->CreateElementMaterial();
	lMaterialElement->SetMappingMode(FbxGeometryElement::eByPolygon);
	lMaterialElement->SetReferenceMode(FbxGeometryElement::eIndexToDirect);


	//==========================================================
	
	int control_point_idx = 0; int uv_idx = 0;
	for (int i = 0; i < m_data.NumMeshs; i++)	// Iter through all sub meshes
	{
		Mesh_t* m = &m_data.Meshs[i];

		// Material & Texture works
		// sTextureExt : ".tga"
		// sTextureName : "TGA_texture"
		// szTextureFilename : TGA_texture.tga		//tree_01.jpg vs tree_01.tga will cause name conflict when importing. So CHANGE!

		std::string sTextureExt = fs::path(m_data.Textures[m->Texture].FileName).extension().string();
		std::transform(sTextureExt.begin(), sTextureExt.end(), sTextureExt.begin(), toupper);
		std::string sTextureName = sTextureExt.substr(1) + "_" + fs::path(m_data.Textures[m->Texture].FileName).replace_extension("").string();
		std::transform(sTextureExt.begin(), sTextureExt.end(), sTextureExt.begin(), tolower);

		char szTextureFilename[256];
		if (rename_textures)
		{
			std::ostringstream oss;
			// "Mesh_T_001.jpg"
			oss << szMeshName << "_T_"
				<< std::setfill('0') << std::setw(3) << i
				<< sTextureExt;
			strcpy_s(szTextureFilename, 256, oss.str().c_str());
		}
		else sprintf(szTextureFilename, "%s%s", sTextureName.c_str(), sTextureExt.c_str());
		
		if (textures)
		{
			std::string texture_key = m_data.Textures[m->Texture].FileName;
			std::transform(texture_key.begin(), texture_key.end(), texture_key.begin(), tolower);
			
			fs::path texture_dest = fs::path(szDest).parent_path().append(fs::path(szTextureFilename).filename().string());
			
			textures->emplace(std::move(texture_key), std::move(texture_dest));
		}


		// New material.
		FbxString lMaterialName = FbxString("MI_") + FbxString(sTextureName.c_str());
		FbxString lShadingName = "Phong";
		FbxDouble3 lBlack(0.0, 0.0, 0.0);
		FbxDouble3 lRed(1.0, 0.0, 0.0);
		FbxDouble3 lDiffuseColor(0.75, 0.75, 0.0);

		// Try to get material at index i first (may not need ???) 
		FbxSurfacePhong* lMaterial = lMeshNode->GetSrcObject<FbxSurfacePhong>(i);
		if (lMaterial == NULL)
		{
			lMaterial = FbxSurfacePhong::Create(lScene, lMaterialName.Buffer());
		}

		// Generate primary and secondary colors.
		//lMaterial->Emissive.Set(lBlack);
		//lMaterial->Ambient.Set(lRed);
		//lMaterial->AmbientFactor.Set(1.);
		// Add texture for diffuse channel
		//lMaterial->Diffuse.Set(lDiffuseColor);
		//lMaterial->DiffuseFactor.Set(1.);
		//lMaterial->TransparencyFactor.Set(0.4);
		lMaterial->ShadingModel.Set(lShadingName);
		lMaterial->Shininess.Set(0.5);
		lMaterial->Specular.Set(lBlack);
		lMaterial->SpecularFactor.Set(0.3);
		// Add it the mesh node
		lMeshNode->AddMaterial(lMaterial);
		
		// New texture file
		FbxFileTexture* lTexture = FbxFileTexture::Create(lScene, sTextureName.c_str());

		// Set texture properties.
		lTexture->SetFileName(szTextureFilename); // Resource file is in current directory.
		lTexture->SetTextureUse(FbxTexture::eStandard);
		lTexture->SetMappingType(FbxTexture::eUV);
		lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
		lTexture->SetSwapUV(false);
		lTexture->SetTranslation(0.0, 0.0);
		lTexture->SetScale(1.0, 1.0);
		lTexture->SetRotation(0.0, 0.0);
		lTexture->UVSet.Set(FbxString(UVSetName)); // Connect texture to the proper UV

		// Don't forget to connect the texture to the corresponding property of the material
		lMaterial->Diffuse.ConnectSrcObject(lTexture);

		// Alpha channel. For now, only .tga
		if(sTextureExt == ".tga")
		{
			lTexture->SetBlendMode(FbxTexture::eTranslucent);
			lTexture->SetAlphaSource(FbxTexture::eBlack);
			lMaterial->TransparentColor.ConnectSrcObject(lTexture);
		}

		//==========================================================
		// Mesh works
		
		// ControlPoints, Normals, Clusters/Skin
		for (int j = 0; j < m->NumVertices; j++)
		{
			// control points
			vec3_t p = { 0.0f, 0.0f, 0.0f };
			if (m->Vertices[j].Node >= 0 && m->Vertices[j].Node < m_data.NumBones)
			{
				VectorTransform(m->Vertices[j].Position, BoneFixup[m->Vertices[j].Node].m, p);
				VectorAdd(p, BoneFixup[m->Vertices[j].Node].WorldOrg, p);
			}
			lControlPoints[control_point_idx + j] = FbxVector4(p[0], p[1], p[2]);

			// normal vectors
			if (export_normals)
			{
				vec3_t n = { 0.0f, 0.0f, 0.0f };
				for (int k = 0; k < m->NumNormals; k++)
				{
					if (m->Normals[k].BindVertex == j && m->Normals[k].Node >= 0 && m->Normals[k].Node < m->NumNormals)
					{
						VectorTransform(m->Normals[k].Normal, BoneFixup[m->Normals[k].Node].m, n);
						VectorNormalize(n);

						break;
					}
				}
				lNormalElement->GetDirectArray().Add(FbxVector4(n[0], n[1], n[2]));
			}

			// skinning
			short node = m->Vertices[j].Node;
			if (node >= 0 && node < m_data.NumBones)
			{
				aClusters[node]->AddControlPointIndex(control_point_idx + j, 1.0);
			}
		}

		// UVs
		for (int j = 0; j < m->NumTexCoords; j++)
		{
			TexCoord_t* uv = &m->TexCoords[j];
			lUVElement->GetDirectArray().Add(FbxVector2(uv->TexCoordU, 1.0f - uv->TexCoordV));
		}

		// Polygons
		for (int j = 0; j < m->NumTriangles; j++)
		{
			Triangle_t* t = &m->Triangles[j];

			// polygon material (i)
			lMesh->BeginPolygon(i);	
			for (int k = 0; k < 3; k++)
			{
				// vertex's control point & uv indexes
				lMesh->AddPolygon(control_point_idx + t->VertexIndex[k], uv_idx + t->TexCoordIndex[k]);
			}
			lMesh->EndPolygon();
		}

		uv_idx += m->NumTexCoords;
		control_point_idx += m->NumVertices;
	}

	//==========================================================
	// Add the clusters to the mesh by creating a skin and adding those clusters to that skin.

	if (m_data.NumBones > 0)
	{
		FbxSkin* lSkin = FbxSkin::Create(lScene, "Skin");

		for (int i = 0; i < m_data.NumBones; i++)
		{
			//if (aClusters[i] == NULL || aSkeletonNodes[i] == NULL)
			//	continue;
			
			if (aClusters[i]->GetControlPointIndicesCount() <= 0)
			{
				aClusters[i]->Destroy();
				continue;
			}

			FbxAMatrix lXMatrix;
			lXMatrix = lMeshNode->EvaluateGlobalTransform();
			aClusters[i]->SetTransformMatrix(lXMatrix);

			lXMatrix = aSkeletonNodes[i]->EvaluateGlobalTransform();
			aClusters[i]->SetTransformLinkMatrix(lXMatrix);

			lSkin->AddCluster(aClusters[i]);
		}

		// Add Skin to Mesh
		if (lSkin->GetClusterCount() > 0)
			lMesh->AddDeformer(lSkin);
		else
			lSkin->Destroy();

		// Store the Bind Pose
		StoreBindPose(lScene, lMeshNode);
	}

	//==========================================================
	// Animations works

	#define ADD_LAST_FRAME 1
	bool has_anim = (m_data.NumActions > 1) 
		|| (m_data.NumActions == 1 && m_data.Actions[0].NumAnimationKeys > 1);
	if (has_anim)
	{
		for (int i = 0; i < m_data.NumActions; i++)
		{
			Action_t* a = &m_data.Actions[i];
			short nKeys = a->NumAnimationKeys;
			if (nKeys <= 0)
				continue;
			if (i == 0 && nKeys <= 1)	//bind pose data
				continue;

			char szAnimStackName[256]; 
			char szAnimLayerName[256];
			sprintf(szAnimStackName, "%d", i);
			sprintf(szAnimLayerName, "AnimLayer_%d", i);
			FbxAnimStack* lAnimStack = FbxAnimStack::Create(lScene, szAnimStackName);
			FbxAnimLayer* lAnimLayer = FbxAnimLayer::Create(lScene, szAnimLayerName);
			lAnimStack->AddMember(lAnimLayer);

			bool bAddLastFrame = (i == 0 && ADD_LAST_FRAME);

			FbxTime lTimeStart, lTimeStop;
			lTimeStart.SetSecondDouble(0.0);
			lTimeStop.SetSecondDouble(BMD_FBX::FRAME_TIME * (nKeys - 1 + (bAddLastFrame ? 1 : 0)));
			FbxTimeSpan lTimeSpan(lTimeStart, lTimeStop);
			lAnimStack->SetLocalTimeSpan(lTimeSpan);

			auto AddAnim = [&](FbxNode* node, BoneMatrix_t* bm, bool bAddLastFrame, bool bLock = false)
			{
				node->LclTranslation.GetCurveNode(lAnimLayer, true);
				node->LclRotation.GetCurveNode(lAnimLayer, true);
				FbxAnimCurve* lCurveTX = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
				FbxAnimCurve* lCurveTY = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
				FbxAnimCurve* lCurveTZ = node->LclTranslation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);
				FbxAnimCurve* lCurveRX = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
				FbxAnimCurve* lCurveRY = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
				FbxAnimCurve* lCurveRZ = node->LclRotation.GetCurve(lAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

				FbxTime lTime; int lKeyIndex = 0;


				lCurveTX->KeyModifyBegin(); lCurveRX->KeyModifyBegin();
				lCurveTY->KeyModifyBegin(); lCurveRY->KeyModifyBegin();
				lCurveTZ->KeyModifyBegin(); lCurveRZ->KeyModifyBegin();
				for (int k = 0; k <= nKeys; k++)
				{
					if (k == nKeys && !bAddLastFrame)
						break;

					FbxVector4 r;
					FbxVector4 t;

					r[0] = !bm ? 0.0 : bm->Rotation[(k == nKeys || bLock) ? 0 : k][0] * (180.0f / Q_PI);
					r[1] = !bm ? 0.0 : bm->Rotation[(k == nKeys || bLock) ? 0 : k][1] * (180.0f / Q_PI);
					r[2] = !bm ? 0.0 : bm->Rotation[(k == nKeys || bLock) ? 0 : k][2] * (180.0f / Q_PI);
					t[0] = !bm ? 0.0 : bm->Position[(k == nKeys || bLock) ? 0 : k][0];
					t[1] = !bm ? 0.0 : bm->Position[(k == nKeys || bLock) ? 0 : k][1];
					t[2] = !bm ? 0.0 : bm->Position[(k == nKeys || bLock) ? 0 : k][2];

					lTime.SetSecondDouble(BMD_FBX::FRAME_TIME * k);
					lKeyIndex = lCurveTX->KeyAdd(lTime);
					lCurveTX->KeySet(lKeyIndex, lTime, t[0], FbxAnimCurveDef::eInterpolationLinear);
					lKeyIndex = lCurveTY->KeyAdd(lTime);
					lCurveTY->KeySet(lKeyIndex, lTime, t[1], FbxAnimCurveDef::eInterpolationLinear);
					lKeyIndex = lCurveTZ->KeyAdd(lTime);
					lCurveTZ->KeySet(lKeyIndex, lTime, t[2], FbxAnimCurveDef::eInterpolationLinear);
					lKeyIndex = lCurveRX->KeyAdd(lTime);
					lCurveRX->KeySet(lKeyIndex, lTime, r[0], FbxAnimCurveDef::eInterpolationLinear);
					lKeyIndex = lCurveRY->KeyAdd(lTime);
					lCurveRY->KeySet(lKeyIndex, lTime, r[1], FbxAnimCurveDef::eInterpolationLinear);
					lKeyIndex = lCurveRZ->KeyAdd(lTime);
					lCurveRZ->KeySet(lKeyIndex, lTime, r[2], FbxAnimCurveDef::eInterpolationLinear);
				}
				lCurveTX->KeyModifyEnd(); lCurveRX->KeyModifyEnd();
				lCurveTY->KeyModifyEnd(); lCurveRY->KeyModifyEnd();
				lCurveTZ->KeyModifyEnd(); lCurveRZ->KeyModifyEnd();
			};

			for (int j = 0; j < m_data.NumBones; j++)
			{
				Bone_t* b = &m_data.Bones[j];
				if (b->Dummy || i >= b->BoneMatrixes.size())
					continue;
				BoneMatrix_t* bm = &b->BoneMatrixes[i];

				
				if (j == 0)
				{
					// Always keep this at the origin point
					AddAnim(lSkeletonRootNode, NULL, bAddLastFrame);

					//https://docs.unrealengine.com/4.27/en-US/AnimatingObjects/SkeletalMeshAnimation/RootMotion/
					// Temporary, this works. I want walking/running/flying animations -> root motions
					AddAnim(aSkeletonNodes[0], bm, bAddLastFrame, a->LockPositions);
				}
				else
				{
					AddAnim(aSkeletonNodes[j], bm, bAddLastFrame);
				}

			}
		}
	}
	
	char szNewDest[512];
	sprintf(szNewDest, "%s\\%s_%s",
		fs::path(szDest).parent_path().string().c_str(),
		(has_anim ? "SKM" : "SM"),
		fs::path(szDest).filename().string().c_str()
	);

	// Save to fbx file
	SaveScene(lSdkManager, lScene, szNewDest, 0);

	// Must clean up after
	DestroySdkObjects(lSdkManager, 0);
	return TRUE;
}

void FindTextureRecursive(fs::path pDir, std::unordered_map<std::string, fs::path>* textures)
{

	auto ReplaceTextureExt = [](fs::path& pFilename) -> BOOL
	{
		fs::path ext = pFilename.extension();
		fs::path new_ext;
		if (ext.string().length() != 4)
			return FALSE;

		DWORD N = Ext2Int(ext.string().c_str());
		switch (N)
		{
		case INT_OZJ:
			new_ext = EXT_JPG;
			break;
		case INT_OZT:
			new_ext = EXT_TGA;
			break;
		case INT_OZB:
			new_ext = EXT_BMP;
			break;
		case INT_OZP:
			new_ext = EXT_PNG;
			break;
		case INT_OZD:
			new_ext = EXT_OZD;
			break;

		default:
			return FALSE;
		}

		pFilename.replace_extension(new_ext);
		return TRUE;
	};

	auto UnpackTexture = [](fs::path pInput, fs::path pOutput) -> BOOL
	{
		OZJ* ozj;
		OZT* ozt;
		OZB* ozb;
		OZP* ozp;
		OZD* ozd;

		DWORD N = Ext2Int(pInput.extension().string().c_str());
		char* szInput = new char[512];
		strcpy_s(szInput, 512, pInput.string().c_str());
		char* szOutput = new char[512]; 
		strcpy_s(szOutput, 512, pOutput.string().c_str());

		BOOL result = FALSE;
		switch (N)
		{
		case INT_OZJ:
			ozj = new OZJ();
			result = ozj->Unpack(szInput, szOutput);
			delete ozj;
			break;
		case INT_OZT:
			ozt = new OZT();
			result = ozt->Unpack(szInput, szOutput);
			delete ozt;
			break;
		case INT_OZB:
			ozb = new OZB();
			result = ozb->Unpack(szInput, szOutput);
			delete ozb;
			break;
		case INT_OZP:
			ozp = new OZP();
			result = ozp->Unpack(szInput, szOutput);
			delete ozp;
			break;
		case INT_OZD:
			ozd = new OZD();
			result = ozd->Unpack(szInput, szOutput);
			delete ozd;
			break;

		default:
			break;
		}
		delete[] szInput;
		delete[] szOutput;
		return result;
	};

	fs::directory_iterator iter(pDir);
	for (auto& dir_entry : iter)
	{
		if (dir_entry.is_regular_file())
		{
			fs::path pFilename = dir_entry.path().filename();
			if (ReplaceTextureExt(pFilename))
			{
				std::string sFilename = pFilename.string();
				std::transform(sFilename.begin(), sFilename.end(), sFilename.begin(), tolower);

				if (textures->count(sFilename) > 0)
				{
					try
					{
						std::cout << "\tTexture: " << dir_entry.path().string().c_str() << std::endl;
						if (!UnpackTexture(dir_entry.path(), textures->at(sFilename)))
							std::cout << "\t[UNPACK FAILED]: " << pFilename.string().c_str() << std::endl;
					}
					catch (std::exception ex)
					{
						std::cout << "\t[UNPACK ERROR]: " << pFilename.string().c_str() << " | " << ex.what() << std::endl;
					}
					textures->extract(sFilename);

					if (textures->empty())
						return;
				}
			}
		}
	}

	std::vector<fs::path> sub_dirs;
	fs::directory_iterator iter2(pDir);
	for (auto& dir_entry : iter2)
	{
		if (dir_entry.is_directory())
		{
			FindTextureRecursive(dir_entry.path(), textures);

			if (textures->empty())
				return;
		}
	}
}

BOOL BMD_FBX::FindTexture(fs::path pDir, std::unordered_map<std::string, fs::path>* textures)
{
	if (!textures || textures->empty() || fs::is_directory(pDir))
	{
		FindTextureRecursive(pDir, textures);

		if (textures->empty()) return TRUE;
		for (auto it = textures->begin(); it != textures->end(); it++)
		{
			std::cout << "\t[LOCAL TEXTURE NOT FOUND]: " << it->first.c_str() << std::endl;
		}

		if (fs::is_directory(ROOT_PATH))
			FindTextureRecursive(ROOT_PATH, textures);

		if (textures->empty()) return TRUE;
		for (auto it = textures->begin(); it != textures->end(); it++)
		{
			std::cout << "\t[GLOBAL TEXTURE NOT FOUND]: " << it->first.c_str() << std::endl;
		}
	}

	return TRUE;	//just TRUE
}

