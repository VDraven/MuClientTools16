// This file contains the 'main' function. Program execution begins and ends there.
// Requirement: C++17
#include "Core.h"
#include "OZJ.h"
#include "OZT.h"
#include "OZB.h"
#include "OZP.h"
#include "OZD.h"
#include "OZG.h"

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

	default:
		return FALSE;
	}

	output.replace_extension(new_ext);
	return TRUE;
}

BOOL UnpackFile(const char* szInputPath, const char* szOutputPath)
{
	DWORD N = Ext2Int(fs::path(szInputPath).extension().string().c_str());
	switch (N)
	{
	case INT_OZJ:
		return sInstance(OZJ)->Unpack(szInputPath, szOutputPath);
	case INT_OZT:
		return sInstance(OZT)->Unpack(szInputPath, szOutputPath);
	case INT_OZB:
		return sInstance(OZB)->Unpack(szInputPath, szOutputPath);
	case INT_OZP:
		return sInstance(OZP)->Unpack(szInputPath, szOutputPath);
	case INT_OZD:
		return sInstance(OZD)->Unpack(szInputPath, szOutputPath);
	case INT_OZG:
		return sInstance(OZG)->Unpack(szInputPath, szOutputPath);
	default:
		return FALSE;
	}
}

BOOL PackFile(const char* szInputPath, const char* szOutputPath)
{
	DWORD N = Ext2Int(fs::path(szInputPath).extension().string().c_str());
	switch (N)
	{
	case INT_JPG:
		return sInstance(OZJ)->Pack(szInputPath, szOutputPath);
	case INT_TGA:
		return sInstance(OZT)->Pack(szInputPath, szOutputPath);
	case INT_BMP:
		return sInstance(OZB)->Pack(szInputPath, szOutputPath);
	case INT_PNG:
		return sInstance(OZP)->Pack(szInputPath, szOutputPath);
	case INT_DDS:
		return sInstance(OZD)->Pack(szInputPath, szOutputPath);
	case INT_GFX:
		return sInstance(OZG)->Pack(szInputPath, szOutputPath);
	default:
		return FALSE;
	}
}

void FolderProcess(fs::path inputPath, fs::path outputPath)
{
	for (auto& iter : fs::directory_iterator(inputPath))
	{
		fs::path p = iter.path();
		fs::path p_out = outputPath;
		p_out += "\\";
		p_out += fs::path(p).filename();

		if (fs::is_directory(p))
		{
			FolderProcess(p, p_out);
		}
		else if (fs::is_regular_file(p))
		{
			if (ReplaceOutputExt(p_out))
			{
				if (!UnpackFile(p.string().c_str(), p_out.string().c_str()))
					PackFile(p.string().c_str(), p_out.string().c_str());
			}
		}
	}
}

int main(int argc, char** argv)
{
	const char* szInputPath = nullptr;
	const char* szOutputPath = nullptr;

	if (argc >= 2)
		szInputPath = argv[1];
	if (argc >= 3)
		szOutputPath = argv[2];

	if (!szInputPath)
	{
		cout << "\t Drag & Drop the file/folder \n";
		cout << "\t  or use console command to execute with the file path. \n";
		return EXIT_FAILURE;
	}

	if (!fs::exists(szInputPath))
	{
		cout << "Error: Input file/folder does not exist.\n" << endl;
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
		cout << "Error: Invalid file/folder path.\n" << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}