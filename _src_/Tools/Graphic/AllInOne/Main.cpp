// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17
#include "Core.h"
#include "OZJ.h"
#include "OZT.h"
#include "OZB.h"
#include "OZP.h"
#include "OZD.h"
#include "OZG.h"
#include "BMD_SMD.h"
#include "MAP.h"
#include "ATT.h"
#include "OBJ.h"

#include <execution>

#define USE_PARALLEL

#ifndef NO_CORE_DEBUG_LOG
#undef USE_PARALLEL
#endif //!NO_CORE_DEBUG_LOG

using namespace std;

BOOL ReplaceOutputExt(fs::path& output)
{
	fs::path ext = output.extension();
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
		new_ext = EXT_DDS;
		break;
	case INT_OZG:
		new_ext = EXT_GFX;
		break;
	case INT_BMD:
		new_ext = EXT_SMD;
		break;
	case INT_MAP:
		new_ext = EXT_PAM;
		break;
	case INT_ATT:
		new_ext = EXT_TTA;
		break;
	case INT_OBJ:
		new_ext = EXT_JBO;
		break;

	case INT_JPG:
		new_ext = EXT_OZJ;
		break;
	case INT_TGA:
		new_ext = EXT_OZT;
		break;
	case INT_BMP:
		new_ext = EXT_OZB;
		break;
	case INT_PNG:
		new_ext = EXT_OZP;
		break;
	case INT_DDS:
		new_ext = EXT_OZD;
		break;
	case INT_GFX:
		new_ext = EXT_OZG;
		break; 
	case INT_SMD:
		new_ext = EXT_BMD;
		break;	
	case INT_PAM:
		new_ext = EXT_MAP;
		break;
	case INT_TTA:
		new_ext = EXT_ATT;
		break;
	case INT_JBO:
		new_ext = EXT_OBJ;
		break;

	default:
		return FALSE;
	}

	output.replace_extension(new_ext);
	return TRUE;
}


BOOL UnpackFile(const char* szInputPath, const char* szOutputPath)
{
	unique_ptr<OZJ> ozj; 
	unique_ptr<OZT> ozt; 
	unique_ptr<OZB> ozb; 
	unique_ptr<OZP> ozp;
	unique_ptr<OZD> ozd;
	unique_ptr<OZG> ozg;
	unique_ptr<BMD_SMD> bmd;
	unique_ptr<MAP> map;
	unique_ptr<ATT> att;
	unique_ptr<OBJ> obj;

	DWORD N = Ext2Int(fs::path(szInputPath).extension().string().c_str());
	switch (N)
	{
	case INT_OZJ:
		ozj = unique_ptr<OZJ>(new OZJ());
		return ozj->Unpack(szInputPath, szOutputPath);
	case INT_OZT:
		ozt = unique_ptr<OZT>(new OZT());
		return  ozt->Unpack(szInputPath, szOutputPath);
	case INT_OZB:
		ozb = unique_ptr<OZB>(new OZB());
		return ozb->Unpack(szInputPath, szOutputPath);
	case INT_OZP:
		ozp = unique_ptr<OZP>(new OZP());
		return ozp->Unpack(szInputPath, szOutputPath);
	case INT_OZD:
		ozd = unique_ptr<OZD>(new OZD());
		return ozd->Unpack(szInputPath, szOutputPath);
	case INT_OZG:
		ozg = unique_ptr<OZG>(new OZG());
		return ozg->Unpack(szInputPath, szOutputPath);
	case INT_BMD:
		bmd = unique_ptr<BMD_SMD>(new BMD_SMD());
		return bmd->Unpack(szInputPath, szOutputPath);
	case INT_MAP:
		map = unique_ptr<MAP>(new MAP());
		return map->Unpack(szInputPath, szOutputPath);
	case INT_ATT:
		att = unique_ptr<ATT>(new ATT());
		return att->Unpack(szInputPath, szOutputPath);
	case INT_OBJ:
		obj = unique_ptr<OBJ>(new OBJ());
		return obj->Unpack(szInputPath, szOutputPath);
	default:
		return FALSE;
	}
}

BOOL PackFile(const char* szInputPath, const char* szOutputPath)
{
	unique_ptr<OZJ> ozj;
	unique_ptr<OZT> ozt;
	unique_ptr<OZB> ozb;
	unique_ptr<OZP> ozp;
	unique_ptr<OZD> ozd;
	unique_ptr<OZG> ozg;
	unique_ptr<BMD_SMD> bmd;
	unique_ptr<MAP> map;
	unique_ptr<ATT> att;
	unique_ptr<OBJ> obj;

	DWORD N = Ext2Int(fs::path(szInputPath).extension().string().c_str());
	switch (N)
	{
	case INT_JPG:
		ozj = unique_ptr<OZJ>(new OZJ());
		return ozj->Pack(szInputPath, szOutputPath);
	case INT_TGA:
		ozt = unique_ptr<OZT>(new OZT());
		return ozt->Pack(szInputPath, szOutputPath);
	case INT_BMP:
		ozb = unique_ptr<OZB>(new OZB());
		return ozb->Pack(szInputPath, szOutputPath);
	case INT_PNG:
		ozp = unique_ptr<OZP>(new OZP());
		return ozp->Pack(szInputPath, szOutputPath);
	case INT_DDS:
		ozd = unique_ptr<OZD>(new OZD());
		return ozd->Pack(szInputPath, szOutputPath);
	case INT_GFX:
		ozg = unique_ptr<OZG>(new OZG());
		return ozg->Pack(szInputPath, szOutputPath);
	case INT_SMD:
		bmd = unique_ptr<BMD_SMD>(new BMD_SMD());
		return bmd->Pack(szInputPath, szOutputPath);
	case INT_PAM:
		map = unique_ptr<MAP>(new MAP());
		return map->Pack(szInputPath, szOutputPath);
	case INT_TTA:
		att = unique_ptr<ATT>(new ATT());
		return att->Pack(szInputPath, szOutputPath);
	case INT_JBO:
		obj = unique_ptr<OBJ>(new OBJ());
		return obj->Pack(szInputPath, szOutputPath);

	default:
		return FALSE;
	}
}

void FolderProcess(fs::path inputPath, fs::path outputPath)
{
	if (!fs::is_directory(inputPath)) return;

	vector<fs::path> paths;
	vector<fs::path> dirs;
	fs::directory_iterator iters1(inputPath);
	std::copy_if(fs::begin(iters1), fs::end(iters1), std::back_inserter(paths), [](const fs::path& p) {return fs::is_regular_file(p); });
	fs::directory_iterator iters2(inputPath);
	std::copy_if(fs::begin(iters2), fs::end(iters2), std::back_inserter(dirs), [](const fs::path& p) {return fs::is_directory(p) && p.filename().string() != "anims"; });


	//Using parallel aglorithms C++17
	std::for_each(
#ifdef USE_PARALLEL
		std::execution::par
#else
		std::execution::seq
#endif	
		, std::begin(paths), std::end(paths), [&outputPath](const fs::path& p)
		{	
			fs::path p_out = outputPath;
			p_out += "\\";
			p_out += fs::path(p).filename();
			
			if (ReplaceOutputExt(p_out))
			{
				if (!UnpackFile(p.string().c_str(), p_out.string().c_str()))
					PackFile(p.string().c_str(), p_out.string().c_str());
			}
		}
	);

	std::for_each(std::execution::seq, std::begin(dirs), std::end(dirs),
		[&outputPath](const std::filesystem::path& d)
		{
			fs::path d_out = outputPath;
			d_out += "\\";
			d_out += fs::path(d).filename();
			FolderProcess(d, d_out);
		}
	);
}

int main(int argc, char** argv)
{
#ifndef NO_CORE_DEBUG_LOG
	ofstream log("AllInOne.log");
	std::cout.rdbuf(log.rdbuf());
#endif // !NO_CORE_DEBUG_LOG

	BMD_SMD::LoadLockPostionData("LockPositionData.txt");

	const char* szInputPath = nullptr;
	const char* szOutputPath = nullptr;

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

	if (fs::is_regular_file(szInputPath))
	{
		if (!UnpackFile(szInputPath, szOutputPath) && !PackFile(szInputPath, szOutputPath))
		{
			return EXIT_FAILURE;	
		}
	}
	else if (fs::is_directory(szInputPath))
	{
		fs::path inputPath = Utls::RemoveSlashEnd(szInputPath);
		fs::path outputPath(szOutputPath ? szOutputPath : (inputPath.string() + "_out"));

		FolderProcess(inputPath, outputPath);
	}
	else
	{
		PRINT_DEBUG("[ERROR] Invalid file/folder path.");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}