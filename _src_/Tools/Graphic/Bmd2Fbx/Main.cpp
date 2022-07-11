// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17

// "FbxLinkData.txt" format:
// [bmd file name - case insensitive] \tab [fbx output name - case sensitive] \tab [a link] \tab [another link] ...
// Ex: 
// Monster01.bmd	Bull_Fighter.fbx	Monster\Group_1\Lorencia	Monster\Group_2\Something	Monster\Group_3\BlaBlaBla

#include "Core.h"
#include "BMD_FBX.h"
#include "FbxLink.h"
#include <execution>
using namespace std;

//==================================================================================

#define FBX_DEBUG_LOG			1	// log file "Bmd2Fbx.log"
#define FBX_FOLDER				1	// "out\model\model.fbx" vs "out\model.fbx"
#define FBX_FIND_TEXTURES		1	// find and copy textures required to fbx folder
#define FBX_CREATE_LINK			1	// grouping by creating Windows .lnk (shortcut) files. Depend on "FbxLinkData.txt"
//Should disable FBX_RENAME_TEXTURE. It makes unnecessary dupicated textures when importing (same textures but different names)
#define FBX_RENAME_TEXTURE		0	// rename textures to model name. Ex: "ModelName_T_001.jpg"	
//Time (in second) between 2 anim keyframes. default = 0.25
#define FBX_FRAME_TIME			0.25

//==================================================================================

#if FBX_DEBUG_LOG
#define PRINT_DEBUG(msg) cout << msg << endl
#else
#define PRINT_DEBUG(msg) 
#endif

extern multimap<string, vector<string>> fbx_links_data;
fs::path fbx_links_dir;

BOOL UnpackBMD(const char* szInputPath, const char* szOutputPath, bool only_unpack = false)
{
	unique_ptr<BMD_FBX> bmd_fbx;

	DWORD N = Ext2Int(fs::path(szInputPath).extension().string().c_str());
	switch (N)
	{
	case INT_BMD:
		bmd_fbx = unique_ptr<BMD_FBX>(new BMD_FBX());
		if(only_unpack)
			return bmd_fbx->Unpack(szInputPath, szOutputPath);
		else
			return bmd_fbx->Unpack(szInputPath, szOutputPath, FBX_FIND_TEXTURES, FBX_RENAME_TEXTURE);

	default:
		return FALSE;
	}
}

BOOL IsBmdFile(const fs::path& p)
{
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

	return (fs::is_regular_file(p) && Ext2Int(p.extension().string().c_str()) == INT_BMD && CheckBmd(p.string().c_str()));
}

void FolderProcess(fs::path inputPath, fs::path outputPath)
{
	vector<fs::path> bmds;
	vector<fs::path> dirs;
	fs::directory_iterator iter1(inputPath);
	std::copy_if(fs::begin(iter1), fs::end(iter1), std::back_inserter(bmds), [](const fs::path& p) {return IsBmdFile(p); });
	fs::directory_iterator iter2(inputPath);
	std::copy_if(fs::begin(iter2), fs::end(iter2), std::back_inserter(dirs), [](const fs::path& p) {return fs::is_directory(p); });

	//FBX SDK is single-thread based !!! NO PARALLEL HERE
	std::for_each(
//		std::execution::par
		std::execution::seq
		, std::begin(bmds), std::end(bmds), [&outputPath](const fs::path& src)
		{
			fs::path dest = outputPath;
#ifdef FBX_FOLDER
			dest.append( std::string("_Fbx_") + fs::path(src).filename().replace_extension("").string());
#endif
			dest.append(fs::path(src).filename().string());
			dest.replace_extension(EXT_FBX);

			try
			{
				if (UnpackBMD(src.string().c_str(), dest.string().c_str()))
				{
					PRINT_DEBUG("Unpacked: " << src.string().c_str());

					if (FBX_CREATE_LINK)
					{
						char szTarget[512];
						char szLink[512];
						strcpy_s(szTarget, 512, dest.string().c_str());

						string key = src.filename().string();
						Utls::ToLowerCaseString(key);

						if (fbx_links_data.count(key) > 0)
						{
							auto ret = fbx_links_data.equal_range(key);
							for (auto it = ret.first; it != ret.second; it++)
							{
								for (string& link : it->second)
								{
									sprintf_s(szLink, 512, "%s\\%s.lnk", fbx_links_dir.string().c_str(), link.c_str());
									Utls::CreateParentDir(fs::path(szLink));

									CreateLink(szTarget, szLink);
									PRINT_DEBUG("\tLinked: " << link.c_str());
								}
							}
						}
					}
				}
				else
					PRINT_DEBUG("\t[FAILED BMD UNPACKING] " << src.string().c_str());
			}
			catch (const exception& e)
			{
				PRINT_DEBUG("\t[ERROR BMD UNPACKING] " << e.what());
			}
		}
	);

	std::for_each(std::execution::seq, std::begin(dirs), std::end(dirs),
		[&outputPath](const std::filesystem::path& d)
		{
			fs::path d_out = outputPath;
			d_out.append(fs::path(d).filename().string());
			FolderProcess(d, d_out);
		}
	);
}

int main(int argc, char** argv)
{
#ifdef FBX_DEBUG_LOG
	ofstream log("Bmd2Fbx.log");
	std::cout.rdbuf(log.rdbuf());
#endif

	//Time (in second) between 2 anim keyframes. default = 0.25
	BMD_FBX::SetFrameTime(FBX_FRAME_TIME);

	if (FBX_CREATE_LINK)
		LoadFbxLinkData("FbxLinkData.txt");

	const char* szInputPath = NULL;
	const char* szOutputPath = NULL;

	if (argc >= 2)
		szInputPath = argv[1];
	if (argc >= 3)
		szOutputPath = argv[2];

	if (!szInputPath)
	{
		PRINT_DEBUG("Drag & Drop the file/folder");
		PRINT_DEBUG(" or use console command to execute with the file path.");
		return EXIT_FAILURE;
	}

	if (!fs::exists(szInputPath))
	{
		PRINT_DEBUG("[ERROR] Input file/folder does not exist.");
		return EXIT_FAILURE;
	}

	if (IsBmdFile(szInputPath))
	{
		try
		{
			if (UnpackBMD(szInputPath, szOutputPath, true))
			{
				PRINT_DEBUG("\tUnpacked: " << szInputPath);
				return EXIT_SUCCESS;
			}
			else
			{
				PRINT_DEBUG("\t[FAILED BMD UNPACKING]: " << szInputPath);
				return EXIT_FAILURE;
			}
		}
		catch (const exception& e)
		{
			PRINT_DEBUG("\t[ERROR BMD UNPACKING] " << e.what());
			return EXIT_FAILURE;
		}
	}
	else if (fs::is_directory(szInputPath))
	{
		fs::path inputPath = Utls::RemoveSlashEnd(szInputPath);
		fs::path outputPath(szOutputPath ? szOutputPath : (inputPath.string() + "_out"));

		if (FBX_CREATE_LINK)
		{
			fbx_links_dir = outputPath;
			fbx_links_dir.append("__FbxLinks__");
		}

		FolderProcess(inputPath, outputPath);
	}
	else
	{
		PRINT_DEBUG("[ERROR] Invalid file/folder path.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}